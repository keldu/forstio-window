#pragma once

#include "../window.h"
#include "../video_mode.h"

#include <kelgin/async.h>

#include <string_view>

namespace gin {
class GlWindow {
public:
	virtual ~GlWindow() = default;

	/*
	 * Bind the current window as the default framebuffer target (id 0)
	 * This is different to the call glBindFramebuffer(GLuint framebuffer_id);
	 */
	virtual void bind() = 0;
	virtual void show() = 0;
	virtual void hide() = 0;
	virtual void swap() = 0;

	virtual const VideoMode &videoMode() const = 0;
	virtual const std::string_view title() const = 0;

	virtual void resize(size_t height, size_t width) = 0;

	virtual Conveyor<Window::VariantEvent> onEvent() = 0;
};
} // namespace gin
