#include <stdio.h>
#include <stdlib.h>
#define ASTEROID_COUNT 10

volatile int *const VG_ADDR = (int *)0x11100000;
volatile int *const VG_COLOR = (int *)0x11140000;
volatile int *const GYRO_X = (int *)0x11080000;
volatile int *const GYRO_Y = (int *)0x11090000;
volatile int *const GYRO_Z = (int *)0x110a0000;

//Resets all positions and gets game ready to start
void initGame();
//Generates random number between upper and lower
int random_from(int lower, int upper);
//Draws spaceship at position where position is the upper lefthand corner of the ship
void drawSpaceship(int position[2]);
//Draws spaceship at position where position is the upper lefthand corner of the ship
void drawAsteroid(int position[2]);
//iterates through asteroid positions and sees if they come in contact with ship
int checkCollision(int spaceshipPosition[2], int asteroidPositions[ASTEROID_COUNT][2]);
//delays animation loop x miliseconds
void delay(int ms);
void resetAsteroid(int *position);

//A
void draw_dot(int X, int Y, int color);
static void draw_horizontal_line(int X, int Y, int toX, int color);
static void draw_vertical_line(int X, int Y, int toY, int color);
//updates the asteroid position as its moving down the screen
void updateAsteroid(int position[2]);
//set up background
void draw_background();

int bgd_color = 0x286A;

int screen_width = 80;
int screen_height = 60;
//Determines if the player's ship hasn't been hit (0 when hit)
int alive = 1;
int spaceship_pos[2];
//An array of asteroid positions
//int astrd_cnt = 10;
int asteroids[10][2] = {0};

int ship_width = 3;
int ship_height = 5;
int asteroid_width = 4;
int asteroid_height = 4;

int random_from(int lower, int upper)
{
    return (rand() %
            (upper - lower + 1)) +
           lower;
}

void delay(int ms)
{
    int count = ms * 12500;
    volatile int i;
    for (i = 0; i < count; i++)
    {
    };
}

int checkCollision(int ship_position[2], int asteroid_positions[ASTEROID_COUNT][2])
{
    for (int i = 0; i < ASTEROID_COUNT; i++)
    {
        int x = asteroid_positions[i][0];
        int y = asteroid_positions[i][1];

        //{x differences},{y difference}
        int x_corner_dif[2] = {x + asteroid_width - ship_position[0], x - (ship_position[0] + ship_width)};
        int x_overlap = abs(x_corner_dif[0] + x_corner_dif[1]) < abs(x_corner_dif[0]) + abs(x_corner_dif[1]);
        int y_corner_dif[2] = {y + asteroid_height - ship_position[1], y - (ship_position[1] + ship_height)};
        int y_overlap = abs(y_corner_dif[0] + y_corner_dif[1]) < abs(y_corner_dif[0]) + abs(y_corner_dif[1]);
        if (x_overlap && y_overlap)
            return 1;
    }
    return 0;
}

//A
void initGame()
{
    //since spaceship pos is top left corner of ship, have to calc the position based of the width/height
    int bottom_padding = 3;
    spaceship_pos[0] = (79 - ship_width / 2) / 2;
    spaceship_pos[1] = 59 - ship_height - bottom_padding;
    for (int i = 0; i < ASTEROID_COUNT; i++)
    {
        asteroids[i][0] = 0;
        asteroids[i][1] = 0;
    }
}

//A
void updateAsteroid(int *position)
{
    //lowers the asteroid by one bit then randomly generates the x position
    position[1] = position[1] + 1;
    int lower = position[0] - 1;
    int upper = position[0] + 1;
    int x = random_from(lower, upper);
    if (x < 0)
    {
        x = 0;
    }
    else if (x < screen_width)
    {
        x = screen_width - 1;
    }
    position[0] = x;

    if (position[1] > screen_height)
        resetAsteroid(position);

    drawAsteroid(position);
}

void resetAsteroid(int *position)
{
    position[0] = random_from(0, screen_width);
    position[1] = 0;
}

//A
void drawAsteroid(int position[2])
{
    //draws a block asteroid with dimensions asteroid_width by asteroid_height
    //for every row of the asteroid
    //colors
    int highlight = 0xC5BE;
    int midtone = 0xAD1B;
    int shadow = 0x9C78;
    //Highlights
    draw_horizontal_line(position[0] + 1, position[1], position[0] + 2, highlight);
    draw_vertical_line(position[0], position[1] + 1, position[1] + 2, highlight);
    //mid
    draw_horizontal_line(position[0] + 1, position[1] + 1, position[0] + 2, 0xC5BE);
    draw_horizontal_line(position[0] + 1, position[1] + 2, position[0] + 2, 0xC5BE);
    //shadow
    draw_horizontal_line(position[0] + 1, position[1] + 3, position[0] + 2, shadow);
    draw_vertical_line(position[0] + 3, position[1] + 1, position[1] + 2, shadow);
}

//A
void draw_background()
{
    for (int i = 0; i < screen_height; i++)
    {
        for (int j = 0; j < screen_width; j++)
        {
            draw_dot(j, i, bgd_color);
        }
    }
}

static void draw_horizontal_line(int X, int Y, int toX, int color)
{
    toX++;
    for (; X != toX; X++)
    {
        draw_dot(X, Y, color);
    }
}

static void draw_vertical_line(int X, int Y, int toY, int color)
{
    toY++;
    for (; Y != toY; Y++)
    {
        draw_dot(X, Y, color);
    }
}

//A
void draw_dot(int X, int Y, int color)
{
    *VG_ADDR = (Y << 7) | X;
    *VG_COLOR = color;
}

void updateSpaceship()
{
    spaceship_pos[0] += *GYRO_X % screen_width;
    // spaceship_pos[1] += *GYRO_Y * .0001;
}

//A
void drawSpaceship(int position[2])
{
    int highlight = 0xFC51;
    int midtone = 0xFACB;
    int shadow = 0xE2EB;

    //Highlights
    draw_vertical_line(position[0], position[1], position[1] + 1, highlight);
    draw_vertical_line(position[0], position[1] + 3, position[1] + 4, highlight);
    draw_dot(position[0] + 1, position[1], highlight);
    //midtone
    draw_vertical_line(position[0] + 1, position[1] + 1, position[1] + 2, midtone);
    //shadow
    draw_vertical_line(position[0] + 2, position[1], position[1] + 1, shadow);
    draw_vertical_line(position[0] + 2, position[1] + 3, position[1] + 4, shadow);
}

int main(void)
{

    initGame();

    int frame_delay = 32; //32 ms
    int asteroid_timer = 0;
    int asteroid_count = 0; //how many asteroids are on the screen
    //asteroid count also serves as the asteroid number when updating the array
    while (alive)
    {
        draw_background();
        updateSpaceship();
        alive = checkCollision(spaceship_pos, asteroids);
        if (asteroid_timer > 1000) //updates the asteroid after 1000ms
        {
            asteroid_timer = 0;
            //draws a new asteroid if there are less than 10 asteroids
            if (asteroid_count <= 10)
            {
                int position[2] = {random_from(1, 80 - asteroid_width - 1), 0};
                asteroids[asteroid_count][0] = position[0];
                asteroids[asteroid_count][1] = position[1];
                drawAsteroid(position);
                asteroid_count = asteroid_count++;
            }

            //updates all asteroids that currently exist
            for (int a = 0; a < asteroid_count + 1; a++)
            {
                updateAsteroid(asteroids[a]);
            }
        }
        asteroid_timer = asteroid_timer++;
        delay(frame_delay); //ships position changes after every shipdelay seconds
    }
};