// coordinates are given in row major format
// (col,row) = (x,y)
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define WHITE 0xFF
const int BG_COLOR = 0xFF; // light blue (0/7 red, 3/7 green, 3/3 blue)
volatile int *const VG_ADDR = (int *)0x11100000;
volatile int *const VG_COLOR = (int *)0x11140000;

static void draw_horizontal_line(int X, int Y, int toX, int color);
static void draw_vertical_line(int X, int Y, int toY, int color);
static void draw_background();
static void draw_dot(int X, int Y, int color);
static void draw_line(int start[2], int end[2], int color);

void main()
{
    // fill screen using default color
    draw_background();
    int bounds[2] = {80, 60};
    int frame[2][2] = {{10, bounds[0] - 10}, {10, bounds[1] - 10}};

    int big_lower[2] = {bounds[0] / 2, frame[1][1]};
    int big_left[2] = {frame[0][0], frame[1][0]};
    int big_right[2] = {frame[0][1], frame[1][0]};

    int inner_lower[2] = {bounds[0] / 2, frame[1][1] - 18};
    int inner_left[2] = {frame[0][0] + 20, frame[1][0] + 10};
    int inner_right[2] = {frame[0][1] - 20, frame[1][0] + 10};

    int top_start[2] = {frame[0][0] + 4, frame[1][0] + 5};
    int top_end[2] = {frame[0][1] - 15, frame[1][0] + 5};
    int top_corner[2] = {bounds[0] / 2, frame[1][1] - 18};

    int left_start[2] = {bounds[0] / 2 + 4, frame[1][1] - 6};
    int left_end[2] = {frame[0][0] + 15, frame[1][0] + 10};
    int left_corner[2] = {frame[0][0] + 20, frame[1][0] + 10};

    int right_start[2] = {frame[0][1] - 5, frame[1][0]};
    int right_end[2] = {bounds[0] / 2 + 4, frame[1][1] - 14};
    int right_corner[2] = {frame[0][0] + 20, frame[1][0] + 10};

    while (1)
    {
        //Outer triangle
        draw_line(big_left, big_lower, 0x2A78);
        draw_line(big_left, big_right, 0x2A78);
        draw_line(big_right, big_lower, 0x2A78);

        //Inner triangle
        draw_line(inner_left, inner_lower, 0x2A78);
        draw_line(inner_left, inner_right, 0x2A78);
        draw_line(inner_right, inner_lower, 0x2A78);

        //Interior lines
        //top
        draw_line(top_start, top_end, 0x2A78);
        draw_line(top_end, top_corner, 0x2A78);

        //left
        draw_line(left_end, left_start, 0x2A78);
        draw_line(left_end, left_corner, 0x2A78);

        //right
        draw_line(right_start, right_end, 0x2A78);
        draw_line(right_corner, right_end, 0x2A78);
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

static void draw_line(int start[2], int end[2], int color)
{
    int rise = end[1] - start[1];
    int run = end[0] - start[0];
    int max_val = abs(rise) > abs(run) ? rise : run;
    for (int i = 0; i < max_val; i++)
    {
        float t = (float)i / max_val;
        int x = start[0] + run * t;
        int y = start[1] + rise * t;
        draw_dot(x, y, color);
    }
}

// fills the screen with BG_COLOR
static void draw_background()
{
    for (int Y = 0; Y != 60; Y++)
    {
        draw_horizontal_line(0, Y, 79, BG_COLOR);
    }
}

// draws a small square (a single memory cell)
static void draw_dot(int X, int Y, int color)
{
    *VG_ADDR = (Y << 7) | X; // store into the address IO register
    *VG_COLOR = color;       // store into the color IO register, which triggers
                             // the actual write to the framebuffer at the address
                             // previously stored in the address IO register
}
