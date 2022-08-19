#include <main.h>

void pixel(int x, int y, int c)                  //draw a pixel at x/y with rgb
{
	int rgb[3];
	if (c == 0) { rgb[0] = 255; rgb[1] = 255; rgb[2] = 0; } //Yellow	
	if (c == 1) { rgb[0] = 160; rgb[1] = 160; rgb[2] = 0; } //Yellow darker	
	if (c == 2) { rgb[0] = 0; rgb[1] = 255; rgb[2] = 0; } //Green	
	if (c == 3) { rgb[0] = 0; rgb[1] = 160; rgb[2] = 0; } //Green darker	
	if (c == 4) { rgb[0] = 0; rgb[1] = 255; rgb[2] = 255; } //Cyan	
	if (c == 5) { rgb[0] = 0; rgb[1] = 160; rgb[2] = 160; } //Cyan darker
	if (c == 6) { rgb[0] = 160; rgb[1] = 100; rgb[2] = 0; } //brown	
	if (c == 7) { rgb[0] = 110; rgb[1] = 50; rgb[2] = 0; } //brown darker
	if (c == 8) { rgb[0] = 0; rgb[1] = 60; rgb[2] = 130; } //background 
	glColor3ub(rgb[0], rgb[1], rgb[2]);
	glBegin(GL_POINTS);
	glVertex2i(x * pixelScale + 2, y * pixelScale + 2);
	glEnd();
}

void movePlayer()
{
	//move up, down, left, right
	if (K.a == 1 && K.m == 0) { P.a -= 4; if (P.a < 0) { P.a += 360; } }
	if (K.d == 1 && K.m == 0) { P.a += 4; if (P.a > 359) { P.a -= 360; } }
	int dx = M.sin[P.a] * 10.0; // sin(RAD) * 10
	int dy = M.cos[P.a] * 10.0; // cos(RAD) * 10
	if (K.w == 1 && K.m == 0) { P.x += dx; P.y += dy; }
	if (K.s == 1 && K.m == 0) { P.x -= dx; P.y -= dy; }
	//strafe left, right
	if (K.sr == 1) { P.x += dy; P.y -= dx; }
	if (K.sl == 1) { P.x -= dy; P.y += dx; }
	//move up, down, look up, look down
	if (K.a == 1 && K.m == 1) { P.l -= 1; }
	if (K.d == 1 && K.m == 1) { P.l += 1; }
	if (K.w == 1 && K.m == 1) { P.z -= 4; }
	if (K.s == 1 && K.m == 1) { P.z += 4; }
}

void clearBackground()
{
	int x, y;
	for (y = 0; y < SH; y++)
	{
		for (x = 0; x < SW; x++) { pixel(x, y, 8); } //clear background color
	}
}

void clipBehindPlayer(int* x1, int* y1, int* z1, int x2, int y2, int z2)
{
	float da = *y1;
	float db = y2;
	float d = da - db; if (d == 0) { d = 1; }
	float s = da / (da - db);                 // Intersection factor (between 0 and 1)
	*x1 = *x1 + s * (x2 - (*x1));
	*y1 = *y1 + s * (y2 - (*y1)); if (*y1 == 0) { *y1 = 1; }
	*z1 = *z1 + s * (z2 - (*z1));
}

void DrawWall(int x1, int x2, int b1, int b2, int t1, int t2, int color, int s)
{
	int x, y;
	// Hold distance delta
	int dyb = b2 - b1;     // Delta Y of bottom line
	int dyt = t2 - t1;     // Delta Y of top line
	int dx = x2 - x1;     // Delta X
	if (dx == 0) { dx = 1; }
	int xs = x1;          // Hold x1 initial pos

	// Clip X
	if (x1 < 1) { x1 = 1; } // Clip left
	if (x2 < 1) { x2 = 1; } // Clip left
	if (x1 > SW - 1) { x1 = SW - 1; } // Clip right
	if (x2 > SW - 1) { x2 = SW - 1; } // Clip right

	// Draw X vertical lines
	for (x = x1; x < x2; x++)
	{
		// Y start & end point
		int y1 = dyb * (x - xs + 0.5) / dx + b1; // Y bottom point
		int y2 = dyt * (x - xs + 0.5) / dx + t1; // Y top point

		// Clip Y
		if (y1 < 1) { y1 = 1; } // Clip left
		if (y2 < 1) { y2 = 1; } // Clip left
		if (y1 > SH - 1) { y1 = SH - 1; } // Clip right
		if (y2 > SH - 1) { y2 = SH - 1; } // Clip right

		// Surfaces
		// if (Sector[s].surface = 1) { Sector[s].surf[x] = y1; printf("1\n"); return; }
		// if (Sector[s].surface = 2) { Sector[s].surf[x] = y2; printf("2\n"); return; }
		// 
		// if (Sector[s].surface = -1)
		// {
		// 	for (y = Sector[s].surf[x]; y < y1; y++)
		// 	{
		// 		pixel(x, y, Sector[s].c1); // Bottom
		// 	}
		// 	printf("-1\n");
		// }
		// if (Sector[s].surface = -2)
		// {
		// 	for (y = y2; y < Sector[s].surf[x]; y++)
		// 	{
		// 		pixel(x, y, Sector[s].c2); // Top
		// 	}
		// 	printf("-2\n");
		// }

		for (y = y1; y < y2; y++)
		{
			pixel(x, y, color); // Normal wall
		}
	}
}

