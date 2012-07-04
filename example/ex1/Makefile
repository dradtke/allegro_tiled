CC      := clang
TARGET  := Example1
CFLAGS  := -I../../include -g
LDFLAGS := -L../..
LIBS    := -lallegro -lallegro_image -lallegro_tiled
SOURCES := $(shell find src/ -type f -name "*.c")
OBJECTS := $(SOURCES:.c=.o)

$(TARGET): $(OBJECTS)
	@echo "  Linking..."; $(CC) $(LDFLAGS) $^ -o $(TARGET) $(LIBS)

%.o: %.c
	@echo "  CC $<"; $(CC) $(CFLAGS) -c -o $@ $<

clean:
	@echo "  Cleaning..."; $(RM) src/*.o $(TARGET)

run: $(TARGET)
	@echo "  Starting..."; LD_LIBRARY_PATH=../../ ./$(TARGET)

.PHONY: clean run
