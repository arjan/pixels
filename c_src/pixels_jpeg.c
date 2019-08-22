#include <string.h>
#ifndef max
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif

#include "erl_nif.h"
#include "ext/ujpeg.h"

ERL_NIF_TERM decode_jpeg(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]) {
    ErlNifBinary binary;

    if (!enif_inspect_iolist_as_binary(env, argv[0], &binary)) {
        return enif_make_badarg(env);
    }

    ujImage uj = ujCreate();
    ujSetChromaMode(uj, UJ_CHROMA_MODE_ACCURATE);

    ujDecode(uj, binary.data, binary.size);
    if (!uj) {
        return enif_make_tuple2(
            env,
            enif_make_atom(env, "error"),
            enif_make_atom(env, "decode_failed")
            );
    }

    ERL_NIF_TERM bindata_term;
    unsigned char * bindata_buf;

    int width = ujGetWidth(uj);
    int height = ujGetHeight(uj);

    char *image_data = ujGetImage(uj, NULL);

    if (!image_data) {
        // conversion failed
        printf("Image conversion failed - progressive or lossless JPEG?\n");
        free(uj);
        return enif_make_tuple2(
            env,
            enif_make_atom(env, "error"),
            enif_make_atom(env, "conversion_failed")
            );
    }

    bindata_buf = enif_make_new_binary(env, width * height * 4, &bindata_term);

    if (ujIsColor(uj)) {
        // RGB -> RGBA
        for (int y=0; y<height; y++) {
            for (int x=0; x<width; x++) {
                bindata_buf[y * width + x * 4 + 0] = image_data[y * width + x * 3 + 0];
                bindata_buf[y * width + x * 4 + 1] = image_data[y * width + x * 3 + 1];
                bindata_buf[y * width + x * 4 + 2] = image_data[y * width + x * 3 + 2];
                bindata_buf[y * width + x * 4 + 3] = 255;
            }
        }
    } else {
        // Gray -> RGBA
        for (int y=0; y<height; y++) {
            for (int x=0; x<width; x++) {
                bindata_buf[y * width + x * 4 + 0] = image_data[y * width + x];
                bindata_buf[y * width + x * 4 + 1] = image_data[y * width + x];
                bindata_buf[y * width + x * 4 + 2] = image_data[y * width + x];
                bindata_buf[y * width + x * 4 + 3] = 255;
            }
        }
    }

    free(uj);
    free(image_data);

    return enif_make_tuple3(
        env,
        enif_make_int(env, width),
        enif_make_int(env, height),
        bindata_term
        );
}
