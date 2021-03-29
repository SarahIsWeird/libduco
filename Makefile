SRC := src/duco.c
HEADER := include/duco.h
OBJ := build/duco.o

ifeq ($(OS),Windows_NT)
TARGET := build/duco.dll
LINK_FLAGS := -lws2_32
else
TARGET := build/libduco.so
LIB_PATH := /usr/lib/libduco.so
HEADER_PATH := /usr/include/duco.h
LINK_FLAGS :=
endif

.PHONY: clean

all: $(OBJ)
	gcc -shared -o $(TARGET) $(OBJ) $(LINK_FLAGS)

build/%.o: src/%.c
	@mkdir -p build/
	gcc -c -Wall -Werror -fpic -std=c89 -o $@ $<

ifneq ($(OS),Windows_NT) # Can't (un)install on Windows.
install: $(TARGET)
	cp $(TARGET) $(LIB_PATH)
	chmod 0755 $(LIB_PATH)
	cp $(HEADER) $(HEADER_PATH)
	ldconfig

uninstall:
	rm -f $(LIB_PATH)
	rm -f $(HEADER_PATH)
endif

clean:
	rm -r build/