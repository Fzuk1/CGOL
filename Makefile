CC = gcc

all: 
	@echo "Choose: 'make cgol' OR 'make run'"

cgol: src/*.c
	@${CC} $^ -o $@ -std=c99 -Wall -Wextra -I src/include `pkg-config --cflags --libs sdl2 SDL2_ttf`


run: 
	@./cgol
