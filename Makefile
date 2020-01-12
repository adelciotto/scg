CFLAGS := -Wall -Werror -Wextra -std=c99 -pedantic -g
LDFLAGS := -lm $(shell pkg-config --libs sdl2)
INCLUDES := $(shell pkg-config --cflags sdl2)

.PHONY: default
default: basic

basic: examples/basic.c scg.h
	$(CC) examples/basic.c -o basic $(CFLAGS) $(LDFLAGS) $(INCLUDES)

.PHONY: format
format:
	clang-format --verbose -i -style=file examples/*.c scg.h

.PHONY: clean
clean:
	rm -f basic
	rm -f **/*.o
	rm -rf *.dSYM
