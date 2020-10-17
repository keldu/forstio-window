#pragma once

#include "device.h"
#include "window_xcb.h"

#include <string>
#include <map>

#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#include <xcb/xcb.h>

namespace gin {
class XcbDevice final : public Device {
private:
	::Display* display;
	int screen;

	xcb_connection_t* xcb_connection;
	xcb_screen_t* xcb_screen;

	std::map<xcb_window_t, XcbWindow*> windows;
public:
	XcbDevice(::Display* display, int screen, xcb_connection_t* xcb_connection, xcb_screen_t* xcb_screen);
	~XcbDevice();

	void windowDestroyed(xcb_window_t window_id);

	Own<XcbWindow> createXcbWindow(const VideoMode& mode, const std::string& title);
	Own<Window> createWindow(const VideoMode& video_mode, const std::string& title) override;

	void flush() override;
};

Own<XcbDevice> createXcbDevice();
}
