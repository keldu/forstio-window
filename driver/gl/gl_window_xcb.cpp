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

void XcbGlWindow::bind() {
	assert(window && context.device && context.device->display);
	if(window){
		if(context.device && context.device->display){
			::glXMakeContextCurrent(context.device->display, 
				glx_window, glx_window, context.context);
		}
	}
}

void XcbGlWindow::show() {
	assert(window);
	if (window) {
		window->show();
	}
}

void XcbGlWindow::hide(){
	assert(window);
	if(window){
		window->hide();
	}
}

void XcbGlWindow::swap() {
	assert(context.device);
	assert(context.device->display);
	if(context.device && context.device->display){
		glXSwapBuffers(context.device->display, glx_window);
	}
}

const VideoMode &XcbGlWindow::videoMode() const { 
	assert(window);
	if(window){
		return window->videoMode();
	}
	{
		static VideoMode mode_which_should_never_exist;
		return mode_which_should_never_exist;
	}
}

const std::string_view XcbGlWindow::title() const { 
	assert(window);
	if(window){
		return window->title();
	}
	return "Bad Window";
}

void XcbGlWindow::resize(size_t height, size_t width){
	assert(window);
	if(window){
		window->resize(height, width);
	}
}
} // namespace gin