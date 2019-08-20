
# Check that we're on a supported build platform
ifeq ($(CROSSCOMPILE),)
    # Not crosscompiling, so check that we're on Linux.
    ifneq ($(shell uname -s),Linux)
        $(error PNG only works on Linux. Crosscompiling is possible if $$CROSSCOMPILE is set.)
    endif
	BUILD := $(MAKE_HOST)
else
	BUILD := $(shell basename $(CROSSCOMPILE))
endif

DEPS := $(PWD)/_build/_ext/$(BUILD)

include external.mk

all: prepare-deps libpng nif

prepare-deps:
	@mkdir -p $(DEPS)

libpng_url := "http://prdownloads.sourceforge.net/libpng/libpng-1.6.37.tar.gz?download"
libpng_tar := libpng-1.6.37.tar.gz
libpng_vsn := 1.6.37

ERLANG_PATH = $(shell erl -eval 'io:format("~s", [lists:concat([code:root_dir(), "/erts-", erlang:system_info(version), "/include"])])' -s init stop -noshell)
NIF_CFLAGS = -g -O3  -I"$(ERLANG_PATH)" -I"$(LIB_DIR)" -I"$(DEPS)/include"
NIF_LDFLAGS += -lpng

ifeq ($(shell uname),Linux)
	NIF_LDFLAGS += -Wl,--no-whole-archive
endif

LIB = pixels_nif
LIB_DIR = $(PWD)/priv
OUTPUT = "$(LIB_DIR)"/$(LIB).so

ifneq ($(OS),Windows_NT)
	NIF_CFLAGS += -fPIC
endif

ifeq ($(shell uname),Darwin)
	NIF_LDFLAGS += -dynamiclib -undefined dynamic_lookup
endif


clean:
	@$(RM) -r "$(LIB_DIR)"/$(LIB).so* zlib* libpng* $(DEPS)

nif:
	@mkdir -p $(LIB_DIR) || :
	@$(CC) $(NIF_CFLAGS) -shared $(NIF_LDFLAGS) -o $(OUTPUT) c_src/$(LIB).c
