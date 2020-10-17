#include "window_xcb.h"

#include "device_xcb.h"

namespace gin {
XcbWindow::XcbWindow(XcbDevice& device, xcb_window_t xcb_window, xcb_colormap_t xcb_colormap, const VideoMode& video_mode, std::string_view title_view):
	device{device},
	xcb_window{xcb_window},
	xcb_colormap{xcb_colormap},
	video_mode{video_mode},
	window_title{title_view}
{
}

XcbWindow::~XcbWindow(){
	device.windowDestroyed(xcb_window);
	xcb_destroy_window(device.xcb_connection, xcb_window);
	device.flush();
}

const VideoMode& XcbWindow::videoMode() const{
	return video_mode;
}

const std::string_view XcbWindow::title() const {
	return window_title;
}
}