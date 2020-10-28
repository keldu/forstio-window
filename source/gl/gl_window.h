#pragma once

#include "../video_mode.h"

namespace gin {
class GlWindow {
public:
	virtual ~GlWindow() = default;

	/*
	* Bind the current window as the default framebuffer target (id 0)
	* This is different to glBindFramebuffer(framebuffer_id);
	*/
	virtual void bind() = 0;
	virtual void show() = 0;
	virtual void swap() = 0;
};
} // namespace gin
