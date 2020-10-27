#pragma once

#include <GL/glx.h>
#include <kelgin/common.h>

#include "gl/gl_window.h"

namespace gin {
class XcbWindow;
class XcbGlContext;
class XcbGlWindow final : public GlWindow {
public:
	Own<XcbWindow> window;
	XcbGlContext &context;

	::GLXWindow glx_window;

public:
	XcbGlWindow(Own<XcbWindow> &&, XcbGlContext &, ::GLXWindow);
	~XcbGlWindow();

	void bind() override;
	void swap() override;
	void show() override;
};
} // namespace gin