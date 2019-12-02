#include <stdio.h>
#include <stdlib.h>
#include <math.h>
const int BG_COLOR = 0xFF; // light blue (0/7 red, 3/7 green, 3/3 blue)
volatile int *const VG_ADDR = (int *)0x11100000;
volatile int *const VG_COLOR = (int *)0x11140000;
volatile int *const GYRO_X = (int *)0x11080000;
volatile int *const GYRO_Y = (int *)0x11090000;
volatile int *const GYRO_Z = (int *)0x110a0000;
volatile int *const SSEG_ADDR = (int *)0x110C0000;
int const SCREEN_WIDTH = 80;
int const SCREEN_HEIGHT = 60;

static void draw_background();
static void draw_dot(int X, int Y, int color);
static void draw_horizontal_line(int X, int Y, int toX, int color);
static void delay(int ms);
static void print_SSEG(int num);

void main()
{
    int x = 0;
    int y = SCREEN_HEIGHT / 2;
    double scaled_x = 0;
    while (1)
    {

        draw_background();
        scaled_x += (float)(*GYRO_X * 0.01);
        x = (int)(scaled_x) % SCREEN_WIDTH;
        draw_dot(x, y, 0x000);
        print_SSEG(*GYRO_X);
        delay(100);
    }
}
void delay(int ms)
{
    int count = ms * 12500;
    volatile int i;
    for (i = 0; i < count; i++)
    {
    };
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

static void print_SSEG(int num)
{
    *SSEG_ADDR = num;
}
