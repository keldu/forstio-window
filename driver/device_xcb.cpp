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
						 .sink([](const Error &error) { return error; })} {}

XcbDevice::~XcbDevice() {
	if (display) {
		xcb_flush(xcb_connection);
		::XCloseDisplay(display);
	}
}

void XcbDevice::windowDestroyed(xcb_window_t window_id) {
	windows.erase(window_id);
}

void XcbDevice::handleEvents() {
	while (xcb_generic_event_t *event = xcb_poll_for_event(xcb_connection)) {
		switch (event->response_type & ~0x80) {
		case XCB_EXPOSE: {
			xcb_expose_event_t *expose =
				reinterpret_cast<xcb_expose_event_t *>(event);
			auto find = windows.find(expose->window);
			if (find != windows.end()) {
				assert(find->second);
				find->second->resizeEvent(static_cast<size_t>(expose->x),
										  static_cast<size_t>(expose->y),
										  static_cast<size_t>(expose->width),
										  static_cast<size_t>(expose->height));
			}
		} break;
		case XCB_BUTTON_RELEASE: {
			xcb_button_release_event_t *button =
				reinterpret_cast<xcb_button_release_event_t *>(event);
			auto find = windows.find(button->event);
			if (find != windows.end()) {
				assert(find->second);
				find->second->mouseEvent(button->event_x, button->event_y,
										 button->state, false);
			}
		} break;
		case XCB_BUTTON_PRESS: {
			xcb_button_press_event_t *button =
				reinterpret_cast<xcb_button_press_event_t *>(event);
			auto find = windows.find(button->event);
			if (find != windows.end()) {
				assert(find->second);
				find->second->mouseEvent(button->event_x, button->event_y,
										 button->state, true);
			}
		} break;
		case XCB_KEY_RELEASE: {
			xcb_key_release_event_t *key =
				reinterpret_cast<xcb_key_release_event_t *>(event);
			auto find = windows.find(key->event);
			if (find != windows.end()) {
				assert(find->second);
				find->second->keyboardEvent(key->event_x, key->event_y,
											key->state, false);
			}
		} break;
		case XCB_KEY_PRESS: {
			xcb_key_press_event_t *key =
				reinterpret_cast<xcb_key_press_event_t *>(event);
			auto find = windows.find(key->event);
			if (find != windows.end()) {
				assert(find->second);
				find->second->keyboardEvent(key->event_x, key->event_y,
											key->state, true);
			}
		} break;
		default:
			break;
		}
		free(event);
	}
}

Own<XcbWindow> XcbDevice::createXcbWindow(const VideoMode &video_mode,
										  std::string_view title_view,
										  int visual_id) {
	assert(xcb_screen);
	assert(xcb_connection);

	xcb_colormap_t xcb_colormap = xcb_generate_id(xcb_connection);
	xcb_window_t xcb_window = xcb_generate_id(xcb_connection);

	xcb_create_colormap(xcb_connection, XCB_COLORMAP_ALLOC_NONE, xcb_colormap,
						xcb_screen->root, visual_id);

	uint32_t eventmask = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS |
						 XCB_EVENT_MASK_KEY_RELEASE |
						 XCB_EVENT_MASK_BUTTON_PRESS |
						 XCB_EVENT_MASK_BUTTON_RELEASE;
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
	auto window = heap<XcbWindow>(*this, xcb_window, xcb_colormap, video_mode,
								  title_view);
	windows[xcb_window] = window.get();

	return window;
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
