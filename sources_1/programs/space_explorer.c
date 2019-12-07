#include <stdio.h>
#include <stdlib.h>
#define ASTEROID_MAX 35
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

void initGame();
int random_from(int lower, int upper);
void drawSpaceship(int position[2]);
void drawAsteroid(int position[2]);
int checkShipCollision();
void delay(int ms);
void resetAsteroid(int *position);
void draw_dot(int X, int Y, int color);
static void draw_horizontal_line(int X, int Y, int toX, int color);
static void draw_vertical_line(int X, int Y, int toY, int color);
void updateAsteroid(int position[2]);
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
int BULLET_COLOR = 0xFF;
int alive = 1; //Determines if the player's ship has been hit (0 when hit)
int spaceship_pos[2];
int bullet_pos[2] = {-1};
int tilt[2] = {0};                              //Would hold tilt of gyroscope
int asteroids[ASTEROID_MAX][2] = {0};           //Holds a list of x and y values for the asteroids
int frame_delay = 10;                           //Controls the speed of the base clock.
int asteroid_timer = 0;                         //Controlls the movement speed of the asteroids.
int spawn_timer = 0;                            //Increases asteroid_count until it hits asteroid_count_max (lowercased).
int difficulty_timer = 0;                       //Slowest clock used to slowly increase the number of asteroids.
int asteroid_count = 0;                         //Current number of asteroids looping on screen.
int score = 0;                                  //The number of asteroids the user has passed.
int ammo = 5;                                   //Number of bullets left to shoot
int asteroid_count_max = 20;                    //The current maximum number of asteroids, which will incrememt based on the difficulty_timer slow clock. It will always be less than the ASTEROID_MAX constant, which is the absoute maximum.
int velocity_queues[2][VEL_Q_LEN] = {{0}, {0}}; //Used when we attempted to avg the angular velocity of the gyroscope in code.

//Sizes
int screen_width = 80;
int screen_height = 60;
int ship_width = 3;
int ship_height = 5;
int asteroid_width = 4;
int asteroid_height = 4;

//FUNCTIONS
//Helpers
//============================================================================================================
//Produces a random integer from lower to upper (inclusive)
int random_from(int lower, int upper)
{
    return (rand() %
            (upper - lower + 1)) +
           lower;
}

//Delays the main thread for ms milliseconds. To be used to define frames in animaiton.
void delay(int ms)
{
    int count = ms * 12500;
    volatile int i;
    for (i = 0; i < count; i++)
    {
    };
}

//Checks if the ship has collided with any asteroids.
int checkShipCollision()
{
    for (int i = 0; i < asteroid_count; i++)
    {
        int aster_x = asteroids[i][0];
        int aster_y = asteroids[i][1];

        if ((aster_x + asteroid_width >= spaceship_pos[0] && aster_x <= spaceship_pos[0] + ship_width) && (aster_y < spaceship_pos[1] + ship_height && aster_y + asteroid_height > spaceship_pos[1]))
            return 1;
    }
    return 0;
}
//Checks if the object provided has been hit by an asteroid. If there is a hit, returns index of asteroid hit, else returns -1.
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

//Binning function for the gyroscope velocity. Used for limiting noise.
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

//Prints the provided number to the seven segment display on the Basys3 board.
static void print_SSEG(int num)
{
    *SSEG_ADDR = num;
}

