CFLAGS := -Wall -Werror -Wextra -std=c++11 -pedantic
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
	seabug \
	mouse \
	tween

.PHONY: default
default: $(EXAMPLES)

$(EXAMPLES): %:examples/%.cpp scg.h
	$(CXX) $< -o $@ $(CFLAGS) $(LDFLAGS) $(INCLUDES)

.PHONY: format
format:
	clang-format --verbose -i -style=file scg.h

.PHONY: clean
clean:
	rm -f $(EXAMPLES)
	rm -f **/*.o
	rm -rf *.dSYM
	rm -f gmon.out
