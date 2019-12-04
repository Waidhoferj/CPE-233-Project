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
int random(int lower, int upper);
//Draws spaceship at position where position is the upper lefthand corner of the ship
void drawSpaceship(int position[2]);
//Draws spaceship at position where position is the upper lefthand corner of the ship
void drawAsteroid(int position[2]);
//iterates through asteroid positions and sees if they come in contact with ship
int checkCollision(int spaceshipPosition[2], int asteroidPositions[ASTEROID_COUNT][2]);
//delays animation loop x miliseconds
void delay(int ms);

//A
void draw_dot(int X, int Y, int color);
//updates the asteroid position as its moving down the screen
void updateAsteroid(int position[2]);
//set up background
void draw_background();

int astrd_color = 0x00;
int bgd_color = 0xFF;

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

int random(int lower, int upper)
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
    //Make these global variables (possibly as structs Ship and Asteroid)
    //int ship_width = 4;
    //int ship_height = 7;
    //int asteroid_width = 4;
    //int asteroid_height = 4;
    // bottom left and top right points of the ship

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
    spaceship_pos[0] = 39 - ((ship_width - 1) / 2);
    spaceship_pos[1] = 79 - ship_height;
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
    position[1] = position[1] - 1;
    int lower = position[0] - 1;
    int upper = position[0] + 1;
    position[0] = random(lower, upper);
    drawAsteroid(position);
}

//A
void drawAsteroid(int position[2])
{
    //draws a block asteroid with dimensions asteroid_width by asteroid_height
    //for every row of the asteroid
    for (int y = position[1]; y < asteroid_height; y++)
    {
        //for every column of each row (moving the x position through each row)
        for (int x = position[0]; x < asteroid_width; x++)
        {
            draw_dot(x, y, astrd_color);
        }
    }
}

//A
void draw_background()
{
    for (int i = 0; i < screen_height; i++)
    {
        for (int j = 0; j < screen_width; j++)
        {
            draw_dot(i, j, bgd_color);
        }
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
    spaceship_pos[0] += *GYRO_X * .0001;
    spaceship_pos[1] += *GYRO_Y * .0001;
}

//A
void drawSpaceship(int position[2])
{
    for (int y = position[1]; y < ship_height; y++)
    {
        //draw dot for every column of every row of the ship
        for (int x = position[0]; x < ship_width; x++)
        {
            draw_dot(x, y, astrd_color);
        }
    }
}

int main(void)
{

    draw_background();
    initGame();
    drawSpaceship(spaceship_pos);

    int shipdelay = 32; //32 ms
    int asteroid_timer = 0;
    int asteroid_count = 0; //how many asteroids are on the screen
    //asteroid count also serves as the asteroid number when updating the array
    while (alive)
    {
        updateSpaceship();
        alive = checkCollision(spaceship_pos, asteroids);
        if (asteroid_timer == 1000) //updates the asteroid after 1000ms
        {
            asteroid_timer = 0;
            //draws a new asteroid if there are less than 10 asteroids
            if (asteroid_count <= 10)
            {
                int position[2] = {random(0, 60 - asteroid_width - 1), 80};
                asteroids[asteroid_count][2] = position; //adds new asteroid position to asteroid arra
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
        delay(shipdelay); //ships position changes after every shipdelay seconds
    }
};
