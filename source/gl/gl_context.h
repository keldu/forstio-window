#pragma once

#include <kelgin/common.h>

#include <string_view>

#include "gl_window.h"

namespace gin {
class GlSettings {
public:
	uint8_t gl_major = 3;
	uint8_t gl_minor = 3;

	enum class RenderType : int32_t { RGBA };

	RenderType render_type = RenderType::RGBA;

	bool renderable = true;

	// gl drawable
	bool window_type = true;

	// Pix and PBuffer are currently ignored.

	/// @hint don't change this unless you want to change the static cast in
	/// glcontext-xcb.cpp and other occurences
	/// Alternatively implement bitwise operations & and |
	enum class DrawableType : int32_t {
		WindowBit = 0x01,
		PixMapBit = 0x02,
		PBufferBit = 0x04
	};
	DrawableType drawable_type = DrawableType::WindowBit;

	bool double_buffer = true;

	int red_bits = 8;
	int green_bits = 8;
	int blue_bits = 8;
	int alpha_bits = 8;

	int depth_bits = 24;

	int stencil_bits = 8;

	bool core_profile = true;
};

class GlContext {
public:
	virtual ~GlContext() = default;

	virtual Own<GlWindow> createWindow(const VideoMode &, std::string_view) = 0;

	virtual void flush() = 0;
};

class AsyncIoProvider;
Own<GlContext> createGlContext(AsyncIoProvider &, const GlSettings &);
} // namespace gin
