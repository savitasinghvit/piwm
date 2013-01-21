#include <X11/Xlib.h>
#include<X11/extensions/Xrandr.h>
#define max(a, b) ((a) > (b) ? (a) : (b))

void spawnXterm() {
    if (fork() == 0) {
        setsid();
        execvp("xterm", NULL);
    }
}

int main() {
    Display * display = XOpenDisplay(0x0);
    char *piwm = "Piwm 0.1";
    Window root = DefaultRootWindow(display);
    XEvent event;
    int num_sizes;
    Rotation original_rotation;
    XWindowAttributes win_attr;
    XButtonEvent sev;
    KeyCode t = XKeysymToKeycode(display, XStringToKeysym("t"));
    KeyCode F = XKeysymToKeycode(display, XStringToKeysym("F"));
    KeyCode F1 = XKeysymToKeycode(display, XStringToKeysym("F1"));
    XRRScreenSize *xrrs = XRRSizes(display, 0, &num_sizes);
    XRRScreenConfiguration *conf = XRRGetScreenInfo(display, root);
    short original_rate = XRRConfigCurrentRate(conf);
    SizeID original_size_id = XRRConfigCurrentConfiguration(conf, &original_rotation);
    if (!(display)) return 1;
    XGrabKey(display, AnyKey, ControlMask, root, True, GrabModeAsync, GrabModeAsync);
    XGrabButton(display, 1, Mod1Mask, root, True, ButtonPressMask, GrabModeAsync, GrabModeAsync, None, None);
    XGrabButton(display, 3, Mod1Mask, root, True, ButtonPressMask, GrabModeAsync, GrabModeAsync, None, None);
    char dark_grey[] = "#aaaaaa";
    GC green_gc;
    Colormap colormap;
    XColor green_col;
    colormap = DefaultColormap(display, 0);
    green_gc = XCreateGC(display, RootWindow(display, DefaultScreen(display)), 0, 0);
    XParseColor(display, colormap, dark_grey, &green_col);
    XAllocColor(display, colormap, &green_col);
    XSetForeground(display, green_gc, green_col.pixel);
    while (1) {
        XDrawLine(display,RootWindow(display, DefaultScreen(display)), green_gc, 0, 20, xrrs[original_size_id].width-1, 20);
        XDrawString(display, RootWindow(display, DefaultScreen(display)), green_gc, xrrs[original_size_id].width - 100, 15, piwm, strlen(piwm));
        XNextEvent(display, &event);
        if (event.type == KeyPress && event.xkey.subwindow == None) {
            if (event.xkey.keycode == t && event.xkey.state & ControlMask) spawnXterm();
        }
        if (event.type == KeyPress && event.xkey.subwindow != None) {
            if (event.xkey.keycode == F1 && event.xkey.state & ControlMask) XRaiseWindow(display, event.xkey.subwindow);
            if (event.xkey.keycode == F && event.xkey.state & ControlMask) XMoveResizeWindow(display, event.xkey.subwindow, 0, 0, xrrs[original_size_id].width, xrrs[original_size_id].height);
        } else if (event.type == ButtonPress && event.xbutton.subwindow != None) {
            XGrabPointer(display, event.xbutton.subwindow, True, PointerMotionMask | ButtonReleaseMask, GrabModeAsync,
                    GrabModeAsync, None, None, CurrentTime);
            XGetWindowAttributes(display, event.xbutton.subwindow, &win_attr);
            sev = event.xbutton;
        } else if (event.type == MotionNotify) {
            while (XCheckTypedEvent(display, MotionNotify, &event));
            XMoveResizeWindow(display, event.xmotion.window,
                    win_attr.x + (sev.button == 1 ? event.xbutton.x_root - sev.x_root : 0),
                    win_attr.y + (sev.button == 1 ? event.xbutton.y_root - sev.y_root : 0),
                    max(1, win_attr.width + (sev.button == 3 ? event.xbutton.x_root - sev.x_root : 0)),
                    max(1, win_attr.height + (sev.button == 3 ? event.xbutton.y_root - sev.y_root : 0)));
        } else if (event.type == ButtonRelease)
            XUngrabPointer(display, CurrentTime);
    }
}