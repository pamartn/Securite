#include <cairo.h>
#include <cairo-xlib.h>
#include <X11/Xlib.h>
#include <string.h>
#include <stdio.h>

//FILE
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


typedef struct
{
	unsigned char *current_position;
	unsigned char *end_of_array;
} png_stream_to_byte_array_closure_t;

static cairo_status_t write_png_stream_to_byte_array (void *in_closure, const unsigned char *data, unsigned int length) {
	png_stream_to_byte_array_closure_t *closure =
		(png_stream_to_byte_array_closure_t *) in_closure;

	if ((closure->current_position + length) > (closure->end_of_array))
		return CAIRO_STATUS_WRITE_ERROR;

	memcpy (closure->current_position, data, length);
	closure->current_position += length;

	return CAIRO_STATUS_SUCCESS;
}

void copy_surface_to_fd(cairo_surface_t *surface, int fd){
	int buf_size = 101024;
	char byte_array[buf_size];
	
	png_stream_to_byte_array_closure_t closure;	
	closure.current_position = byte_array;
	closure.end_of_array = byte_array + sizeof (byte_array);
	cairo_surface_write_to_png_stream (surface, write_png_stream_to_byte_array,
		&closure);
	
	write(fd, byte_array, buf_size);
	printf("\nFINISHED\n");
	
}


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
	unsigned char *data = cairo_image_surface_get_data(surface);


	int fd = open("hello.png", O_CREAT | O_RDWR);	
	copy_surface_to_fd(surface, fd);
	close(fd);

	//OTHER
	/*cairo_surface_write_to_png(
			surface,
			"test.png");*/
	cairo_surface_destroy(surface);

	return 0;
}
