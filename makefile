CC=clang

CFLAGS=-Wall -Wextra -Werror -std=c99 -pedantic -Wno-gnu-zero-variadic-macro-arguments -Wfloat-equal -Wpointer-arith -Wconversion -Wuninitialized -Winit-self -Wcast-align -Wunreachable-code
INCLUDE=-I.

RELFLAGS=-O3
DBGFLAGS=-g

VGFLAGS=-q --leak-check=full --show-leak-kinds=all

TESTS=$(shell find test/ -type f -name '*.c')

.PHONY: clean leaktest test all

all: test

clean:
	rm -fr out/

release: clean out/rel/common.o out/rel/reader.o out/rel/writer.o

test: out/bin/test
	./out/bin/test

leaktest: out/bin/test
	valgrind $(VGFLAGS) out/bin/test

out/rel/%.o: src/%.c src/%.h src/common.h
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(RELFLAGS) $(INCLUDE) -c $< -o $@

out/dbg/%.o: src/%.c src/%.h src/common.h
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(DBGFLAGS) $(INCLUDE) -c $< -o $@

out/dbg/test.o: test/test.c test/unit.h  src/reader.h src/writer.h $(TESTS)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(DBGFLAGS) $(INCLUDE) -c $< -o $@

out/bin/test: out/dbg/common.o out/dbg/reader.o out/dbg/writer.o out/dbg/test.o
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(DBGFLAGS) $(INCLUDE) $^ -o $@
