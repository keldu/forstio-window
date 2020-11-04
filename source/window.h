#pragma once

#include <kelgin/async.h>
#include <kelgin/common.h>

#include <string_view>
#include <variant>

#include "video_mode.h"

namespace gin {

class Window {
public:
	class Event {
	public:
		struct Resize {
			size_t width;
			size_t height;
		};
	};

	using VariantEvent = std::variant<Event::Resize>;

	virtual ~Window() = default;

	virtual void show() = 0;
	virtual void hide() = 0;

	virtual const VideoMode &videoMode() const = 0;
	virtual const std::string_view title() const = 0;

	virtual void resize(size_t width, size_t height) = 0;

	virtual Conveyor<VariantEvent> onEvent() = 0;
};
} // namespace gin
