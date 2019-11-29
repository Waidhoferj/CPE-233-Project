#include <stdio.h>
#include <stdlib.h>
#define ASTEROID_COUNT 10

//Resets all positions and gets game ready to start
void initGame(int *spaceship_pos, int *asteroids);
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

int screen_width = 80;
int screen_height = 60;
//Determines if the player's ship hasn't been hit (0 when hit)
int alive = 1;
int spaceship_pos[2];
//An array of asteroid positions
int asteroids[10][2];

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
    int ship_width = 4;
    int ship_height = 7;
    int asteroid_width = 4;
    int asteroid_height = 4;
    // bottom left and top right points of the ship

    for (int i = 0; i < ASTEROID_COUNT; i++)
    {
        int asteriod_position[2] = asteroid_positions[i];
        //{x differences},{y difference}
        int x_corner_dif[2] = {asteriod_position[0] + asteroid_width - ship_position[0], asteriod_position[0] - (ship_position[0] + ship_width)};
        int x_overlap = abs(x_corner_dif[0] + x_corner_dif[1]) < abs(x_corner_dif[0]) + abs(x_corner_dif[1]);
        int y_corner_dif[2] = {asteriod_position[1] + asteroid_height - ship_position[1], asteriod_position[1] - (ship_position[1] + ship_height)};
        int y_overlap = abs(y_corner_dif[0] + y_corner_dif[1]) < abs(y_corner_dif[0]) + abs(y_corner_dif[1]);
        if (x_overlap && y_overlap)
            return 1;
    }
    return 0;
}

int main(void){
    //...
};
