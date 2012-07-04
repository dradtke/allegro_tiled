CC  	:= clang
LIBNAME := allegro_tiled
PKGS	:= allegro-5.0 allegro_image-5.0 libxml-2.0 zlib
CFLAGS  := -g -fPIC -Wall -Iinclude `pkg-config --cflags $(PKGS)`
LIBS    := `pkg-config --libs $(PKGS)`

TARGET  := lib$(LIBNAME).so
LDFLAGS := -shared -Wl,-soname=$(TARGET)
SOURCES := $(shell find src/ -type f -name *.c)
OBJECTS := $(patsubst src/%,build/%,$(SOURCES:.c=.o))
DEPS 	:= $(OBJECTS:.o=.deps)

$(TARGET): $(OBJECTS)
	@echo "  Linking..."; $(CC) $(LDFLAGS) $^ -o $(TARGET) $(LIBS)

build/%.o: src/%.c
	@mkdir -p build/
	@echo "  CC $<"; $(CC) $(CFLAGS) -MD -MF $(@:.o=.deps) -c -o $@ $<

compile: $(TARGET)

clean:
	@echo "  Cleaning..."; $(RM) -r build/ $(TARGET)

-include $(DEPS)

.PHONY: clean compile
