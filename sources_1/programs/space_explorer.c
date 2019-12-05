#include <stdio.h>
#include <stdlib.h>
#define ASTEROID_MAX 10
#define VEL_Q_LEN 50

//Addresses
//============================================================================================================

volatile int *const VG_ADDR = (int *)0x11100000;
volatile int *const VG_COLOR = (int *)0x11140000;
volatile int *const GYRO_X = (int *)0x11080000;
volatile int *const GYRO_Y = (int *)0x11090000;
volatile int *const GYRO_Z = (int *)0x110a0000;
volatile int *const BTN_LEFT_ADDR = (int *)0x110b0000;
volatile int *const BTN_RIGHT_ADDR = (int *)0x110c0000;
volatile int *const BTN_TOP_ADDR = (int *)0x110d0000;
volatile int *const BTN_BOTTOM_ADDR = (int *)0x110e0000;
volatile int *const BTN_CENTER_ADDR = (int *)0x110f0000;
volatile int *const SSEG_ADDR = (int *)0x110C0000;

//Prototypes
//============================================================================================================

//Resets all positions and gets game ready to start
void initGame();
//Generates random number between upper and lower
int random_from(int lower, int upper);
//Draws spaceship at position where position is the upper lefthand corner of the ship
void drawSpaceship(int position[2]);
//Draws spaceship at position where position is the upper lefthand corner of the ship
void drawAsteroid(int position[2]);
//iterates through asteroid positions and sees if they come in contact with ship
int checkCollision(int spaceshipPosition[2], int asteroidPositions[ASTEROID_MAX][2]);
//delays animation loop x miliseconds
void delay(int ms);
void resetAsteroid(int *position);
void draw_dot(int X, int Y, int color);
static void draw_horizontal_line(int X, int Y, int toX, int color);
static void draw_vertical_line(int X, int Y, int toY, int color);
//updates the asteroid position as its moving down the screen
void updateAsteroid(int position[2]);
//set up background
void draw_background();
int bin_val(int val);
void updateGyroTilt();
static void print_SSEG(int num);
static int convertGyro(int vel);

//Global Variables
//============================================================================================================
int bgd_color = 0x0807;
int screen_width = 80;
int screen_height = 60;
//Determines if the player's ship hasn't been hit (0 when hit)
int alive = 1;
int spaceship_pos[2];
//X and y tilt values
int tilt[2] = {0};
//X and y angular velocity value queues for averaging
int velocity_queues[2][VEL_Q_LEN] = {{0}, {0}};
//An array of asteroid positions
int asteroids[ASTEROID_MAX][2] = {0};

int frame_delay = 70;
int asteroid_timer = 0;
int asteroid_count = 0;

//Sizes
int ship_width = 3;
int ship_height = 5;
int asteroid_width = 4;
int asteroid_height = 4;

//Function Definitions

//Helpers
//============================================================================================================
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

int checkCollision(int ship_position[2], int asteroid_positions[ASTEROID_MAX][2])
{
    for (int i = 0; i < asteroid_count; i++)
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

int bin_val(int val)
{
    if (-200 < val && val < 200)
        return 0;
    else if (val < -500)
        return -500;
    else if (val > 500)
        return 500;
    else
        return val;
}

static void print_SSEG(int num)
{
    *SSEG_ADDR = num;
}

//Gamestate
//============================================================================================================
void initGame()
{
    //since spaceship pos is top left corner of ship, have to calc the position based of the width/height
    int bottom_padding = 3;
    spaceship_pos[0] = (79 - ship_width / 2) / 2;
    spaceship_pos[1] = 59 - ship_height - bottom_padding;
    for (int i = 0; i < ASTEROID_MAX; i++)
    {
        asteroids[i][0] = 0;
        asteroids[i][1] = 0;
    }
}

//Updates
//============================================================================================================

void updateAsteroid(int *position)
{
    //lowers the asteroid by one bit then randomly generates the x position
    position[1] = position[1] + 1;

    if (position[1] > screen_height)
        resetAsteroid(position);
}

void resetAsteroid(int *position)
{
    position[0] = random_from(0, screen_width);
    position[1] = 0;
}

void updateSpaceship()
{
    if (*BTN_RIGHT_ADDR && !*BTN_LEFT_ADDR)
    {
        spaceship_pos[0] = (spaceship_pos[0] + 1) % screen_width;
    }
    else if (*BTN_LEFT_ADDR && !*BTN_LEFT_ADDR)
    {
        spaceship_pos[0] < 1 ? spaceship_pos[0] = screen_width - 1 : spaceship_pos[0]--;
    }
    spaceship_pos[0] = tilt[0];
    //FOR TESTING
    print_SSEG(spaceship_pos[0]);
    drawSpaceship(spaceship_pos);
}

void updateGyroTilt()
{

    int sums[2] = {0};
    for (int i = VEL_Q_LEN - 2; i < 0; i--)
    {
        velocity_queues[0][i + 1] = velocity_queues[0][i];
        velocity_queues[1][i + 1] = velocity_queues[1][i];
        sums[0] += velocity_queues[0][i];
        sums[1] += velocity_queues[1][i];
    }
    //add latest reading to the velocity queue
    velocity_queues[0][0] = *GYRO_X;
    velocity_queues[1][0] = *GYRO_Y;
    //Update tilt
    tilt[0] = convertGyro(*GYRO_X);
    tilt[1] = bin_val(sums[1] / VEL_Q_LEN);
}

static int convertGyro(int vel)
{
    return (int)(((float)vel) * 8.75 / 1000);
}

//Draw
//============================================================================================================

void drawAsteroid(int position[2])
{
    //colors
    int highlight = 0x000;
    int midtone = 0x000;
    int shadow = 0x000;
    //Highlights
    draw_horizontal_line(position[0] + 1, position[1], position[0] + 2, highlight);
    draw_vertical_line(position[0], position[1] + 1, position[1] + 2, highlight);
    //mid
    draw_horizontal_line(position[0] + 1, position[1] + 1, position[0] + 2, midtone);
    draw_horizontal_line(position[0] + 1, position[1] + 2, position[0] + 2, midtone);
    //shadow
    draw_horizontal_line(position[0] + 1, position[1] + 3, position[0] + 2, shadow);
    draw_vertical_line(position[0] + 3, position[1] + 1, position[1] + 2, shadow);
}

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

void draw_dot(int X, int Y, int color)
{
    *VG_ADDR = (Y << 7) | X;
    *VG_COLOR = color;
}

//MAIN
//============================================================================================================
int main(void)
{
    initGame();
    //how many asteroids are on the screen
    //asteroid count also serves as the asteroid number when updating the array

    while (alive)
    {
        draw_background();
        updateGyroTilt();
        updateSpaceship();
        // alive = checkCollision(spaceship_pos, asteroids);
        if (asteroid_timer > 5)
        {
            asteroid_timer = 0;
            //draws a new asteroid if there are less than 10 asteroids
            if (asteroid_count < ASTEROID_MAX)
            {
                int position[2] = {random_from(1, 80 - asteroid_width - 1), 0};
                asteroids[asteroid_count][0] = position[0];
                asteroids[asteroid_count][1] = position[1];
                asteroid_count++;
            }

            //updates all asteroids that currently exist
            for (int a = 0; a < asteroid_count; a++)
            {
                updateAsteroid(asteroids[a]);
            }
        }
        for (int a = 0; a < asteroid_count; a++)
        {
            drawAsteroid(asteroids[a]);
        }
        asteroid_timer++;
        delay(frame_delay); //amount of time before next frame
    }
};
