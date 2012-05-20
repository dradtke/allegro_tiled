CC  	:= clang
LIBNAME := allegro_tiled
CFLAGS  := -g -fPIC -Wall -Iinclude/
LIBS    := -lallegro -lallegro_image -lxml2 -lz 

TARGET  := lib$(LIBNAME).so
LDFLAGS := -shared -Wl,-soname=$(TARGET)
SOURCES := $(shell find src/ -type f -name *.c)
OBJECTS := $(patsubst src/%,build/%,$(SOURCES:.c=.o))

$(TARGET): $(OBJECTS)
	@echo "  Linking..."; $(CC) $(LDFLAGS) $^ -o $(TARGET) $(LIBS)

build/%.o: src/%.c
	@mkdir -p build/
	@echo "  CC $<"; $(CC) $(CFLAGS) -c -o $@ $<

clean:
	@echo "  Cleaning..."; $(RM) -r build/ $(TARGET)

.PHONY: clean
