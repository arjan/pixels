#include "erl_nif.h"
#include <string.h>
#include "ext/lodepng.h"

static ERL_NIF_TERM _png_result(ErlNifEnv *env, unsigned error, unsigned char *image_data, unsigned width, unsigned height) {

    if (error) {
        // printf("error %u: %s\n", error, lodepng_error_text(error));
        return enif_make_tuple3(
            env,
            enif_make_atom(env, "error"),
            enif_make_int(env, error),
            enif_make_string(env, lodepng_error_text(error), ERL_NIF_LATIN1)
            );
    }

    ERL_NIF_TERM bindata_term;
    unsigned char * bindata_buf;

    bindata_buf = enif_make_new_binary(env, width * height * 4, &bindata_term);
    memcpy(bindata_buf, image_data, width * height * 4);

    free(image_data);

    return enif_make_tuple3(
        env,
        enif_make_int(env, width),
        enif_make_int(env, height),
        bindata_term
        );
}


ERL_NIF_TERM read_png_file(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]) {
    ErlNifBinary binary;

    if (!enif_inspect_iolist_as_binary(env, argv[0], &binary)) {
        return enif_make_badarg(env);
    }

    char filename[1000];

    strncpy(filename, binary.data, binary.size);
    filename[binary.size] = 0;

    unsigned error;
    unsigned width = 0, height = 0;
    unsigned char* image_data = 0;

    error = lodepng_decode32_file(&image_data, &width, &height, filename);
    return _png_result(env, error, image_data, width, height);
}

ERL_NIF_TERM read_png_buffer(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]) {
    ErlNifBinary binary;

    if (!enif_inspect_iolist_as_binary(env, argv[0], &binary)) {
        return enif_make_badarg(env);
    }

    unsigned error;
    unsigned width = 0, height = 0;
    unsigned char* image_data = 0;

    error = lodepng_decode32(&image_data, &width, &height, binary.data, binary.size);
    return _png_result(env, error, image_data, width, height);
}
