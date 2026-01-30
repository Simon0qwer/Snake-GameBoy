#include <gb/gb.h>
#include <stdint.h>
#include <stdio.h> 
#include <stdbool.h>
#include <rand.h>
#include <stdlib.h>
#include <time.h>

#include "assets/snake_head.h"
#include "assets/coin_sprite.h"

#include "assets/snake_game_tiles.h"
#include "assets/snake_game_map.h"

// Map size = GameBoy screen
// 160px x 144px or 20 tiles x 18 tiles
// #define mapWidth 20
// #define mapHeight 18

#define BORDER_LEFT   16
#define BORDER_RIGHT  152
#define BORDER_TOP    24
#define BORDER_BOTTOM 120

#define DIR_NONE  0
#define DIR_RIGHT 1
#define DIR_LEFT  2
#define DIR_UP    3
#define DIR_DOWN  4

#define MOVE_DELAY 30
uint8_t dir = DIR_NONE;
uint8_t joy;

uint8_t snake_x = 80;
uint8_t snake_y = 72;
uint8_t next_snake_x = 0;
uint8_t next_snake_y = 0;

uint8_t coin_x = 55;
uint8_t coin_y = 55;

uint8_t move_timer = 0;

bool is_running = 0;

uint8_t r,seed,key;

void input_handler() {
    joy = joypad();
    
    // & - bitwise AND operation && - logical AND operation
    if ((joy & J_RIGHT) && dir != DIR_LEFT) {
        dir = DIR_RIGHT;    
        set_sprite_tile(0, 1);
        set_sprite_prop(0, S_FLIPX);
    }
    if ((joy & J_LEFT) && dir != DIR_RIGHT) {
        dir = DIR_LEFT;
        set_sprite_tile(0, 1);
        set_sprite_prop(0, 0);
    }
    if ((joy & J_UP) && dir != DIR_DOWN) {
        dir = DIR_UP;
        set_sprite_tile(0, 0);
        set_sprite_prop(0, S_FLIPY);
    }
    if ((joy & J_DOWN) && dir != DIR_UP) {
        dir = DIR_DOWN;
        set_sprite_tile(0, 0);
        set_sprite_prop(0, 0);
            
    }

}

void game_over(){
    is_running = 0;
    HIDE_SPRITES;
    printf("\n\n\n\n\n\n\n\n\n === GAME OVER ===");
}

void snake_move() {
    next_snake_x = snake_x;
    next_snake_y = snake_y;

    switch(dir) {
        case DIR_RIGHT: next_snake_x += 8; 
        break;
        case DIR_LEFT:  next_snake_x -= 8; 
        break;                
        case DIR_UP:    next_snake_y -= 8;
        break;
        case DIR_DOWN:  next_snake_y += 8; 
        break;
        default: return;
    }

    if (next_snake_x < BORDER_LEFT  ||
        next_snake_x > BORDER_RIGHT ||
        next_snake_y < BORDER_TOP   ||
        next_snake_y > BORDER_BOTTOM) {
        dir = DIR_NONE;
        game_over();
    }

    else{
        snake_x = next_snake_x;
        snake_y = next_snake_y;
        move_sprite(0, snake_x, snake_y);
    }

}

void init_random() {

    seed=0;
    key=0;
    r=0;

    while(!joypad()){seed++; if(seed>=255)seed=1;}  
    initrand(seed);                    
}


uint8_t get_random_between(uint8_t min, uint8_t max) {
    return (uint8_t)(rand() % (max - min + 1)) + min;
}


void spawn_coin(){
    uint8_t tile_x = get_random_between(3, 19);
    uint8_t tile_y = get_random_between(4, 15);

    move_sprite(1, tile_x * 8, tile_y * 8); // setting coin spawning point
}

void main() {
    
    SPRITES_8x8;

    set_sprite_data(0, 2, snake_head); 

    set_sprite_data(2, 1, coin_sprite); 
    // 2 = index of first tile for coin sprite 
    // 1 = number of tiles for coin sprite
    set_sprite_tile(1, 2);
    // 1 = sprite number for coin
    // 2 = tile index for coin sprite

    move_sprite(0, snake_x, snake_y); // setting snake spawning point

    set_bkg_data(0, 8, snake_game_tiles);
    set_bkg_tiles(0, 0, 20, 18, snake_game_map);

    is_running = 1;

    SHOW_SPRITES;
    SHOW_BKG;
    DISPLAY_ON;

    init_random();
    spawn_coin();

    while(is_running) {
        
        wait_vbl_done();     

        input_handler();     

        move_timer++; // Add to timer each frame
        if (move_timer >= MOVE_DELAY) { 
            snake_move(); // call function move snake on every 30 frames 
            move_timer = 0; // reset timer
        }
    }
}

