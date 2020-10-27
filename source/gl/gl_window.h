#pragma once

#include "../video_mode.h"

namespace gin {
class GlWindow {
public:
	virtual ~GlWindow() = default;

	virtual void bind() = 0;
	virtual void show() = 0;
	virtual void swap() = 0;
};
} // namespace gin
