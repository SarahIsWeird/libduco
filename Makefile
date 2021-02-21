LIB_PATH := /usr/lib/libduco.so
HEADER_PATH := /usr/include/duco.h

SRC := src/duco.c
HEADER := include/duco.h

OBJ := build/duco.o
TARGET := build/libduco.so

.PHONY: clean

all: $(OBJ)
	gcc -shared -o $(TARGET) $(OBJ)

build/%.o: src/%.c
	@mkdir -p build/
	gcc -c -Wall -Werror -fpic -std=c89 -o $@ $<

install: $(TARGET)
	cp $(TARGET) $(LIB_PATH)
	chmod 0755 $(LIB_PATH)
	cp $(HEADER) $(HEADER_PATH)
	ldconfig

uninstall:
	rm -f $(LIB_PATH)
	rm -f $(HEADER_PATH)

clean:
	rm -r build/