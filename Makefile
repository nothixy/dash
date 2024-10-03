.PHONY: clean makebuilddir install

CC ?= cc

all: makebuilddir build/libargument_parser.so

clean:
	rm -rf build/

makebuilddir:
	mkdir -p build/

build/libargument_parser.so: argument_parser.c argument_parser.h
	${CC} -Wall -Wextra -fPIC -shared -o $@ $<

example: makebuilddir
	${CC} -Wall -Wextra -o build/example example.c argument_parser.c
