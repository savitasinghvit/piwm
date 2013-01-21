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
    Window root = DefaultRootWindow(display);
    XEvent event;
    int num_sizes;
    Rotation original_rotation;
    XWindowAttributes win_attr;
    XButtonEvent sev;
    XRRScreenSize *xrrs = XRRSizes(display, 0, &num_sizes);
    XRRScreenConfiguration *conf = XRRGetScreenInfo(display, root);
    short original_rate = XRRConfigCurrentRate(conf);
    SizeID original_size_id = XRRConfigCurrentConfiguration(conf, &original_rotation);
    KeyCode t = XKeysymToKeycode(display, XStringToKeysym("t"));
    KeyCode F = XKeysymToKeycode(display, XStringToKeysym("F"));
    KeyCode F1 = XKeysymToKeycode(display, XStringToKeysym("F1"));
    if (!(display)) return 1;
    XGrabKey(display, AnyKey, ControlMask, root, True, GrabModeAsync, GrabModeAsync);
    XGrabButton(display, 1, Mod1Mask, root, True, ButtonPressMask, GrabModeAsync, GrabModeAsync, None, None);
    XGrabButton(display, 3, Mod1Mask, root, True, ButtonPressMask, GrabModeAsync, GrabModeAsync, None, None);
    while (1) {
        XNextEvent(display, &event);
        if (event.type == KeyPress && event.xkey.subwindow == None) {
            if (event.xkey.keycode == t && event.xkey.state & ControlMask) spawnXterm();
        }
        if (event.type == KeyPress && event.xkey.subwindow != None) {
            if (event.xkey.keycode == F1 && event.xkey.state & ControlMask) XRaiseWindow(display, event.xkey.subwindow);
            if (event.xkey.keycode == F && event.xkey.state & ControlMask) XMoveResizeWindow(display, event.xkey.subwindow, 0, 0, xrrs[original_size_id].width,  xrrs[original_size_id].height);
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