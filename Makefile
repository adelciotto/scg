CFLAGS := -Wall -Werror -Wextra -std=c99 -pedantic
LDFLAGS := -lm $(shell pkg-config --libs sdl2)
INCLUDES := $(shell pkg-config --cflags sdl2)

DEBUG ?= 0
ifeq ($(DEBUG), 1)
	CFLAGS += -g -DDEBUG
else
	CFLAGS += -O2 -DNDEBUG
endif

PROFILE ?= 0
ifeq ($(PROFILE), 1)
	CFLAGS += -pg
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
	voxel_space

.PHONY: default
default: $(EXAMPLES)

basic: examples/basic.c scg.h
	$(CC) examples/basic.c -o basic $(CFLAGS) $(LDFLAGS) $(INCLUDES)

audio: examples/audio.c scg.h
	$(CC) examples/audio.c -o audio $(CFLAGS) $(LDFLAGS) $(INCLUDES)

plasma: examples/plasma.c scg.h
	$(CC) examples/plasma.c -o plasma $(CFLAGS) $(LDFLAGS) $(INCLUDES)

image: examples/image.c scg.h
	$(CC) examples/image.c -o image $(CFLAGS) $(LDFLAGS) $(INCLUDES)

transform: examples/transform.c scg.h
	$(CC) examples/transform.c -o transform $(CFLAGS) $(LDFLAGS) $(INCLUDES)

rotozoom: examples/rotozoom.c scg.h
	$(CC) examples/rotozoom.c -o rotozoom $(CFLAGS) $(LDFLAGS) $(INCLUDES)

tunnel: examples/tunnel.c scg.h
	$(CC) examples/tunnel.c -o tunnel $(CFLAGS) $(LDFLAGS) $(INCLUDES)

starfield: examples/starfield.c scg.h
	$(CC) examples/starfield.c -o starfield $(CFLAGS) $(LDFLAGS) $(INCLUDES)

starfield_3d: examples/starfield_3d.c scg.h
	$(CC) examples/starfield_3d.c -o starfield_3d $(CFLAGS) $(LDFLAGS) $(INCLUDES)

voxel_space: examples/voxel_space.c scg.h
	$(CC) examples/voxel_space.c -o voxel_space $(CFLAGS) $(LDFLAGS) $(INCLUDES)

.PHONY: format
format:
	clang-format --verbose -i -style=file examples/*.c scg.h

.PHONY: clean
clean:
	rm -f $(EXAMPLES)
	rm -f **/*.o
	rm -rf *.dSYM
	rm -f gmon.out
