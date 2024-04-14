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
    int workspace;
} WindowInfo;

int main() {
    Display *display;
    Window rootWindow, titleBar, statusBar;
    XEvent event;
    XSetWindowAttributes titleBarAttributes, statusBarAttributes;
    XFontStruct *fontInfo;
    GC gc;
    XTextProperty textProp;
    char titleBarText[256];
    WindowInfo windows[MAX_WINDOWS];
    int numWindows = 0;
    int currentWorkspace = 1;
    int focusedWindow = -1; // Index of the currently focused window
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

    // Create the title bar window
    titleBarAttributes.override_redirect = True;
    titleBar = XCreateWindow(display, rootWindow, 0, 0,
                             DisplayWidth(display, screen), TITLE_BAR_HEIGHT,
                             0, CopyFromParent, InputOutput, CopyFromParent,
                             CWOverrideRedirect, &titleBarAttributes);

    // Create the status bar window
    statusBarAttributes.override_redirect = True;
    statusBarAttributes.background_pixel = BlackPixel(display, screen);
    statusBar = XCreateWindow(display, rootWindow, 0,
                              DisplayHeight(display, screen) - STATUS_BAR_HEIGHT,
                              DisplayWidth(display, screen), STATUS_BAR_HEIGHT,
                              0, CopyFromParent, InputOutput, CopyFromParent,
                              CWOverrideRedirect | CWBackPixel, &statusBarAttributes);

    // Set up graphics context for drawing on title bar
    gc = XCreateGC(display, titleBar, 0, NULL);
    XSetFont(display, gc, fontInfo->fid);

    // Map the windows
    XMapWindow(display, titleBar);
    XMapWindow(display, statusBar);

    // Main event loop
    while (1) {
        XNextEvent(display, &event);

        // Handle KeyPress events
        if (event.type == KeyPress) {
            KeySym keySym = XLookupKeysym(&event.xkey, 0);
            switch (keySym) {
                case XK_Escape:
                    // Quit entire titling manager when Escape key is pressed
                    for (int i = 0; i < numWindows; i++) {
                        XDestroyWindow(display, windows[i].window);
                    }
                    XDestroyWindow(display, titleBar);
                    XDestroyWindow(display, statusBar);
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

        // Handle FocusIn events
        if (event.type == FocusIn) {
            // Find the window that gained focus
            for (int i = 0; i < numWindows; i++) {
                if (windows[i].window == event.xfocus.window) {
                    focusedWindow = i;
                    break;
                }
            }
        }

        // Handle FocusOut events
        if (event.type == FocusOut) {
            // Reset focusedWindow when a window loses focus
            focusedWindow = -1;
        }

        // Redraw title bar and status bar
        if (focusedWindow >= 0 && focusedWindow < numWindows) {
            // Display title of focused window
            sprintf(titleBarText, "{Program Name: %s} [%d] [%d] [%d]",
                    windows[focusedWindow].title,
                    (currentWorkspace == 1), (currentWorkspace == 2), (currentWorkspace == 3));
        } else {
            // No window is focused
            sprintf(titleBarText, "{No Program Focused} [%d] [%d] [%d]",
                    (currentWorkspace == 1), (currentWorkspace == 2), (currentWorkspace == 3));
        }
        XDrawString(display, titleBar, gc, 10, 20, titleBarText, strlen(titleBarText));
        XDrawString(display, statusBar, gc, 10, 20, "Status Bar", 10);
    }

    // This code is unreachable since the program exits via the escape key press

    return 0;
}
