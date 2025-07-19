/*
A simple graphics library for CSE 20211 by Douglas Thain
For complete documentation, see:
http://www.nd.edu/~dthain/courses/cse20211/fall2011/gfx
version 4, 01/29/2020 - Added missing window size functions and fixed key lookup.
Version 4, 01/20/2020 - Added missing window size functions.
Version 3, 11/07/2012 - Now much faster at changing colors rapidly.
Version 2, 9/23/2011 - Fixes a bug that could result in jerky animation.
*/

/*
A simple (yet more powerful) library for SPBU CS class
by Georgy Brovkin
Version 5, 05/03/2025 - Added functionality.
*/

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "gfx.h"

/*
gfx_open creates several X11 objects, and stores them in globals
for use by the other functions in the library.
*/

static Display *gfx_display=0;
static Window  gfx_window;
static GC      gfx_gc;
static Colormap gfx_colormap;
static int      gfx_fast_color_mode = 0;

/* These values are saved by gfx_wait then retrieved later by gfx_xpos and gfx_ypos. */

static int saved_xpos = 0;
static int saved_ypos = 0;
static int saved_xsize = 0;
static int saved_ysize = 0;

/* Variables */

const int Solid = 0;
const int Dashed = 1;

/* Open a new graphics window. */

void gfx_open(int width, int height, const char *title)
{
	gfx_display = XOpenDisplay(0);
	if(!gfx_display) {
		fprintf(stderr,"gfx_open: unable to open the graphics window.\n");
		exit(1);
	}

	Visual *visual = DefaultVisual(gfx_display,0);
	if(visual && visual->class==TrueColor) {
		gfx_fast_color_mode = 1;
	} else {
		gfx_fast_color_mode = 0;
	}

	int blackColor = BlackPixel(gfx_display, DefaultScreen(gfx_display));
	int whiteColor = WhitePixel(gfx_display, DefaultScreen(gfx_display));

	gfx_window = XCreateSimpleWindow(gfx_display, DefaultRootWindow(gfx_display), 0, 0, width, height, 0, blackColor, blackColor);

	XSetWindowAttributes attr;
	attr.backing_store = Always;

	XChangeWindowAttributes(gfx_display,gfx_window,CWBackingStore,&attr);

	XStoreName(gfx_display,gfx_window,title);

	XSelectInput(gfx_display, gfx_window, StructureNotifyMask|KeyPressMask|ButtonPressMask);

	XMapWindow(gfx_display,gfx_window);

	gfx_gc = XCreateGC(gfx_display, gfx_window, 0, 0);

	gfx_colormap = DefaultColormap(gfx_display,0);

	XSetForeground(gfx_display, gfx_gc, whiteColor);

	// Wait for the MapNotify event

	for(;;) {
		XEvent e;
		XNextEvent(gfx_display, &e);
		if (e.type == MapNotify)
			break;
	}

	saved_xsize = width;
	saved_ysize = height;
}

/* Draw a single point at (x,y) */

void gfx_point(int x, int y)
{
	XDrawPoint(gfx_display,gfx_window,gfx_gc,x,y);
}

/* Draw a line from (x1,y1) to (x2,y2) */

void gfx_line(int x1, int y1, int x2, int y2 )
{
	XDrawLine(gfx_display, gfx_window, gfx_gc, x1, y1, x2, y2);
}

/* Change the current drawing color. */

void gfx_color(int r, int g, int b)
{
	XColor color;

	if(gfx_fast_color_mode) {
		/* If this is a truecolor display, we can just pick the color directly. */
		color.pixel = ((b&0xff) | ((g&0xff)<<8) | ((r&0xff)<<16) );
	} else {
		/* Otherwise, we have to allocate it from the colormap of the display. */
		color.pixel = 0;
		color.red = r<<8;
		color.green = g<<8;
		color.blue = b<<8;
		XAllocColor(gfx_display,gfx_colormap,&color);
	}

	XSetForeground(gfx_display, gfx_gc, color.pixel);
}

/* Clear the graphics window to the background color. */

void gfx_clear()
{
	XClearWindow(gfx_display,gfx_window);
}

/* Change the current background color. */

void gfx_background(int r, int g, int b)
{
	XColor color;
	color.pixel = 0;
	color.red = r<<8;
	color.green = g<<8;
	color.blue = b<<8;
	XAllocColor(gfx_display,gfx_colormap,&color);

	XSetWindowBackground(gfx_display, gfx_window, color.pixel);

	XClearWindow(gfx_display, gfx_window);
	
	gfx_flush();
}

