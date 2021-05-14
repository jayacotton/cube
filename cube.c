#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

#include "gems.h"

#define SWAP(a,b)  {a^=b; b^=a; a^=b;}
#define absolute(i,j,k) ((i-j)*(k=((i-j)<0 ? -1 : 1)))


extern void oled_test();
extern void oled_draw_image(char *, int);
extern void oled_clean();
extern void oled_set_full_screen();
extern void oled_clear();
extern void oled_init();

#define MAXBUF 600
unsigned char image[MAXBUF];

void refresh()
{
    oled_draw_image(image, MAXBUF);
}

int row, col;

struct IntPoint3Struct Cube[24] = {
    { 0, 16, 16 }, { 16, 16, 16 },
    { 16, 0, 16 }, { 16, 16, 16 },
    { 0, 0, 16 }, { 0, 16, 16 },
    { 0, 0, 16 }, { 16, 0, 16 },

    { 0, 0, 16 }, { 0, 0, 0 },
    { 0, 16, 16 }, { 0, 16, 0 },
    { 16, 0, 16 }, { 16, 0, 0 },
    { 16, 16, 16 }, { 16, 16, 0 },

    { 0, 16, 0 }, { 16, 16, 0 },
    { 16, 0, 0 }, { 16, 16, 0 },
    { 0, 0, 0 }, { 0, 16, 0 },
    { 0, 0, 0 }, { 16, 0, 0 },
};

#define NCUBE (sizeof (Cube)/sizeof (struct IntPoint3Struct))
void unity(struct Matrix4Struct *mat)
{
    int i, j;
    for (i = 0; i < 4; i++)
	for (j = 0; j < 4; j++)
	    mat->element[i][j] = 0.0;
    mat->element[0][0] = mat->element[1][1] = mat->element[2][2] =
	mat->element[3][3] = 1.0;
}

void move(int x, int y)
{
// confine to the display space only.
//      x = abs(x);
//      y = abs(y);
//      if(x >= 32) 
//              x = 31;
//      if(y >= 128) 
//              y = 127;
    row = x;
    col = y;
}

// we need to decide which pixel is to be written, and if
// its a 0 we can just skip it (for now)
//  there are 8 pixels in the byte, the byte is arranged in
//  scan line order with then lsb at bit 0 and the msb at
//  bit 7.  So the position of the one in the byte tells us
//  the scan line that we need to address.
//  the dx dy tell us the absolute position that we are requesting
//  Need to calculate the memory address and or in the bit.
//
// dx can be 0 to 31          
// dy can be 0 to 127           
//
//  work space dimensions
//  --> 128 columns 
//  +---------------------------------------------------------------------~ ~ -----------+ v 32 bits
//  |---------------------------------------------------------------------~ ~ -----------|
//  |---------------------------------------------------------------------~ ~ -----------|
//  +---------------------------------------------------------------------~ ~ -----------+
//   ^ each column is a 32 bit work broken into 4 bytes.
//
// dx v 
// dy >
//    bit 0      V
// scanline  0 > 1 0 0 0 0  etc.  v 8 bits      makes a \
//               0 1 0 0 0
//               0 0 1 0 0
//               0 0 0 1 0
//               0 0 0 0 1
//               0 0 0 0 0
//               0 0 0 0 0
//    bit 7   >  0 0 0 0 0      -> 127 bytes 
void putpix(unsigned char pixel)
{
    int addr;
    int scanline;
//char buff[33];
    //printf ("put pixel %02x ",pixel);
//itoa(pixel,buff,2);
//printf("%s",buff);
// printf (" at dx %d dy %d ", row, col);
    if (pixel) {
// primitive clip
	if (col >= 127)
	    return;
	if (row >= 32)
	    return;
	if (col < 0)
	    return;
	if (row < 0)
	    return;
	// scanline should be 0 to 3   There are 4 scanlines on the display
	scanline = row / 8;
	// dx is the byte position in the scanline
	addr = col + (scanline * 132);
	//     printf ("scanline %d addr %d\n", scanline, addr);
	image[addr] |= pixel;
    }
}

extern void DRAW(unsigned char);
void setpixel(x, y)
{
    move(x, y);
    DRAW(1 << (x & 0x7));
}

void DRAW(unsigned char dc)
{
//  move (row, col);
    putpix(dc);
}

/* non-zero flag indicates the pixels needing SWAP back. */
void plot(int x, int y, int flag)
{
    if (flag) {
	row = y;
	col = x;
	DRAW(1 << (row & 0x7));
    } else {
	row = x;
	col = y;
	DRAW(1 << (row & 0x7));
    }
}

