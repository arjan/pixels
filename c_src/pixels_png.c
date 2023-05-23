#include "erl_nif.h"
#include <string.h>
#include "ext/lodepng.h"

ERL_NIF_TERM decode_png(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]) {
    ErlNifBinary binary;

    if (!enif_inspect_iolist_as_binary(env, argv[0], &binary)) {
        return enif_make_badarg(env);
    }

    unsigned error;
    unsigned width = 0, height = 0;
    unsigned char* image_data = 0;

    error = lodepng_decode32(&image_data, &width, &height, binary.data, binary.size);

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

ERL_NIF_TERM encode_png(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]) {
    ErlNifBinary binary;

    unsigned error;
    unsigned width = 0, height = 0;

    if (!enif_inspect_iolist_as_binary(env, argv[0], &binary)) {
        return enif_make_badarg(env);
    }

    if (!enif_get_uint(env, argv[1], &width)) {
        return enif_make_badarg(env);
    }

    if (!enif_get_uint(env, argv[2], &height)) {
        return enif_make_badarg(env);
    }

    unsigned char* png;
    size_t pngsize;

    error = lodepng_encode32(&png, &pngsize, binary.data, width, height);

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

    bindata_buf = enif_make_new_binary(env, pngsize, &bindata_term);
    memcpy(bindata_buf, png, pngsize);

    free(png);

    return enif_make_tuple2(
        env,
        enif_make_atom(env, "ok"),
        bindata_term
        );
}
