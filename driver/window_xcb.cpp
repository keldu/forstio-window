#include "window_xcb.h"

#include <cassert>

#include "device_xcb.h"

namespace gin {
XcbWindow::XcbWindow(XcbDevice& device, xcb_window_t xcb_window,
                     xcb_colormap_t xcb_colormap, const VideoMode& video_mode,
                     std::string_view title_view)
    : device{device},
      xcb_window{xcb_window},
      xcb_colormap{xcb_colormap},
      video_mode{video_mode},
      window_title{title_view} {}

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

const VideoMode& XcbWindow::videoMode() const { return video_mode; }

const std::string_view XcbWindow::title() const { return window_title; }
}  // namespace gin