void plotlinelow(int x0, int y0, int x1, int y1)
{
    int dy, dx, D, y, x, yi;
    dx = x1 - x0;
    dy = y1 - y0;
    yi = 1;
    if (dy < 0) {
	yi = -1;
	dy = -dy;
    }
    D = 2 * dy - dx;
    y = y0;
    for (x = x0; x < x1; x++) {
	setpixel(x, y);
	if (D > 0) {
	    y = y + yi;
	    D = D - 2 * dx;
	}
	D = D + 2 * dy;
    }
}

void plotlinehigh(int x0, int y0, int x1, int y1)
{
    int dx, dy, xi, D, x, y;
    dx = x1 - x0;
    dy = y1 - y0;
    xi = 1;
    if (dx < 0) {
	xi = -1;
	dx = -dx;
    }
    D = 2 * dx - dy;
    x = x0;
    for (y = y0; y < y1; y++) {
	setpixel(x, y);
	if (D > 0) {
	    x = x + xi;
	    D = D - 2 * dy;
	}
	D = D + 2 * dx;
    }
}

void plotline(int x0, int y0, int x1, int y1)
{
    if (abs(y1 - y0) < abs(x1 - x0)) {
	if (x0 > x1) {
	    plotlinelow(x1, y1, x0, y0);
	} else {
	    plotlinelow(x0, y0, x1, y1);
	}
    } else {
	if (y0 > y1) {
	    plotlinehigh(x1, y1, x0, y0);
	} else {
	    plotlinehigh(x0, y0, x1, y1);
	}
    }
}

void
transform(struct Matrix4Struct *mat, struct IntPoint3Struct *Pin,
	  struct IntPoint3Struct *Pout)
{
    double_t w;
    Pout->x =
	(Pin->x * mat->element[0][0]) + (Pin->y * mat->element[1][0]) +
	(Pin->z * mat->element[2][0]) + mat->element[3][0];
    Pout->y =
	(Pin->x * mat->element[0][1]) + (Pin->y * mat->element[1][1]) +
	(Pin->z * mat->element[2][1]) + mat->element[3][1];
    Pout->z =
	(Pin->x * mat->element[0][2]) + (Pin->y * mat->element[3][2]) +
	(Pin->z * mat->element[2][2]) + mat->element[3][2];
    w = (Pin->x * mat->element[0][3]) + (Pin->y * mat->element[1][3]) +
	(Pin->z * mat->element[2][3]) + mat->element[3][3];
    if (w != 0.0) {
	Pout->x /= w;
	Pout->y /= w;
	Pout->z /= w;
    }
}

void rotate(struct Matrix4Struct *mat, float angle, int axis)
{
    float rads;
    float c;

    rads = angle * 3.1415928 / 180.0;
    switch (axis) {
    case 1:			/* x axis */
	mat->element[1][1] = c = cos(rads);
	mat->element[1][2] = sin(rads);
	mat->element[2][1] = -sin(rads);
	mat->element[2][2] = c;
	break;
    case 2:			/* y axis */
	mat->element[0][0] = c = cos(rads);
	mat->element[0][2] = -sin(rads);
	mat->element[2][0] = sin(rads);
	mat->element[2][2] = c;
	break;
    case 3:			/* z axis */
	mat->element[0][0] = c = cos(rads);
	mat->element[0][1] = sin(rads);
	mat->element[1][0] = -sin(rads);
	mat->element[1][1] = c;
	break;
    default:
	break;
    }
}

void main()
{
    int i, j;
    char d;
    int x1, y1, x2, y2, z1, z2;
    float ang;
    struct IntPoint3Struct *CubePtr, P1, P2;
    struct Matrix4Struct matrix;
    oled_init();
    oled_clear();
    oled_set_full_screen();
    memset(image, 0, MAXBUF);
    unity(&matrix);
// x
//  plotline (0, 0, 31, 31);
//  plotline (31, 0, 0, 31);
// box
//  plotline (0, 0, 0,127 );
//  plotline (0, 127, 31,127 );
//  plotline (31, 127, 31,0 );
//  plotline (31, 0, 0, 0);
//  refresh ();
    memset(image, 0, MAXBUF);
    ang = 10.0;
    while(1){
	if(bdos,(CPM_ICON,0)) exit(0);
	CubePtr = &Cube[0];
	for (i = 0; i < NCUBE; i += 2) {
	    transform(&matrix, CubePtr, &P1);
	    x1 = P1.x;
	    y1 = P1.y;
	    z1 = P1.z;
	    CubePtr++;
	    transform(&matrix, CubePtr, &P2);
	    x2 = P2.x;
	    y2 = P2.y;
	    z2 = P2.z;
	    CubePtr++;
	    plotline(x1 + 16, y1 + 16, x2 + 16, y2 + 16);
	}
	rotate(&matrix, ang, 1);
	rotate(&matrix, ang, 2);
	rotate(&matrix, ang, 3);
	ang += 10.0;
// may need to gather all this into the refresh command
	//oled_init();
	oled_set_full_screen();
	refresh();
	memset(image, 0, MAXBUF);
    }
}
