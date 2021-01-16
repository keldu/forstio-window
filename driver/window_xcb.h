#pragma once

#include <X11/Xlib-xcb.h>
#include <X11/Xlib.h>
#include <xcb/xcb.h>

#include <string>

#include "window.h"

namespace gin {
class XcbDevice;
class XcbWindow final : public Window {
public:
	XcbDevice &device;

	xcb_window_t xcb_window;
	xcb_colormap_t xcb_colormap;

	VideoMode video_mode;
	std::string window_title;

	Own<ConveyorFeeder<Window::VariantEvent>> event_feeder = nullptr;

public:
	XcbWindow(XcbDevice &device, xcb_window_t xcb_window,
			  xcb_colormap_t xcb_colormap, const VideoMode &video_mode,
			  std::string_view title_view);
	~XcbWindow();

	void show() override;
	void hide() override;

	const VideoMode &videoMode() const override;
	const std::string_view title() const override;

	void resize(size_t width, size_t height) override;

	Conveyor<Window::VariantEvent> onEvent() override;

	void resizeEvent(size_t x, size_t y, size_t width, size_t height);
	void mouseEvent(int16_t x, int16_t y, uint16_t state, bool pressed);
	void mouseMoveEvent(int16_t x, int16_t y);
	void keyboardEvent(int16_t x, int16_t y, uint32_t keycode, bool pressed,
					   bool repeat);
};
} // namespace gin
