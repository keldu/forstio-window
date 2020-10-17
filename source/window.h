#pragma once

#include <string_view>

#include <kelgin/common.h>

#include "video_mode.h"

namespace gin {
class Window {
public:
	virtual ~Window() = default;

	virtual const VideoMode& videoMode() const = 0;
	virtual const std::string_view title() const = 0;
};
}
