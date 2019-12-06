#include <stdio.h>
#include <stdlib.h>
#define ASTEROID_MAX 35
#define VEL_Q_LEN 50
int BULLET_COLOR = 0xFF;

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
int checkCollision(int spaceshipPosition[2]);
//delays animation loop x miliseconds
void delay(int ms);
void resetAsteroid(int *position);
void draw_dot(int X, int Y, int color);
static void draw_horizontal_line(int X, int Y, int toX, int color);
static void draw_vertical_line(int X, int Y, int toY, int color);
//updates the asteroid position as its moving down the screen
void updateAsteroid(int position[2]);
//set up background
static void draw_background();
int bin_val(int val);
static void updateGyroTilt();
static void print_SSEG(int num);
static int convertGyro(int vel);
static void drawL();
static int runGame();
static void drawAmmo();
static void pewPew();
int checkHitAsteroid(int *position, int width, int height);

//Global Variables
//============================================================================================================
int bgd_color = 0x000;
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

int frame_delay = 10;
int asteroid_timer = 0;
int spawn_timer = 0;
int asteroid_count = 0;
int score = 0;
int difficulty_timer = 0;
int bullet_pos[2] = {0};
int ammo = 5;
int asteroid_count_max = 20;

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

int checkCollision(int ship_position[2])
{
    for (int i = 0; i < asteroid_count; i++)
    {
        int aster_x = asteroids[i][0];
        int aster_y = asteroids[i][1];

        if ((aster_x + asteroid_width >= ship_position[0] && aster_x <= ship_position[0] + ship_width) && (aster_y < ship_position[1] + ship_height && aster_y + asteroid_height > ship_position[1]))
            return 1;
    }
    return 0;
}
//If there is a hit, returns index of asteroid hit + 1
int checkHitAsteroid(int *position, int width, int height)
{
    for (int i = 0; i < asteroid_count; i++)
    {
        int aster_x = asteroids[i][0];
        int aster_y = asteroids[i][1];

        if ((aster_x + asteroid_width >= position[0] && aster_x <= position[0] + ship_width) && (aster_y < position[1] + height && aster_y + asteroid_height > position[1]))
            return i;
    }
    return -1;
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
    asteroid_timer = 0;
    spawn_timer = 0;
    asteroid_count = 0;
    ammo = 5;
    bullet_pos[0] = -1;
    bullet_pos[1] = -1;
    score = 0;
    alive = 1;
    frame_delay = 10;

    for (int i = 0; i < ASTEROID_MAX; i++)
    {
        asteroids[i][0] = 0;
        asteroids[i][1] = 0;
    }
}

static int runGame()
{
    while (alive)
    {
        draw_background();
        updateGyroTilt();
        updateSpaceship();
        alive = !checkCollision(spaceship_pos);

        if (spawn_timer > 10 && asteroid_count < asteroid_count_max)
        {
            spawn_timer = 0;
            int position[2] = {random_from(1, 80 - asteroid_width - 1), 0};
            asteroids[asteroid_count][0] = position[0];
            asteroids[asteroid_count][1] = position[1];
            asteroid_count++;
        }
        if (asteroid_timer > 3)
        {
            asteroid_timer = 0;
            for (int a = 0; a < asteroid_count; a++)
            {
                updateAsteroid(asteroids[a]);
            }
        }

        if (difficulty_timer > 120)
        {
            difficulty_timer = 0;
            asteroid_count_max < ASTEROID_MAX ? asteroid_count_max++ : 0;
            // frame_delay > 5 ? frame_delay-- : 0;
        }

        for (int a = 0; a < asteroid_count; a++)
        {
            drawAsteroid(asteroids[a]);
        }

        if (bullet_pos[1] > 0)
        {
            bullet_pos[1]--;
            draw_dot(bullet_pos[0], bullet_pos[1], BULLET_COLOR);
            int bullet_hit = checkHitAsteroid(bullet_pos, 1, 1);
            if (bullet_hit != -1)
            {
                removeAsteroid(bullet_hit);
                bullet_pos[0] = -1;
                bullet_pos[1] = -1;
            }
        }

        drawAmmo();
        difficulty_timer++;
        asteroid_timer++;
        spawn_timer++;
        delay(frame_delay); //amount of time before next frame
    }
    return 1;
}

static void drawL()
{

    while (!*BTN_BOTTOM_ADDR)
    {
        for (int i = 9; i < 20; i++)
        {
            draw_vertical_line(i, 10, 50, 0xFFF);
            delay(20);
        }
        for (int i = 40; i < 51; i++)
        {
            draw_horizontal_line(19, i, 50, 0xFFF);
            delay(20);
        }
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
    score++;
    print_SSEG(score);
    position[0] = random_from(0, screen_width);
    position[1] = 0;
}

void updateSpaceship()
{
    if (*BTN_RIGHT_ADDR && !*BTN_LEFT_ADDR)
    {
        spaceship_pos[0] = (spaceship_pos[0] + 1) % screen_width;
    }
    else if (*BTN_LEFT_ADDR && !*BTN_RIGHT_ADDR)
    {
        spaceship_pos[0] < 1 ? spaceship_pos[0] = screen_width - 1 : spaceship_pos[0]--;
    }
    else if (*BTN_CENTER_ADDR && bullet_pos[1] < 0 && ammo)
        pewPew();

    //FOR TESTING
    drawSpaceship(spaceship_pos);
}

static void updateGyroTilt()
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

void removeAsteroid(int index)
{
    for (int i = index; i < asteroid_count_max - 1; i++)
    {
        asteroids[i][0] = asteroids[i + 1][0];
        asteroids[i][1] = asteroids[i + 1][1];
    }
    asteroid_count--;
}

//Draw
//============================================================================================================

void drawAsteroid(int position[2])
{
    //colors
    int highlight = 3;
    int midtone = 0x2;
    int shadow = 0x2;
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
    int highlight = 0xf3;
    int midtone = 0xc0;
    int shadow = 0xa0;

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

static void draw_background()
{
    for (int i = 0; i < screen_height; i++)
    {
        for (int j = 0; j < screen_width; j++)
        {
            draw_dot(j, i, bgd_color);
        }
    }
}

static void drawAmmo()
{
    for (int i = 0; i < ammo; i++)
    {
        draw_dot(screen_width - 2 - i * 2, screen_height - 1, BULLET_COLOR);
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

static void pewPew()
{
    bullet_pos[0] = spaceship_pos[0] + 1;
    bullet_pos[1] = spaceship_pos[1] - 1;
    ammo--;
}

//MAIN
//============================================================================================================
int main(void)
{
    while (1)
    {

        initGame();
        int res = runGame();
        drawL();
    }

    //how many asteroids are on the screen
    //asteroid count also serves as the asteroid number when updating the array
};
