/*
 * Implementing Conway's Game of Life.
 */

/* INCLUDES */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>

#include <SDL2/SDL.h>

/* DEFINES */
// Window
#define WIDTH 1280
#define HEIGHT 900

// Render
#define RENDER_HORI_START ((WIDTH - HEIGHT) / 2)
#define RENDER_HORI_END (WIDTH - ((WIDTH - HEIGHT) / 2))
#define RENDER_WIDTH (RENDER_HORI_END - RENDER_HORI_START)
#define RENDER_HEIGHT HEIGHT

// Game
#define NUM_CELLS 150
#define CELL_H (floor(HEIGHT / NUM_CELLS))
#define CELL_W CELL_H

// Color
#define BLACK 0x00000000
#define WHITE 0xFFFFFFFF

/* STRUCTS */
typedef struct Window {
	SDL_Window *win;
	SDL_Surface *surf;
} Window_t;

typedef struct Position {
	uint32_t x, y;
} Position_t;

typedef struct Cell {
	Position_t pos;
	uint8_t state;
} Cell_t;

typedef struct Grid {
	Cell_t state[NUM_CELLS][NUM_CELLS];
} Grid_t;

/* GLOBALS */
static Window_t Window;
static Grid_t Grid;

/* FUNCTIONS */
void render_grid() {
	/*
	  Render background grid.
	*/

	SDL_Rect dstRect;

	// Vertical lines
	dstRect.w = 1;
	dstRect.h = RENDER_HEIGHT;
	dstRect.y = 0;
	for (size_t x = RENDER_HORI_START; x < RENDER_HORI_END; x += CELL_W) {
		dstRect.x = x;
		SDL_FillRect(Window.surf, &dstRect, BLACK);
	}
	dstRect.x = RENDER_HORI_END;
	SDL_FillRect(Window.surf, &dstRect, BLACK);

	// Horizontal lines
	dstRect.w = RENDER_WIDTH + 1;
	dstRect.h = 1;
	dstRect.x = RENDER_HORI_START;
	for (size_t y = CELL_H; y < RENDER_HEIGHT; y += CELL_H) {
		dstRect.y = y - 1;
		SDL_FillRect(Window.surf, &dstRect, BLACK);
	}

	return;
}

void render_fill_cell(Position_t pos, uint32_t color) {
	/*
	  Fill a single cell at position (x, y).
	*/

	// Check if cell is in the grid
	if (!((pos.x < NUM_CELLS) && (pos.y < NUM_CELLS))) {
		printf("Out of bounds: (%d, %d)\n", pos.x, pos.y);
		return;
	}

	// Fill cell (x, y) with color
	SDL_Rect dstRect;
	dstRect.w = CELL_W;
	dstRect.h = CELL_H;
	dstRect.x = RENDER_HORI_START + (pos.x * CELL_W);
	dstRect.y = (pos.y * CELL_H);

	SDL_FillRect(Window.surf, &dstRect, color);

	return;
}

void game_update(Cell_t *cell) {
	/*
	  Update cells based on rules.
	*/

	// Rule 1: Keep living if, 2 or 3 living neighbors (state = 0, means alive)
	// Rule 2: Get born if, exactly 3 living neighbors
	// Rule 3: Die if, less than 2 or more than 3 living neighbors

	// Get neighbors of the current cell
	Cell_t *n0 = &Grid.state[(cell->pos.y - 1) % NUM_CELLS][(cell->pos.x - 1) % NUM_CELLS];
	Cell_t *n1 = &Grid.state[(cell->pos.y - 1) % NUM_CELLS][(cell->pos.x) % NUM_CELLS];
	Cell_t *n2 = &Grid.state[(cell->pos.y - 1) % NUM_CELLS][(cell->pos.x + 1) % NUM_CELLS];
	Cell_t *n3 = &Grid.state[(cell->pos.y) % NUM_CELLS][(cell->pos.x - 1) % NUM_CELLS];
	Cell_t *n4 = &Grid.state[(cell->pos.y) % NUM_CELLS][(cell->pos.x + 1) % NUM_CELLS];
	Cell_t *n5 = &Grid.state[(cell->pos.y + 1) % NUM_CELLS][(cell->pos.x - 1) % NUM_CELLS];
	Cell_t *n6 = &Grid.state[(cell->pos.y + 1) % NUM_CELLS][(cell->pos.x) % NUM_CELLS];
	Cell_t *n7 = &Grid.state[(cell->pos.y + 1) % NUM_CELLS][(cell->pos.x + 1) % NUM_CELLS];

	// Put them into an array to iterate over it
	Cell_t *arr[8] = {n0, n1, n2, n3, n4, n5, n6, n7};

	// Count alive neighbors of current cell
	size_t aliveNeighCtr = 0;
	for (size_t i = 0; i < 8; i++) {
		if (!arr[i]->state) {
			aliveNeighCtr++;
		}
	}

	if (aliveNeighCtr == 2) {
		// Rule 1: Stay alive
		return;
	}
	else if (aliveNeighCtr == 3) {
		if (!cell->state) {
			// Rule 1: Stay alive
			return;
		}
		else {
			// Rule 2: Get born
			cell->state = 0;
		}
	}
	else if (aliveNeighCtr < 2 || aliveNeighCtr > 3) {
		// Rule 3: Die
		cell->state = 1;
	}

	return;
}

void game_init() {
	/*
	  Init all Cells on the game board.
	  TODO: Load initial board config from a file.
	*/

	// Here: random initialization
	for (size_t y = 0; y < NUM_CELLS; y++) {
		for (size_t x = 0; x < NUM_CELLS; x++) {
			Grid.state[y][x].state = rand() % 15;
			Grid.state[y][x].pos = (Position_t) {x, y};
		}
	}

	return;
}

int main() {

	// Init randomness for init game state
	srand(time(NULL));

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("%s\n", SDL_GetError());
		return 1;
	}

	Window.win = SDL_CreateWindow("Conway's Game of Life", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
	if (!Window.win) {
		printf("%s\n", SDL_GetError());
		return 1;
	}

	Window.surf = SDL_GetWindowSurface(Window.win);
	if (!Window.surf) {
		printf("%s\n", SDL_GetError());
		return 1;
	}

	// Init game
	game_init();
	
	uint8_t running = 1;
	SDL_Event event;
	while (running) {
		/* SDL events */
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				running = 0;
				break;
			}
		}

		/* Game events */
		// Background white
		SDL_FillRect(Window.surf, NULL, WHITE);

		render_grid();

		// Loop over all cells
		for (size_t y = 0; y < NUM_CELLS; y++) {
			// Render current state
			for (size_t x = 0; x < NUM_CELLS; x++) {
				if (!Grid.state[y][x].state) {
					render_fill_cell(Grid.state[y][x].pos, BLACK);
				}

				// Update cells
				game_update(&Grid.state[y][x]);
			}
		}

		// Show current state
		SDL_UpdateWindowSurface(Window.win);

		// Game speed in ms
		SDL_Delay(1000);
	}

	SDL_DestroyWindowSurface(Window.win);
	SDL_DestroyWindow(Window.win);
	SDL_Quit();
	
	return 0;
}

