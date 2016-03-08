#include <cairo.h>
#include <cairo-xlib.h>
#include <X11/Xlib.h>

#include <stdio.h>

int main(int argc, char** argv) {
	Display *disp;
	Window root;
	cairo_surface_t *surface;
	int scr;

	disp = XOpenDisplay(NULL);
	scr = DefaultScreen(disp);
	root = DefaultRootWindow(disp);

	surface = cairo_xlib_surface_create(disp, root, DefaultVisual(disp, scr),
			DisplayWidth(disp, scr), DisplayHeight(disp, scr));
	
	
	//MOI
	cairo_surface_flush(surface);
	char *data = cairo_image_surface_get_data(surface);
	
	if(data != NULL)
		printf("Data not null\n");
	
	
	//OTHER
	/*cairo_surface_write_to_png(
			surface,
			"test.png");*/
	cairo_surface_destroy(surface);

	return 0;
}
