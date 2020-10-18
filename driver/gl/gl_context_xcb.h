#pragma once

#include "gl/gl_context.h"

namespace gin {
class XcbGlContext final : public GlContext {
public:
	void bind() override;
	Own<GlWindow> createWindow(const VideoMode&, std::string_view) override;

	void flush() override;
};
}
