CC      := g++
CFLAGS  := -Wall -std=c++20 -O3 -march=native -ffast-math $(shell pkg-config --cflags sdl2 SDL2_ttf)
LDFLAGS := $(shell pkg-config --libs sdl2 SDL2_ttf) -mconsole
SRC     := $(wildcard src/*.cpp)
OUT     := build/app.exe

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -rf build
.PHONY: all clean