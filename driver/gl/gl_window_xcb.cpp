#include "gl_window_xcb.h"

#include "gl_context_xcb.h"

#include "../device_xcb.h"
#include "../window_xcb.h"

#include <cassert>

namespace gin {
XcbGlWindow::XcbGlWindow(Own<XcbWindow> &&win, XcbGlContext &ctx,
						 ::GLXWindow glx_win)
	: window{std::move(win)}, context{ctx}, glx_window{glx_win} {}

XcbGlWindow::~XcbGlWindow() {
	assert(context.device);
	if (context.device) {
		glXDestroyWindow(context.device->display, glx_window);
	}
}

void XcbGlWindow::bind() {}

void XcbGlWindow::show() {
	assert(window);
	if (window) {
		window->show();
	}
}

void XcbGlWindow::swap() {}
} // namespace gin