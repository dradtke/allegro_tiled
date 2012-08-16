CC  	:= clang
LIBNAME := allegro_tiled
PKGS	:= allegro-5.0 allegro_image-5.0 libxml-2.0 zlib glib-2.0
CFLAGS  := -g -fPIC -Wall -Iinclude `pkg-config --cflags $(PKGS)`
LIBS    := `pkg-config --libs $(PKGS)`

TARGET  := lib$(LIBNAME).so
LDFLAGS := -shared -Wl,-soname=$(TARGET)
SOURCES := $(shell find src/ -type f -name *.c)
OBJECTS := $(patsubst src/%,build/%,$(SOURCES:.c=.o))
DEPS 	:= $(OBJECTS:.o=.deps)

PREFIX	= /usr/local
LIBDIR	= $(PREFIX)/lib

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@echo "  Linking..."; $(CC) $(LDFLAGS) $^ -o $(TARGET) $(LIBS)

build/%.o: src/%.c | init
	@echo "  CC $<"; $(CC) $(CFLAGS) -MD -MF $(@:.o=.deps) -c -o $@ $<

init:
	@mkdir -p build/

install: all
	@echo "  Installing..."; install -D -m 0644 "$(TARGET)" "$(DESTDIR)$(LIBDIR)/$(TARGET)"

uninstall:
	@echo "  Uninstalling..."; $(RM) "$(DESTDIR)$(LIBDIR)/$(TARGET)"

clean:
	@echo "  Cleaning..."; $(RM) -r build/ $(TARGET)

-include $(DEPS)

.PHONY: all clean install uninstall
