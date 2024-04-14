#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#define STATUS_BAR_HEIGHT 30
#define TITLE_BAR_HEIGHT 30
#define MAX_WINDOWS 10

// Structure to store information about each window
typedef struct {
    Window window;
    char title[256];
} WindowInfo;

int main() {
    Display *display;
    Window rootWindow, mainWindow, statusBar;
    XEvent event;
    XSetWindowAttributes mainAttributes, statusAttributes;
    XFontStruct *fontInfo;
    GC gc;
    XTextProperty textProp;
    char titleBarText[256];
    WindowInfo windows[MAX_WINDOWS];
    int numWindows = 0;
    int currentWorkspace = 1;
    int screen;

    display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "Error: Could not open display.\n");
        exit(1);
    }

    screen = DefaultScreen(display);
    rootWindow = RootWindow(display, screen);

    // Load font for title bar text
    fontInfo = XLoadQueryFont(display, "fixed");
    if (fontInfo == NULL) {
        fprintf(stderr, "Error: Could not load font.\n");
        exit(1);
    }

    // Create the main window
    mainAttributes.override_redirect = True;
    mainWindow = XCreateWindow(display, rootWindow, 0, TITLE_BAR_HEIGHT,
                               DisplayWidth(display, screen),
                               DisplayHeight(display, screen) - STATUS_BAR_HEIGHT - TITLE_BAR_HEIGHT,
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

    // Set up graphics context for drawing on main window
    gc = XCreateGC(display, mainWindow, 0, NULL);
    XSetFont(display, gc, fontInfo->fid);

    // Map the windows
    XMapWindow(display, mainWindow);
    XMapWindow(display, statusBar);

    // Main event loop
    while (1) {
        XNextEvent(display, &event);

        // Handle KeyPress events
        if (event.type == KeyPress) {
            KeySym keySym = XLookupKeysym(&event.xkey, 0);
            switch (keySym) {
                case XK_Escape:
                    // Quit program when Escape key is pressed
                    XCloseDisplay(display);
                    exit(0);
                    break;
                case XK_1:
                case XK_2:
                case XK_3:
                    // Change workspace when corresponding number key is pressed
                    currentWorkspace = keySym - XK_0;
                    break;
                default:
                    break;
            }
        }

        // Redraw title bar and status bar
        sprintf(titleBarText, "{Program Name: %s} [%d] [%d] [%d]",
                (numWindows > 0) ? windows[numWindows - 1].title : "None",
                (currentWorkspace == 1), (currentWorkspace == 2), (currentWorkspace == 3));
        XDrawString(display, mainWindow, gc, 10, 20, titleBarText, strlen(titleBarText));
        XDrawString(display, statusBar, gc, 10, 20, "Status Bar", 10);
    }

    // Destroy the windows and close display
    XDestroyWindow(display, mainWindow);
    XDestroyWindow(display, statusBar);
    XCloseDisplay(display);

    return 0;
}
