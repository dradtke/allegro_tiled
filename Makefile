CC		:= gcc
ALLEGRO := /opt/allegro/current
CFLAGS	:= -I$(ALLEGRO)/include -g
LDFLAGS := -L$(ALLEGRO)/lib
LIBS	:= -lallegro  -lxml2 -lz
SOURCES := $(shell find src/ -type f -name "*.c")
OBJECTS := $(SOURCES:.c=.o)
TARGET	:= main

all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS)
	@echo "  Linking..."; $(CC) $(LDFLAGS) $^ -o $@ $(LIBS)

%.o: %.c
	@echo "  CC $<"; $(CC) $(CFLAGS) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) src/*.o $(TARGET)

.PHONY: all clean
