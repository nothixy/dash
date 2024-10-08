.PHONY: clean makebuilddir install

CC ?= cc

all: makebuilddir build/libdash.so

clean:
	rm -rf build/

makebuilddir:
	mkdir -p build/

build/libdash.so: dash.c dash.h
	${CC} -Wall -Wextra -fPIC -shared -o $@ $<

example: makebuilddir
	${CC} -Wall -Wextra -o build/example example.c dash.c
