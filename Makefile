CC         := gcc
LIBNAME    := allegro_tiled
CFLAGS     := -g -fPIC -Wall -Iinclude/
LDFLAGS    := -shared -Wl,-soname,lib$(LIBNAME).so
LIBS       := -lallegro -lallegro_image -lxml2 -lz
SOURCES := $(shell find src/ -type f -name "*.c")
OBJECTS := $(SOURCES:.c=.o)

$(LIBNAME): $(OBJECTS)
	@echo "  Linking..."; $(CC) $(LDFLAGS) $^ -o lib$@.so $(LIBS)

%.o: %.c
	@echo "  CC $<"; $(CC) $(CFLAGS) -c -o $@ $<

clean:
	@echo "  Cleaning..."; $(RM) src/*.o lib$(LIBNAME).so

.PHONY: clean
