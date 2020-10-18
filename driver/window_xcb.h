#pragma once

#include "window.h"

#include <string>

#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#include <xcb/xcb.h>

namespace gin {
class XcbDevice;
class XcbWindow final : public Window {
public:
	XcbDevice& device;

	xcb_window_t xcb_window;
	xcb_colormap_t xcb_colormap;

	VideoMode video_mode;
	std::string window_title;
public:
	XcbWindow(XcbDevice& device, xcb_window_t xcb_window, xcb_colormap_t xcb_colormap, const VideoMode& video_mode, std::string_view title_view);
	~XcbWindow();

	void show() override;
	void hide() override;

	const VideoMode& videoMode() const override;
	const std::string_view title() const override;
};
}