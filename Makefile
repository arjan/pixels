LIB := pixels_nif

ifeq ($(ERL_EI_INCLUDE_DIR),)
   $(error Please run 'mix compile' instead of 'make')
endif

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

SOURCES := $(wildcard c_src/*.c c_src/ext/*.c)
SOURCES := $(SOURCES:.c=.o)

CFLAGS += -g -O3  -I"$(ERTS_INCLUDE_DIR)"

ifeq ($(shell uname),Linux)
	LDFLAGS += -Wl,--no-whole-archive
endif

ifneq ($(OS),Windows_NT)
	CFLAGS += -fPIC
endif

ifeq ($(shell uname),Darwin)
	LDFLAGS += -dynamiclib -undefined dynamic_lookup
endif

###

all: $(LIB)

clean:
	@$(RM) -r c_src/*.o "$(LIB_DIR)"/$(LIB).so* $(DEPS)

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

$(LIB): $(SOURCES)
	@mkdir -p $(MIX_APP_PATH)/priv || :
	$(CC) $^ $(LDFLAGS) -shared -o $(MIX_APP_PATH)/priv/$(LIB).so
