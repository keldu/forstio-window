#pragma once

#include <GL/glx.h>
#include <forstio/common.h>

#include "forstio/window/gl/gl_window.h"

namespace saw {
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
	void hide() override;

	const VideoMode &videoMode() const override;
	const std::string_view title() const override;

	void resize(size_t height, size_t width) override;

	Conveyor<Window::VariantEvent> onEvent() override;
};
} // namespace saw
