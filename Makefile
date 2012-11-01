CC  	:= clang
LIBNAME := allegro_tiled
PKGS	:= allegro-5.0 allegro_image-5.0 libxml-2.0 zlib glib-2.0
CFLAGS  := -g -fPIC -Wall -Iinclude $(shell pkg-config --cflags $(PKGS))
LIBS    := $(shell pkg-config --libs $(PKGS))

TARGET  := lib$(LIBNAME).so
LDFLAGS := -shared -Wl,-soname=$(TARGET)
SOURCES := $(shell find src/ -type f -name *.c)
OBJECTS := $(patsubst src/%,build/%,$(SOURCES:.c=.o))
DEPS 	:= $(OBJECTS:.o=.deps)

PREFIX	= /usr/local
LIBDIR	= $(PREFIX)/lib$(if $(eq `uname -m`,"x86_64"),64)
INCDIR	= $(PREFIX)/include

HEADER	= "allegro5/allegro_tiled.h"
PCFILE	= "allegro_tiled-5.0.pc"

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@echo "  Linking..."; $(CC) $(LDFLAGS) $^ -o $(TARGET) $(LIBS)

build/%.o: src/%.c | init
	@echo "  CC $<"; $(CC) $(CFLAGS) -MD -MF $(@:.o=.deps) -c -o $@ $<

init:
	@mkdir -p build/

install: all
	@echo "  Installing..."
	@install -D -m 0644 "$(TARGET)" "$(DESTDIR)$(LIBDIR)/$(TARGET)"
	@install -D -m 0644 "include/$(HEADER)" "$(DESTDIR)$(INCDIR)/$(HEADER)"
	@cat "misc/$(PCFILE)" | sed 's#@LIBDIR@#$(LIBDIR)#g' | sed 's#@INCDIR@#$(INCDIR)#g' > "$(DESTDIR)$(TMPDIR)/$(PCFILE)"
	@install -D -m 0644 "$(DESTDIR)$(TMPDIR)/$(PCFILE)" "$(DESTDIR)$(LIBDIR)/pkgconfig/$(PCFILE)"
	@$(RM) "$(DESTDIR)$(TMPDIR)/$(PCFILE)"

uninstall:
	@echo "  Uninstalling..."
	@$(RM) "$(DESTDIR)$(LIBDIR)/$(TARGET)"
	@$(RM) "$(DESTDIR)$(INCDIR)/$(HEADER)"
	@$(RM) "$(DESTDIR)$(LIBDIR)/pkgconfig/$(PCFILE)"

clean:
	@echo "  Cleaning..."; $(RM) -r build/ $(TARGET)

-include $(DEPS)

.PHONY: all clean init install uninstall
