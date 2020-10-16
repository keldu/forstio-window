#include "device-xcb.h"

#include <cassert>

#include "window-xcb.h"

namespace gin {
XcbDevice::XcbDevice(::Display* display, int screen, xcb_connection_t* xcb_connection, xcb_screen_t* xcb_screen):
	display{display},
	screen{screen},
	xcb_connection{xcb_connection},
	xcb_screen{xcb_screen}
{
}

XcbDevice::~XcbDevice()
{
	if(display){
		xcb_flush(xcb_connection);
		::XCloseDisplay(display);
	}
}

void XcbDevice::windowDestroyed(xcb_window_t window_id){
	windows.erase(window_id);
}

Own<XcbWindow> XcbDevice::createXcbWindow(const VideoMode& video_mode, const std::string& title){
	return nullptr;
}

Own<Window> XcbDevice::createWindow(const VideoMode& video_mode, const std::string& title){
	return createXcbWindow(video_mode, title);
}

void XcbDevice::flush(){
	assert(xcb_connection);
	xcb_flush(xcb_connection);
}

Own<XcbDevice> createXcbDevice(){
	::Display* display = ::XOpenDisplay(nullptr);
	if(!display){
		/// @todo log errors
		return nullptr;
	}

	int screen = ::XDefaultScreen(display);

	xcb_connection_t* xcb_connection = ::XGetXCBConnection(display);
	if(!xcb_connection){
		/// @todo log errors
		::XCloseDisplay(display);
		return nullptr;
	}

	::XSetEventQueueOwner(display, XCBOwnsEventQueue);

	xcb_screen_iterator_t screen_iter = 
		xcb_setup_roots_iterator(xcb_get_setup(xcb_connection));
	for(int screen_i = screen; screen_iter.rem && screen_i > 0; --screen_i, xcb_screen_next(&screen_iter));
	
	xcb_screen_t* xcb_screen = screen_iter.data;

	int fd = xcb_get_file_descriptor(xcb_connection);

	return heap<XcbDevice>(display, screen, xcb_connection, xcb_screen);
}

Own<Device> createDevice(){
	return createXcbDevice();
}
}