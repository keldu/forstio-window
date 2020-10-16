#pragma once

#include "window.h"

namespace gin {
class XcbWindow : public Window {
private:
	XcbDevice& device;

	xcb_window_t xcb_window;
	xcb_colormap_t xcb_colormap;

	VideoMode video_mode;
	std::string title;
public:
	XcbWindow(XcbDevice& device, xcb_window_t xcb_window, xcb_colormap_t xcb_colormap, const VideoMode& video_mode, std::string_view title_view);
	~XcbWindow();
};
}