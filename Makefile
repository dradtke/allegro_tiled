CC		:= gcc
CFLAGS	:= -g -Wall
LIBS	:= -lallegro -lxml2 -lz
SOURCES := $(shell find src/ -type f -name "*.c")
OBJECTS := $(SOURCES:.c=.o)
TARGET	:= main

all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS)
	@echo "  Linking..."; $(CC)  $^ -o $@ $(LIBS)

%.o: %.c
	@echo "  CC $<"; $(CC) $(CFLAGS) -c -o $@ $<

clean:
	@echo "  Cleaning..."; $(RM) src/*.o $(TARGET)

.PHONY: all clean
