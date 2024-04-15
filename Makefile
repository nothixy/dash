.PHONY: clean makebuilddir install

all: makebuilddir build/libargument_parser.so

clean:
	rm -rf build/

makebuilddir:
	mkdir -p build/

build/libargument_parser.so: argument_parser.c argument_parser.h
	gcc -fPIC -shared -o $@ $<

example: makebuilddir
	gcc -o build/example example.c argument_parser.c
