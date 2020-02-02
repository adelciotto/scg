CFLAGS := -Wall -Werror -Wextra -std=c99 -pedantic
LDFLAGS := -lm $(shell pkg-config --libs sdl2)
INCLUDES := $(shell pkg-config --cflags sdl2)

DEBUG ?= 0
ifeq ($(DEBUG), 1)
	CFLAGS += -g -DDEBUG
else
	CFLAGS += -O3 -DNDEBUG
endif

.PHONY: default
default: basic plasma image

basic: examples/basic.c scg.h
	$(CC) examples/basic.c -o basic $(CFLAGS) $(LDFLAGS) $(INCLUDES)

plasma: examples/plasma.c scg.h
	$(CC) examples/plasma.c -o plasma $(CFLAGS) $(LDFLAGS) $(INCLUDES)

image: examples/image.c scg.h
	$(CC) examples/image.c -o image $(CFLAGS) $(LDFLAGS) $(INCLUDES)

.PHONY: format
format:
	clang-format --verbose -i -style=file examples/*.c scg.h

.PHONY: clean
clean:
	rm -f basic plasma image
	rm -f **/*.o
	rm -rf *.dSYM
