#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

int main() {
    Display *display;
    Window rootWindow, window;
    XEvent event;
    XSetWindowAttributes setWindowAttributes;

    display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "Error: Could not open display.\n");
        exit(1);
    }

    rootWindow = DefaultRootWindow(display);

    // Set window attributes
    setWindowAttributes.border_pixel = BlackPixel(display, DefaultScreen(display));
    setWindowAttributes.event_mask = ExposureMask | KeyPressMask;

    // Create a window
    window = XCreateWindow(display, rootWindow, 100, 100, 400, 300, 0,
                           CopyFromParent, InputOutput, CopyFromParent,
                           CWBorderPixel | CWEventMask, &setWindowAttributes);

    // Map the window
    XMapWindow(display, window);

    // Event loop
    while (1) {
        XNextEvent(display, &event);
        if (event.type == KeyPress) {
            // Exit if a key is pressed
            break;
        } else if (event.type == Expose) {
            // Redraw window on expose event
            XFillRectangle(display, window, DefaultGC(display, DefaultScreen(display)),
                           20, 20, 360, 260);
            XDrawString(display, window, DefaultGC(display, DefaultScreen(display)),
                        50, 50, "Hello, Window!", 14);
        }
    }

    // Destroy the window
    XDestroyWindow(display, window);
    XCloseDisplay(display);

    return 0;
}
