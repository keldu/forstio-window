#include "window_xcb.h"

#include <cassert>

#include "device_xcb.h"

namespace gin {
XcbWindow::XcbWindow(XcbDevice &device, xcb_window_t xcb_window,
					 xcb_colormap_t xcb_colormap, const VideoMode &video_mode,
					 std::string_view title_view)
	: device{device}, xcb_window{xcb_window}, xcb_colormap{xcb_colormap},
	  video_mode{video_mode}, window_title{title_view} {}

XcbWindow::~XcbWindow() {
	device.windowDestroyed(xcb_window);
	xcb_destroy_window(device.xcb_connection, xcb_window);
	device.flush();
}

void XcbWindow::show() {
	assert(device.xcb_connection);
	xcb_map_window(device.xcb_connection, xcb_window);
}

void XcbWindow::hide() {
	assert(device.xcb_connection);
	xcb_unmap_window(device.xcb_connection, xcb_window);
}

const VideoMode &XcbWindow::videoMode() const { return video_mode; }

const std::string_view XcbWindow::title() const { return window_title; }

void XcbWindow::resize(size_t width, size_t height) {
	const uint32_t values[2] = {static_cast<uint32_t>(width),
								static_cast<uint32_t>(height)};

	xcb_configure_window(device.xcb_connection, xcb_window,
						 XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT,
						 values);
	video_mode.width = width;
	video_mode.height = height;
}

Conveyor<Window::VariantEvent> XcbWindow::onEvent() {
	auto caf = newConveyorAndFeeder<Window::VariantEvent>();
	event_feeder = std::move(caf.feeder);
	return std::move(caf.conveyor);
}

void XcbWindow::resizeEvent(size_t x, size_t y, size_t width, size_t height) {
	(void)x;
	(void)y;
	/// @todo maybe include x and y?
	video_mode.width = width;
	video_mode.height = height;

	if (event_feeder) {
		event_feeder->feed(
			Window::VariantEvent{Window::Event::Resize{width, height}});
	}
}

void XcbWindow::mouseEvent(int16_t x, int16_t y, uint16_t state, bool pressed) {
	if (x < 0 || y < 0) {
		return;
	}
	uint32_t ux = static_cast<uint32_t>(x);
	uint32_t uy = static_cast<uint32_t>(y);
	if (ux >= video_mode.width || uy >= video_mode.height) {
		return;
	}
	if (event_feeder) {
		event_feeder->feed(
			Window::VariantEvent{Window::Event::Mouse{state, pressed, ux, uy}});
	}
}

void XcbWindow::mouseMoveEvent(int16_t x, int16_t y, uint16_t state,
							   uint8_t move_type) {
	if (x < 0 || y < 0) {
		return;
	}
	uint32_t ux = static_cast<uint32_t>(x);
	uint32_t uy = static_cast<uint32_t>(y);
	if (ux >= video_mode.width || uy >= video_mode.height) {
		return;
	}
	if (event_feeder) {
		event_feeder->feed(Window::VariantEvent{
			Window::Event::Mouse{state, move_type > 0 ? true : false, ux, uy}});
	}
}

void XcbWindow::keyboardEvent(int16_t x, int16_t y, uint32_t keycode,
							  bool pressed, bool repeat) {
	if (x < 0 || y < 0) {
		return;
	}
	uint32_t ux = static_cast<uint32_t>(x);
	uint32_t uy = static_cast<uint32_t>(y);
	if (ux >= video_mode.width || uy >= video_mode.height) {
		return;
	}
	if (event_feeder) {
		event_feeder->feed(Window::VariantEvent{
			Window::Event::Keyboard{keycode, keycode, pressed, repeat}});
	}
}

} // namespace gin