int dist(int x1, int y1, int x2, int y2)
{
	int distance = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
	return distance;
}

void draw3D()
{
	int wx[4], wy[4], wz[4];
	float CS = M.cos[P.a], SN = M.sin[P.a];

	// Sort sectors
	for (int n = 0; n < numSect - 1; n++)
	{
		for (int w = 0; w < numSect - n - 1; w++)
		{
			if (Sector[w].depth < Sector[w + 1].depth)
			{
				sectors st = Sector[w];
				Sector[w] = Sector[w + 1];
				Sector[w + 1] = st;
			}
		}
	}

	// Draw sectors
	for (int n = 0; n < numSect; n++)
	{
		Sector[n].depth = 0; // Clear depth / distance

		if (P.z < Sector[n].z1)
		{
			// printf("%i", Sector[n].surface);
			Sector[n].surface = 1; // Bottom surface
		}
		else if (P.z > Sector[n].z2)
		{
			// printf("%i", Sector[n].surface);
			Sector[n].surface = 2; // Top surface
		}
		else
		{
			// printf("%i", Sector[n].surface);
			Sector[n].surface = 0; // No surface
		}

		for (int loop = 0; loop < 2; loop++)
		{
			for (int w = Sector[n].wallStart; w < Sector[n].wallEnd; w++)
			{

				// Offset bottom 2 points by player pos
				int x1 = Wall[w].x1 - P.x, y1 = Wall[w].y1 - P.y;
				int x2 = Wall[w].x2 - P.x, y2 = Wall[w].y2 - P.y;

				if (loop == 0) // Swap to draw back faces
				{
					int swap = x1;
					x1 = x2;
					x2 = swap;
					swap = y1;
					y1 = y2;
					y2 = swap;
				}

				// World X pos
				wx[0] = x1 * CS - y1 * SN;
				wx[1] = x2 * CS - y2 * SN;
				wx[2] = wx[0]; // Top line has same X
				wx[3] = wx[1];
				// World Y pos (depth)
				wy[0] = y1 * CS + x1 * SN;
				wy[1] = y2 * CS + x2 * SN;
				wy[2] = wy[0]; // Top line has same Y
				wy[3] = wy[1];

				Sector[n].depth += dist(0, 0, (wx[0] + wx[1]) / 2, (wy[0] + wy[1]) / 2); // Wall distance

				// World Z pos (height)
				wz[0] = Sector[n].z1 - P.z + ((P.l * wy[0]) / 32);
				wz[1] = Sector[n].z1 - P.z + ((P.l * wy[1]) / 32);
				wz[2] = wz[0] + Sector[n].z2;
				wz[3] = wz[1] + Sector[n].z2;

				// Cull if behind camera by skipping the current sector
				if (wy[0] < 1 && wy[1] < 1) { continue; }

				// Check point 1
				if (wy[0] < 1)
				{
					clipBehindPlayer(&wx[0], &wy[0], &wz[0], wx[1], wy[1], wz[1]); // Bottom line
					clipBehindPlayer(&wx[2], &wy[2], &wz[2], wx[3], wy[3], wz[3]); // Top line
				}
				// Check point 2
				if (wy[1] < 1)
				{
					clipBehindPlayer(&wx[1], &wy[1], &wz[1], wx[0], wy[0], wz[0]); // Bottom line
					clipBehindPlayer(&wx[3], &wy[3], &wz[3], wx[2], wy[2], wz[2]); // Top line
				}

				for (int n = 0; n < 4; n++)
				{
					// Catch divide by 0 errors
					if (wx[n] == 0) { wx[n] = 1; }
					if (wy[n] == 0) { wy[n] = 1; }
					if (wz[n] == 0) { wz[n] = 1; }
				}

				// Screen X/Y pos
				wx[0] = wx[0] * 200 / wy[0] + SW2; wy[0] = wz[0] * 200 / wy[0] + SH2;
				wx[1] = wx[1] * 200 / wy[1] + SW2; wy[1] = wz[1] * 200 / wy[1] + SH2;
				wx[2] = wx[2] * 200 / wy[2] + SW2; wy[2] = wz[2] * 200 / wy[2] + SH2;
				wx[3] = wx[3] * 200 / wy[3] + SW2; wy[3] = wz[3] * 200 / wy[3] + SH2;

				DrawWall(wx[0], wx[1], wy[0], wy[1], wy[2], wy[3], Wall[w].c, n);
			}
			Sector[n].depth /= (Sector[n].wallEnd - Sector[n].wallStart); // Average sector distance
			Sector[n].surface *= -1; // Invert surface value
		}
	}
}

