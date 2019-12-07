//PUROPSE: Test the gyroscope and button connections in an isolated and simple environment.
//CREDIT: Used the OTTER VGA 80x60 library to draw to the screen

//IMPORTED LIBS
//======================================================================
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//ADDRESSES
//======================================================================
volatile int *const VG_ADDR = (int *)0x11100000;
volatile int *const VG_COLOR = (int *)0x11140000;
volatile int *const GYRO_X = (int *)0x11080000;
volatile int *const GYRO_Y = (int *)0x11090000;
volatile int *const GYRO_Z = (int *)0x110a0000;
volatile int *const SSEG_ADDR = (int *)0x110C0000;
volatile int *const BTN_LEFT_ADDR = (int *)0x110b0000;
volatile int *const BTN_RIGHT_ADDR = (int *)0x110c0000;
volatile int *const BTN_TOP_ADDR = (int *)0x110d0000;
volatile int *const BTN_BOTTOM_ADDR = (int *)0x110e0000;
volatile int *const BTN_CENTER_ADDR = (int *)0x110f0000;

//GLOBALS
//======================================================================
const int BG_COLOR = 0xFF; // light blue (0/7 red, 3/7 green, 3/3 blue)
int const SCREEN_WIDTH = 80;
int const SCREEN_HEIGHT = 60;

//Prototypes
//======================================================================
static void draw_background();
static void draw_dot(int X, int Y, int color);
static void draw_horizontal_line(int X, int Y, int toX, int color);
static void delay(int ms);
static void print_SSEG(int num);

//MAIN
//======================================================================
void main()
{
    int x = SCREEN_WIDTH / 2;
    int y = SCREEN_HEIGHT / 2;
    while (1)
    {
        if (*BTN_BOTTOM_ADDR)
            y = SCREEN_HEIGHT / 2 + 10;
        else
            y = SCREEN_HEIGHT / 2;
        draw_background();
        draw_dot(x, y, 0x000);
        delay(100);
    }
}

//FUNCTIONS
//======================================================================
//Delays the main thread ms number of milliseconds.
void delay(int ms)
{
    int count = ms * 12500;
    volatile int i;
    for (i = 0; i < count; i++)
    {
    };
}
//Prints number as hex to the seven segment display on the OTTER.
static void print_SSEG(int num)
{
    *SSEG_ADDR = num;
}

static void draw_dot(int X, int Y, int color)
{
    *VG_ADDR = (Y << 7) | X; // store into the address IO register
    *VG_COLOR = color;       // store into the color IO register, which triggers
                             // the actual write to the framebuffer at the address
                             // previously stored in the address IO register
}

static void draw_background()
{
    for (int Y = 0; Y != 60; Y++)
    {
        draw_horizontal_line(0, Y, 79, BG_COLOR);
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
