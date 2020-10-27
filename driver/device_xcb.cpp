#include "device_xcb.h"

#include <cassert>

#include "window_xcb.h"

namespace gin {
XcbDevice::XcbDevice(::Display *display, int screen,
					 xcb_connection_t *xcb_connection, xcb_screen_t *xcb_screen,
					 Own<InputStream> &&an)
	: display{display}, screen{screen}, xcb_connection{xcb_connection},
	  xcb_screen{xcb_screen}, async_notifier{std::move(an)},
	  async_conveyor{async_notifier->readReady()
						 .then([this]() { handleEvents(); })
						 .buffer(1)} {}

XcbDevice::~XcbDevice() {
	if (display) {
		xcb_flush(xcb_connection);
		::XCloseDisplay(display);
	}
}

void XcbDevice::windowDestroyed(xcb_window_t window_id) {
	windows.erase(window_id);
}

void XcbDevice::handleEvents() {}

Own<XcbWindow> XcbDevice::createXcbWindow(const VideoMode &video_mode,
										  std::string_view title_view,
										  int visual_id) {
	assert(xcb_screen);
	assert(xcb_connection);

	xcb_colormap_t xcb_colormap = xcb_generate_id(xcb_connection);
	xcb_window_t xcb_window = xcb_generate_id(xcb_connection);

	xcb_create_colormap(xcb_connection, XCB_COLORMAP_ALLOC_NONE, xcb_colormap,
						xcb_screen->root, visual_id);

	uint32_t eventmask = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS;
	uint32_t valuelist[] = {eventmask, xcb_colormap, 0};
	uint32_t valuemask = XCB_CW_EVENT_MASK | XCB_CW_COLORMAP;

	xcb_create_window(xcb_connection, XCB_COPY_FROM_PARENT, xcb_window,
					  xcb_screen->root, 0, 0, video_mode.width,
					  video_mode.height, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT,
					  visual_id, valuemask, valuelist);

	xcb_change_property(xcb_connection, XCB_PROP_MODE_REPLACE, xcb_window,
						XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, title_view.size(),
						title_view.data());

	xcb_flush(xcb_connection);

	return heap<XcbWindow>(*this, xcb_window, xcb_colormap, video_mode,
						   title_view);
}

Own<Window> XcbDevice::createWindow(const VideoMode &video_mode,
									std::string_view title_view) {
	assert(xcb_screen);
	return createXcbWindow(video_mode, title_view, xcb_screen->root_visual);
}

void XcbDevice::flush() {
	assert(xcb_connection);
	xcb_flush(xcb_connection);
}

Own<XcbDevice> createXcbDevice(AsyncIoProvider &provider) {
	::Display *display = ::XOpenDisplay(nullptr);
	if (!display) {
		/// @todo log errors
		return nullptr;
	}

	int screen = ::XDefaultScreen(display);

	xcb_connection_t *xcb_connection = ::XGetXCBConnection(display);
	if (!xcb_connection) {
		/// @todo log errors
		::XCloseDisplay(display);
		return nullptr;
	}

	int fd = xcb_get_file_descriptor(xcb_connection);

	Own<InputStream> fd_wrapped = provider.wrapInputFd(fd);
	if (!fd_wrapped) {
		::XCloseDisplay(display);
		return nullptr;
	}

	::XSetEventQueueOwner(display, XCBOwnsEventQueue);

	xcb_screen_iterator_t screen_iter =
		xcb_setup_roots_iterator(xcb_get_setup(xcb_connection));
	for (int screen_i = screen; screen_iter.rem && screen_i > 0;
		 --screen_i, xcb_screen_next(&screen_iter))
		;

	xcb_screen_t *xcb_screen = screen_iter.data;

	return heap<XcbDevice>(display, screen, xcb_connection, xcb_screen,
						   std::move(fd_wrapped));
}

Own<Device> createDevice(AsyncIoProvider &provider) {
	return createXcbDevice(provider);
}
} // namespace gin