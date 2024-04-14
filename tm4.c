#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define STATUS_BAR_HEIGHT 30 // Adjust as needed

int main() {
    Display *display;
    Window rootWindow, mainWindow, statusBar;
    XEvent event;
    XSetWindowAttributes mainAttributes, statusAttributes;
    int screen;

    display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "Error: Could not open display.\n");
        exit(1);
    }

    screen = DefaultScreen(display);
    rootWindow = RootWindow(display, screen);

    // Create the main window
    mainAttributes.override_redirect = True; // Fullscreen, no window manager decorations
    mainWindow = XCreateWindow(display, rootWindow, 0, 0,
                               DisplayWidth(display, screen),
                               DisplayHeight(display, screen) - STATUS_BAR_HEIGHT,
                               0, CopyFromParent, InputOutput, CopyFromParent,
                               CWOverrideRedirect, &mainAttributes);

    // Create the status bar window
    statusAttributes.override_redirect = True;
    statusAttributes.background_pixel = BlackPixel(display, screen);
    statusBar = XCreateWindow(display, rootWindow, 0,
                              DisplayHeight(display, screen) - STATUS_BAR_HEIGHT,
                              DisplayWidth(display, screen), STATUS_BAR_HEIGHT,
                              0, CopyFromParent, InputOutput, CopyFromParent,
                              CWOverrideRedirect | CWBackPixel, &statusAttributes);

    // Map the windows
    XMapWindow(display, mainWindow);
    XMapWindow(display, statusBar);

    // Event loop
    while (1) {
        XNextEvent(display, &event);
        if (event.type == KeyPress || event.type == ButtonPress) {
            // Exit if a key or mouse button is pressed
            break;
        }
    }

    // Destroy the windows
    XDestroyWindow(display, mainWindow);
    XDestroyWindow(display, statusBar);
    XCloseDisplay(display);

    return 0;
}
