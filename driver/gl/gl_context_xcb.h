#pragma once

#include <GL/glx.h>

#include <set>
#include <string_view>

#include "forstio/window/gl/gl_context.h"

namespace saw {
struct GlxLibraryExtensions {
public:
	std::string_view raw_extension_string;

	GLXContext (*glXCreateContextAttribsARB)(Display *, GLXFBConfig, GLXContext,
											 Bool, const int *) = nullptr;
};

class XcbDevice;
class XcbGlContext final : public GlContext {
public:
	GlxLibraryExtensions ext_lib;
	Own<XcbDevice> device;
	int visual_id;
	GLXContext context;
	GLXFBConfig fb_config;

public:
	XcbGlContext(const GlxLibraryExtensions &, Own<XcbDevice> &&, int,
				 GLXContext, GLXFBConfig);
	~XcbGlContext();

	Own<GlWindow> createWindow(const VideoMode &, std::string_view) override;

	void flush() override;
};
} // namespace saw
