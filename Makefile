NIF := pixels_nif

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

LIB := $(MIX_APP_PATH)/priv/$(NIF).so
SOURCES := $(wildcard c_src/*.c c_src/ext/*.c)
OBJECTS := $(SOURCES:.c=.o)

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
	@$(RM) -f $(OBJECTS) $(LIB)

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

$(LIB): $(OBJECTS)
	@mkdir -p $(MIX_APP_PATH)/priv || :
	$(CC) $^ $(LDFLAGS) -shared -o $(LIB)
