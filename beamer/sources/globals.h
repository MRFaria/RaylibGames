#ifndef GLOBALS_H
#define GLOBALS_H

const char TILE_WALL = '#';
const char TILE_EMPTY = '.';
const char TILE_FLOOD = 'F';

const int N_TILE_WIDTH = 64;
const int N_TILE_HEIGHT = N_TILE_WIDTH;
const int WALL_CHANCE = 45;
const int DEATH_LIMIT = 4; //if alive and num tiles around less
const int BIRTH_LIMIT = 4; //if dead and num tiles around more
const int NUM_PASSES = 10;

const int INITIAL_SCREEN_HEIGHT = 600;
const int INITIAL_SCREEN_WIDTH = 800;

const int TILE_WALL_OUTLINE_WIDTH = 4;

#endif
