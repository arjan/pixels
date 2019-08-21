
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

SOURCES := $(wildcard c_src/*.c)
SOURCES := $(SOURCES:.c=.o)

all: nif

ERLANG_PATH = $(shell erl -eval 'io:format("~s", [lists:concat([code:root_dir(), "/erts-", erlang:system_info(version), "/include"])])' -s init stop -noshell)
NIF_CFLAGS = -g -O3  -I"$(ERLANG_PATH)" -I"$(LIB_DIR)" -I"$(DEPS)/include"

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
	@$(RM) -r c_src/*.o "$(LIB_DIR)"/$(LIB).so* $(DEPS)

%.o: %.c
	$(CC) -c $(NIF_CFLAGS) -o $@ $<

nif: $(SOURCES)
	@mkdir -p $(LIB_DIR) || :
	$(CC) $^ $(NIF_LDFLAGS) -shared -o $(OUTPUT)
