CFLAGS := -Wall -Werror -Wextra -std=c99 -pedantic
LDFLAGS := -lm $(shell pkg-config --libs sdl2)
INCLUDES := $(shell pkg-config --cflags sdl2)

DEBUG ?= 0
ifeq ($(DEBUG), 1)
	CFLAGS += -g -DDEBUG
else
	CFLAGS += -O2 -DNDEBUG
endif

EXAMPLES := \
	basic \
	audio \
	plasma \
	image \
	transform \
	rotozoom \
	tunnel \
	starfield \
	starfield_3d \
	voxel_space \
	matrix \
	metablobs \
	seabug

.PHONY: default
default: $(EXAMPLES)

$(EXAMPLES): %:examples/%.c scg.h
	$(CC) $< -o $@ $(CFLAGS) $(LDFLAGS) $(INCLUDES)

.PHONY: format
format:
	clang-format --verbose -i -style=file examples/*.c scg.h

.PHONY: clean
clean:
	rm -f $(EXAMPLES)
	rm -f **/*.o
	rm -rf *.dSYM
	rm -f gmon.out