//Gamestate
//============================================================================================================
//Prepares state variables for a new game.
void initGame()
{
    int bottom_padding = 3;
    spaceship_pos[0] = (79 - ship_width / 2) / 2;
    spaceship_pos[1] = 59 - ship_height - bottom_padding;
    asteroid_timer = 0;
    spawn_timer = 0;
    difficulty_timer = 0;
    asteroid_count = 0;
    asteroid_count_max = 20;
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

//Starts game and keeps the animation loop running until players is hit with asteroid.
static int runGame()
{
    //Main animation loop
    while (alive)
    {
        draw_background();
        updateGyroTilt();
        updateSpaceship();
        alive = !checkShipCollision();

        //Spawn Timer
        if (spawn_timer > 10 && asteroid_count < asteroid_count_max)
        {
            //Adds new asteroids
            spawn_timer = 0;
            int position[2] = {random_from(1, 80 - asteroid_width - 1), 0};
            asteroids[asteroid_count][0] = position[0];
            asteroids[asteroid_count][1] = position[1];
            asteroid_count++;
        }
        //Asteroid Timer
        if (asteroid_timer > 3)
        {
            //Move asteroids
            asteroid_timer = 0;
            for (int a = 0; a < asteroid_count; a++)
            {
                updateAsteroid(asteroids[a]);
            }
        }
        //Difficulty Timer
        if (difficulty_timer > 500)
        {

            ammo < 10 ? ammo++ : 0; //Increase ammo supply
            difficulty_timer = 0;
            asteroid_count_max < ASTEROID_MAX ? asteroid_count_max++ : 0; //Increase the max number of asteroids in the array.
        }

        //Draw all of the asteroids.
        for (int a = 0; a < asteroid_count; a++)
        {
            drawAsteroid(asteroids[a]);
        }
        //Bullet logic
        if (bullet_pos[1] >= 0)
        {

            draw_dot(bullet_pos[0], bullet_pos[1], BULLET_COLOR);
            int bullet_hit = checkHitAsteroid(bullet_pos, 1, 1);
            if (bullet_hit != -1)
            {
                removeAsteroid(bullet_hit);
                bullet_pos[0] = -1;
                bullet_pos[1] = -1;
            }
            bullet_pos[1]--;
        }

        drawAmmo();
        //Incrememt all slowclocks
        difficulty_timer++;
        asteroid_timer++;
        spawn_timer++;
        delay(frame_delay); //amount of time before next frame
    }
    return 1;
}

//Draws the game over screen and holds user there until they click the restart button.
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
//Updates the position of the asteroid
void updateAsteroid(int *position)
{
    //lowers the asteroid by one bit then randomly generates the x position
    position[1] = position[1] + 1;

    if (position[1] > screen_height)
        resetAsteroid(position);
}

//Places the asteroid back at the top of the screen.
void resetAsteroid(int *position)
{
    score++;
    print_SSEG(score);
    position[0] = random_from(0, screen_width - asteroid_width);
    position[1] = 0;
}

//Removes asteroid from array at index.
void removeAsteroid(int index)
{
    for (int i = index; i < asteroid_count_max - 1; i++)
    {
        asteroids[i][0] = asteroids[i + 1][0];
        asteroids[i][1] = asteroids[i + 1][1];
    }
    asteroid_count--;
}

//Handles user input
void updateSpaceship()
{
    if (*BTN_RIGHT_ADDR && !*BTN_LEFT_ADDR) //Right button pressed : go right.
    {
        spaceship_pos[0] = (spaceship_pos[0] + 1) % screen_width;
    }
    else if (*BTN_LEFT_ADDR && !*BTN_RIGHT_ADDR) //Left button pressed: go left.
    {
        spaceship_pos[0] < 1 ? spaceship_pos[0] = screen_width - 1 : spaceship_pos[0]--;
    }
    else if (*BTN_CENTER_ADDR && bullet_pos[1] < 0 && ammo) //Center button pressed: shoot if possible.
        pewPew();

    drawSpaceship(spaceship_pos);
}

static void pewPew()
{
    bullet_pos[0] = spaceship_pos[0] + 1;
    bullet_pos[1] = spaceship_pos[1] - 1;
    ammo--;
}

//NOT USED: was an attempt to interpret the gyro data in software. Keeping it here to show process.
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
//NOT USED: was an attempt to interpret the gyro data in software. Keeping it here to show process.
static int convertGyro(int vel)
{
    //Conversion dps/digit to dps as addressed the the pmodgyro documentation
    return (int)(((float)vel) * 8.75 / 1000);
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

//MAIN
//============================================================================================================
int main(void)
{
    while (1)
    {
        initGame();
        int res = runGame();
        //In this case we only have 1 screen, but if we had 2 screen options
        //we can use the res value to return the screen choice from runGame
        //and display it in the main animation loop
        drawL();
    }
};