void display()
{
	// int x, y;
	if (T.fr1 - T.fr2 >= 50)                        //only draw 20 frames/second
	{
		clearBackground();
		movePlayer();
		draw3D();

		T.fr2 = T.fr1;
		glutSwapBuffers();
		glutReshapeWindow(GLSW, GLSH);             //prevent window scaling
	}

	T.fr1 = glutGet(GLUT_ELAPSED_TIME);          //1000 Milliseconds per second
	glutPostRedisplay();
}

void KeysDown(unsigned char key, int x, int y)
{
	if (key == 'w' == 1) { K.w = 1; }
	if (key == 's' == 1) { K.s = 1; }
	if (key == 'a' == 1) { K.a = 1; }
	if (key == 'd' == 1) { K.d = 1; }
	if (key == 'm' == 1) { K.m = 1; }
	if (key == ',' == 1) { K.sr = 1; }
	if (key == '.' == 1) { K.sl = 1; }
}
void KeysUp(unsigned char key, int x, int y)
{
	if (key == 'w' == 1) { K.w = 0; }
	if (key == 's' == 1) { K.s = 0; }
	if (key == 'a' == 1) { K.a = 0; }
	if (key == 'd' == 1) { K.d = 0; }
	if (key == 'm' == 1) { K.m = 0; }
	if (key == ',' == 1) { K.sr = 0; }
	if (key == '.' == 1) { K.sl = 0; }
}

int SectorData[] =
{ // Wall start, Wall end, z1 height, z2 height, bottom color, top color
	0, 4, 0, 40, 0, 1,   // sector 1
	4, 8, 0, 40, 2, 3,   // sector 2
	8, 12, 0, 40, 4, 5,  // sector 3
	12, 16, 0, 40, 6, 7, // sector 4
};

int WallData[] =
{ // x1, y1, x2, y2, color
	0, 0, 32, 0, 0,
	32, 0, 32, 32, 1,
	32, 32, 0, 32, 0,
	0, 32, 0, 0, 1,

	64, 0, 96, 0, 2,
	96, 0, 96, 32, 3,
	96, 32, 64, 32, 2,
	64, 32, 64, 0, 3,

	64, 64, 96, 64, 4,
	96, 64, 96, 96, 5,
	96, 96, 64, 96, 4,
	64, 96, 64, 64, 5,

	0, 64, 32, 64, 6,
	32, 64, 32, 96, 7,
	32, 96, 0, 96, 6,
	0, 96, 0, 64, 7,
};

void init()
{
	// Store sin/cos in degrees
	for (int x = 0; x < 360; x++)
	{
		M.cos[x] = cos(x / 180.0 * M_PI);
		M.sin[x] = sin(x / 180.0 * M_PI);
	}
	// Init Player
	P.x = 70; P.y = -110; P.z = 20;
	P.a, P.l = 0;

	// Load sectors
	int v1 = 0, v2 = 0;
	for (int n = 0; n < numSect; n++)
	{
		Sector[n].wallStart = SectorData[v1];
		Sector[n].wallEnd = SectorData[v1 + 1];
		Sector[n].z1 = SectorData[v1 + 2];
		Sector[n].z2 = SectorData[v1 + 3] - Sector[n].z1;
		Sector[n].c1 = SectorData[v1 + 4]; // Sector bottom color
		Sector[n].c2 = SectorData[v1 + 5]; // Sector top color
		v1 += 6;

		for (int w = Sector[n].wallStart; w < Sector[n].wallEnd; w++)
		{
			Wall[w].x1 = WallData[v2];
			Wall[w].y1 = WallData[v2 + 1];
			Wall[w].x2 = WallData[v2 + 2];
			Wall[w].y2 = WallData[v2 + 3];
			Wall[w].c =  WallData[v2 + 4];
			v2 += 5;
		}
	}
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition(GLSW / 2, GLSH / 2);
	glutInitWindowSize(GLSW, GLSH);
	glutCreateWindow("");
	glPointSize(pixelScale);                        //pixel size
	gluOrtho2D(0, GLSW, 0, GLSH);                      //origin bottom left
	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(KeysDown);
	glutKeyboardUpFunc(KeysUp);
	glutMainLoop();
	return 0;
}
