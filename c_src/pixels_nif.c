#include "erl_nif.h"

#include <string.h>
#include <png.h>

void free_image_data(png_bytepp data, int height)
{
    for (int y=0; y<height; ++y)
    {
        free(data[y]);
        data[y] = NULL;
    }
    free(data);
}

#define E_NOTFOUND 1
#define E_INVALID 2
#define E_BADARG 3

unsigned int read_png_file_real(const char* file_name, int* rst_width, int* rst_height, int* rst_color_type, png_bytepp *retval)
{
    png_structp png_ptr;
    png_infop info_ptr;
    png_bytepp row_pointers;

    char header[8];    // 8 is the maximum size that can be checked

    /* open file and test for it being a png */
    FILE *fp = fopen(file_name, "rb");
    if (!fp) {
        return E_NOTFOUND;
    }
    fread(header, 1, 8, fp);

    if (png_sig_cmp(header, 0, 8)) {
        return E_INVALID;
    }

    /* initialize stuff */
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png_ptr) {
        return E_BADARG;
    }
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        return E_BADARG;
    }
    if (setjmp(png_jmpbuf(png_ptr))) {
        return E_BADARG;
    }
    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);

    png_read_info(png_ptr, info_ptr);

    *rst_width = png_get_image_width(png_ptr, info_ptr);
    *rst_height = png_get_image_height(png_ptr, info_ptr);
    *rst_color_type = png_get_color_type(png_ptr, info_ptr);

    png_set_interlace_handling(png_ptr);
    png_read_update_info(png_ptr, info_ptr);

    /* read file */
    if (setjmp(png_jmpbuf(png_ptr))) {
        return E_INVALID;
    }
    row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * *rst_height);
    for (int y=0; y<*rst_height; y++)
        row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(png_ptr,info_ptr));

    png_read_image(png_ptr, row_pointers);

    // clear png resource
    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

    // close file
    fclose(fp);

    *retval = row_pointers;
    return 0;
}



static ERL_NIF_TERM
read_png_file(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]) {
    ErlNifBinary binary;

    if (!enif_inspect_iolist_as_binary(env, argv[0], &binary)) {
        return enif_make_badarg(env);
    }

    int width = 0, height = 0, color_type = 0;

    char filename[1000];
    strncpy(filename, binary.data, binary.size);
    filename[binary.size] = 0;

    png_bytepp image_data;

    unsigned int ret = read_png_file_real(filename, &width, &height, &color_type, &image_data);

    switch (ret) {
    case E_NOTFOUND:
        return enif_make_atom(env, "not_found");
    case E_INVALID:
        return enif_make_atom(env, "invalid_format");
    case E_BADARG:
        return enif_make_badarg(env);
    default:
        break;
    }

    ERL_NIF_TERM bindata_term;
    unsigned char * bindata_buf;

    int b = 0;

    if (color_type == PNG_COLOR_TYPE_RGB) {
        b = 3;
    } else if (color_type == PNG_COLOR_TYPE_RGBA) {
        b = 4;
    } else {
        fprintf(stderr, "Unsupported PNG color type, use RGB or RGBA");
        return enif_make_badarg(env);
    }

    bindata_buf = enif_make_new_binary(env, width * height * b, &bindata_term);
    // memcpy(bindata_buf, image_data, width * height * 3);
    memset(bindata_buf, 0, width * height * b);

    if (color_type == PNG_COLOR_TYPE_RGB) {
        for (int y=0; y<height; y++) {
            png_byte* row = image_data[y];
            for (int x=0; x<width; x++) {
                png_byte* ptr = &(row[x*3]);
                bindata_buf[y * width + x * 3 + 0] = ptr[0];
                bindata_buf[y * width + x * 3 + 1] = ptr[1];
                bindata_buf[y * width + x * 3 + 2] = ptr[2];
            }
        }
    } else {
        for (int y=0; y<height; y++) {
            png_byte* row = image_data[y];
            for (int x=0; x<width; x++) {
                png_byte* ptr = &(row[x*4]);
                bindata_buf[y * width + x * 4 + 0] = ptr[0];
                bindata_buf[y * width + x * 4 + 1] = ptr[1];
                bindata_buf[y * width + x * 4 + 2] = ptr[2];
                bindata_buf[y * width + x * 4 + 3] = ptr[3];
            }
        }
    }

    free_image_data(image_data, height);

    return enif_make_tuple4(
        env,
        enif_make_int(env, width),
        enif_make_int(env, height),
        enif_make_int(env, color_type),
        bindata_term
        );
}

// Let's define the array of ErlNifFunc beforehand:
static ErlNifFunc nif_funcs[] = {
    // {erl_function_name, erl_function_arity, c_function}
    {"read_png_file", 1, read_png_file}
};

ERL_NIF_INIT(Elixir.Pixels.Nif, nif_funcs, NULL, NULL, NULL, NULL)
