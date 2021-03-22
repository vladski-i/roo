CC = gcc
SRCDIR = src/impl
CFLAGS = -ljack -lm
OUTPUT_DIR = build
FILES = roo startup callbacks
SOURCES=$(patsubst %, $(SRCDIR)/%.c, $(FILES))
OBJECTS=$(patsubst %, $(OUTPUT_DIR)/%.o, $(FILES))
_dummy := $(shell mkdir -p build)

.DEFAULT_GOAL = roo

roo: $(OBJECTS)
	$(CC) $^ $(CFLAGS) -o $(OUTPUT_DIR)/$@

$(OUTPUT_DIR)/startup.o: $(SRCDIR)/startup.c
	$(CC) -c $^ -o $@

$(OUTPUT_DIR)/roo.o: $(SRCDIR)/roo.c
	$(CC) -c $^ -o $@

$(OUTPUT_DIR)/callbacks.o: $(SRCDIR)/callbacks.c
	$(CC) -c $^ -o $@

clean:
	rm -rf $(OUTPUT_DIR)

run: roo
	build/roo