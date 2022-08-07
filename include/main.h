#pragma once

// Starter code from 3DSage on YouTube / https://github.com/3DSage/OpenGL-Starter_v1

#include <math.h>
#include <stdio.h>
#include <GL/glut.h>

#define res        1                        // 0=160x120 1=360x240 4=640x480
#define SW         160*res                  // screen width
#define SH         120*res                  // screen height
#define SW2        (SW/2)                   // half of screen width
#define SH2        (SH/2)                   // half of screen height
#define pixelScale 4/res                    // OpenGL pixel scale
#define GLSW       (SW*pixelScale)          // OpenGL window width
#define GLSH       (SH*pixelScale)          // OpenGL window height

#define M_PI       3.1415926                // pi

//------------------------------------------------------------------------------
typedef struct
{
	int fr1, fr2;           //frame 1 frame 2, to create constant frame rate
}time; time T;

typedef struct
{
	int w, s, a, d;           //move up, down, left, right
	int sl, sr;             //strafe left, right 
	int m;                 //move up, down, look up, down
}keys; keys K;

typedef struct
{
	float cos[360];
	float sin[360];
}math; math M;

typedef struct
{
	int x, y, z;     // Player position, Z Up
	int a;           // Left/Right rotation
	int l;           // Vertical rotation
}player; player P;
//------------------------------------------------------------------------------

void pixel(int x, int y, int c);
void movePlayer();
void clearBackground();
void clipBehindPlayer(int* x1, int* y1, int* z1, int x2, int y2, int z2);
void DrawWall(int x1, int x2, int b1, int b2, int t1, int t2);
void draw3D();
void display();
void KeysDown(unsigned char key, int x, int y);
void KeysUp(unsigned char key, int x, int y);
void init();
int main(int argc, char* argv[]);