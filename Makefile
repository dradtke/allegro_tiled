CC      := gcc
LIBNAME := allegro_tiled
CFLAGS  := -g -fPIC -Wall -Iinclude/
LDFLAGS := -shared -Wl,-soname,lib$(LIBNAME).so
LIBS    := -lallegro -lallegro_image -lxml2 -lz
SOURCES := $(shell find src/ -type f -name "*.c")
OBJECTS := $(patsubst src/%,build/%,$(SOURCES:.c=.o))


$(LIBNAME): $(OBJECTS)
	@echo "  Linking..."; $(CC) $(LDFLAGS) $^ -o lib$@.so $(LIBS)

build/%.o: src/%.c
	@mkdir -p build/
	@echo "  CC $<"; $(CC) $(CFLAGS) -c -o $@ $<

clean:
	@echo "  Cleaning..."; $(RM) -r build/ lib$(LIBNAME).so

.PHONY: clean
