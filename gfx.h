 /*
A simple graphics library for CSE 20211 by Douglas Thain
For course assignments, you should not change this file.
For complete documentation, see:
http://www.nd.edu/~dthain/courses/cse20211/fall2011/gfx
version 4, 01/29/2020 - Added missing window size functions and fixed key lookup.
Version 3, 11/07/2012 - Now much faster at changing colors rapidly.
Version 2, 9/23/2011 - Fixes a bug that could result in jerky animation.
*/

#ifndef GFX_H
#define GFX_H

/* Open a new graphics window. */
void gfx_open(int width, int height, const char *title);

/* Draw a point at (x,y) */
void gfx_point(int x, int y);

/* Draw a line from (x1,y1) to (x2,y2) */
void gfx_line(int x1, int y1, int x2, int y2);

/* Change the current drawing color. */
void gfx_color(int red, int green, int blue);

/* Clear the graphics window to the background color. */
void gfx_clear();

/* Change the current background color. */
void gfx_background(int red, int green, int blue);

/* Wait for the user to press a key or mouse button. */
int gfx_wait();

/* Return the X and Y coordinates of the last event. */
int gfx_xpos();
int gfx_ypos();

/* Return the X and Y dimensions of the window. */
int gfx_xsize();
int gfx_ysize();

/* Check to see if an event is waiting. */
int gfx_event_waiting();

/* Flush all previous output to the window. */
void gfx_flush();

/* Draw a triangle on three points: (x1, y1), (x2, y2), (x3, y3) */
void gfx_triangle(int x1, int y1, int x2, int y2, int x3, int y3);

/* Fill a triangle erected on three points: (x1, y1), (x2, y2), (x3, y3) */
void gfx_fill_triangle(int x1, int y1, int x2, int y2, int x3, int y3);

/* Draw a rectangle at point (x, y) with (width) and (height) */
void gfx_rectangle(int x, int y, unsigned int width, unsigned int height);

/* Fill a rectangle */
void gfx_fill_rectangle(int x, int y, unsigned int width, unsigned int height);

/* Draw a segment of an arc from (start) to (end) in degrees  in a relative position to the point (x, y) with axes (width) and (height) */
void gfx_arc(int x, int y, unsigned int width, unsigned int height, int start, int length);

/* Draw a circular segment from (start) to (end) in a relative position to the point (x, y) with (radius) */
void gfx_segment(int x, int y, unsigned int radius, int start, int length);

/* Fill a segment of an arc (see: gfx_arc) */
void gfx_fill_arc(int x, int y, unsigned int width, unsigned int height, int start, int length);

/* Fill a circular segment (see: gfx_segment) */
void gfx_fill_segment(int x, int y, unsigned int radius, int start, int length);

/* Defined in gfx.c for convenience */
extern const int Solid;
extern const int Dashed;

/* Change the current style of drawing lines (its (width) and (style): Solid, Dashed), change is applied until the next change */
void gfx_line_style(unsigned int width, int style);

/* Draw the given (str) starting at the position (x, y) */
void gfx_text(int x, int y, char *str, int len);

// void gfx_wait_string(char end, char* str, const int buf);

#endif
