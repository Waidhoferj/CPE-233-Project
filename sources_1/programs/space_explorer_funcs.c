#include <stdio.h>
#include <stdlib.h>
#define ASTEROID_COUNT 10

volatile int *const VG_ADDR = (int *)0x11100000;
volatile int *const VG_COLOR = (int *)0x11140000;
 

//Resets all positions and gets game ready to start
void initGame(int *spaceship_pos, int *asteroids);
//Generates random number between upper and lower
int random(int lower, int upper);
//Draws spaceship at position where position is the upper lefthand corner of the ship
void drawSpaceship(int spaceshipPosition[2]);
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

int atsrd_color = 0x00
int bgd_color = 0xFF

int screen_width = 80;
int screen_height = 60;
//Determines if the player's ship hasn't been hit (0 when hit)
int alive = 1;
int spaceship_pos[2];
//An array of asteroid positions
int astrd_cnt = 10
int asteroids[astrd_cnt][2];

int ship_width = 4;
int ship_height = 7;
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

//A
void initGame(int *spaceship_pos, int *asteroids)
{
    spaceship_pos[1] = //middle of screem
    int i = 0;
    for (i; ASTEROID_COUNT; i++)
    {
        asteroids[i][2] = 0;
    }
}

//A
void updateAsteroid(int position[2])
{
    //lowers the asteroid by one bit then randomly generates the x position
    position[1] = position[1] - 1;
    int lower = position[0] - 1;
    int upper = position[0] + 1;
    position[0] = random(lower, upper)

} 

//A
void drawAsteroid(int position[2], int asteroid_width, int asteroid_height)
{
        i = asteroid_width;
        j = asteroid_height;
        l = 0;
        //draws a block asteroid with dimensions asteroid_width by asteroid_height 
        for (l; j; l++)
        {
            for (position[0];postion[0] + i; position[0]++)
            {
                draw_dot(posiotion[0],position[1],astrd_color);
            }   
        }
        
}

//A
void draw_background()
{
    int i = 0;
    int j = 0;
    for (i; int screen_height; i++ )
    {
        for(j; int screen_width; j++)
        {
            draw_dot(int i, int j, bgd_color)
        }
    }
}

//A
void draw_dot(int X, int Y, int color)
{
    *VG_ADDR = (Y << 7) | X;
    *VG_COLOR = color;   
}

//A
void drawSpaceship(int spaceshipPosition[2])
{
    i = ship_width;
    j = ship_height;
    l = 0;
    //draws a block asteroid with dimensions ship_width by ship_height 
    for (l; j; l++)
    {
        for (spaceshipPosition[0];spaceshipPostion[0] + i; spaceshipPosition[0]++)
        {
            draw_dot(spaceshipPosiotion[0],spaceshipPosition[1],astrd_color);
        }   
    }
}
int main(void){
    
    draw_background();
    //setup ship
    drawSpaceship(spaceshipPosition[2])
    //setup asteroids
    //i dont really understand how to continually update the array in C so i kind of did it like python
    int delay = 32; //32 ms 
    int astrd_shift_time = 5000// 5000ms(half second)
    int counter = 0;
    int c = 0
    for (c, ASTEROID_COUNT, c++)
    {
        int position[2] = {random(0,60-asteroid_width-1),80};
        asteroids[counter][2] = drawAsteroid(position[2]);
        delay(astrd_shift_time);
        for (i, counter, i++)
        {
            asteroids[i][2] = updateAsteroid(asteroids[i][2]);
        }
         counter = counter++;   
    }
  


    int i = 0;
    for (i; astrd_cnt, i++ )
    {
        int position[2] = {random(0,60-asteroid_width-1), 80};
        asteroids[i][2] = drawAsteroid(position[2]);    
    }

};