int gfx_event_waiting()
{
       XEvent event;

       gfx_flush();

       while (1) {
               if(XCheckMaskEvent(gfx_display,-1,&event)) {
                       if(event.type==KeyPress) {
                               XPutBackEvent(gfx_display,&event);
                               return 1;
                       } else if (event.type==ButtonPress) {
                               XPutBackEvent(gfx_display,&event);
                               return 1;
                       } else {
                               return 0;
                       }
               } else {
                       return 0;
               }
       }
}

/* Wait for the user to press a key or mouse button. */

int gfx_wait()
{
	XEvent event;

	gfx_flush();

	while(1) {
		XNextEvent(gfx_display,&event);

		if(event.type==KeyPress) {
			saved_xpos = event.xkey.x;
			saved_ypos = event.xkey.y;

			/* If the key sequence maps to an ascii character, return that. */
			KeySym symbol;
			char str[4];
			int r = XLookupString(&event.xkey,str,sizeof(str),&symbol,0);
			if(r==1) return str[0];

			/* Special case for navigation keys, return codes above 129. */
			if(symbol>=0xff50 && symbol<=0xff58) {
				return 129 + (symbol-0xff50);
			}

		} else if(event.type==ButtonPress) {
			saved_xpos = event.xkey.x;
			saved_ypos = event.xkey.y;
			return event.xbutton.button;
		} else if(event.type==ConfigureNotify) {
			saved_xsize = event.xconfigure.width;
			saved_ysize = event.xconfigure.height;
		}
	}
}

/* Return the X and Y coordinates of the last event. */

int gfx_xpos()
{
	return saved_xpos;
}

int gfx_ypos()
{
	return saved_ypos;
}

/* Flush all previous output to the window. */

void gfx_flush()
{
	XFlush(gfx_display);
}

int gfx_xsize()
{
	return saved_xsize;
}

int gfx_ysize()
{
	return saved_ysize;
}

void gfx_rectangle(int x, int y, unsigned int width, unsigned int height) {
  XDrawRectangle(gfx_display, gfx_window, gfx_gc, x, y, width, height);
}

void gfx_fill_rectangle(int x, int y, unsigned int width, unsigned height) {
  XFillRectangle(gfx_display, gfx_window, gfx_gc, x, y, width, height);
}

void gfx_arc(int x, int y, unsigned int width, unsigned int height, int start, int length) {
  XDrawArc(gfx_display, gfx_window, gfx_gc, x, y, width, height, start, length);
}

void gfx_segment(int x, int y, unsigned int radius, int start, int length) {
  gfx_arc(x, y, radius, radius, start, length);
}

void gfx_fill_arc(int x, int y, unsigned int width, unsigned int heigth, int start, int length) {
  XFillArc(gfx_display, gfx_window, gfx_gc, x, y, width, heigth, start, length);
}

void gfx_fill_segment(int x, int y, unsigned radius, int start, int length) {
  gfx_fill_arc(x, y, radius, radius, start, length);
}

void gfx_triangle(int x1, int y1, int x2, int y2, int x3, int y3) {
  XDrawLine(gfx_display, gfx_window, gfx_gc, x1, y1, x2, y2);
  XDrawLine(gfx_display, gfx_window, gfx_gc, x2, y2, x3, y3);
  XDrawLine(gfx_display, gfx_window, gfx_gc, x3, y3, x1, y1);
}

void gfx_fill_triangle(int x1, int y1, int x2, int y2, int x3, int y3) {
  XPoint pp[3] = {{x1, y1}, {x2, y2}, {x3, y3}};
  XFillPolygon(gfx_display, gfx_window, gfx_gc, pp, 3, Convex, CoordModeOrigin);
}

void gfx_line_style(unsigned width, int style) {
  int s;

  switch (style) {
  case 0:
    s = LineSolid;
    break;
  case 1:
    s = LineOnOffDash;
    break;
  }
  
  XSetLineAttributes(gfx_display, gfx_gc, width, s, CapButt, JoinRound);
}

void gfx_text(int x, int y, char *str, int len) {
  // XTextItem tt[1] = {{str, , 10, None}};
  // XDrawText(gfx_display, gfx_window, gfx_gc, x, y, tt, 1);
  XDrawImageString(gfx_display, gfx_window, gfx_gc, x, y, str, len);
}

// void gfx_wait_string(char end, char* str, const int buf) {
//  char c;
//  for (int i = 0; i < buf && (c = gfx_wait()) != end; ++i) {
//    str[i] = c;
//  }
// }
