#pragma once

#include <X11/Xlib-xcb.h>
#include <X11/Xlib.h>
#include <kelgin/io.h>
#include <xcb/xcb.h>

#include <map>
#include <vector>

#include "device.h"
#include "window_xcb.h"

namespace gin {
class XcbDevice final : public Device {
public:
	::Display *display;
	int screen;

	xcb_connection_t *xcb_connection;
	xcb_screen_t *xcb_screen;

	Own<InputStream> async_notifier;
	SinkConveyor async_conveyor;

	std::map<xcb_window_t, XcbWindow *> windows;

	std::vector<xcb_generic_event_t*> pending_events;
public:
	XcbDevice(::Display *display, int screen, xcb_connection_t *xcb_connection,
			  xcb_screen_t *xcb_screen, Own<InputStream> &&an);
	~XcbDevice();

	void windowDestroyed(xcb_window_t window_id);
	void handleEvents();

	Own<XcbWindow> createXcbWindow(const VideoMode &mode,
								   std::string_view title_view, int visual_id);
	Own<Window> createWindow(const VideoMode &video_mode,
							 std::string_view title_view) override;

	void flush() override;
};

Own<XcbDevice> createXcbDevice(AsyncIoProvider &provider);
} // namespace gin
