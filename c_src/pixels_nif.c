#include "erl_nif.h"
#include "pixels_nif.h"

// Let's define the array of ErlNifFunc beforehand:
static ErlNifFunc nif_funcs[] = {
    // {erl_function_name, erl_function_arity, c_function}
    {"read_png_file", 1, read_png_file},
    {"read_png_buffer", 1, read_png_buffer}
};

ERL_NIF_INIT(Elixir.Pixels.Nif, nif_funcs, NULL, NULL, NULL, NULL)
