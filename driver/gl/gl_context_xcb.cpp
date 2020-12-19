#include "gl_context_xcb.h"

#include <GL/glx.h>

#include <cassert>
#include <cstdint>
#include <string_view>
#include <vector>

#include "../device_xcb.h"
#include "gl_window_xcb.h"

namespace gin {
namespace {
GlxLibraryExtensions glxLibraryExtensions(const char *extension_string) {
	std::string_view extensions_view{extension_string};
	std::set<std::string_view> extensions;
	while (1) {
		size_t n = extensions_view.find_first_of(' ');
		if (n != extensions_view.npos && n < extensions_view.size()) {
			std::string_view sub_glx_ext = extensions_view.substr(0, n);
			extensions.insert(sub_glx_ext);
			extensions_view.remove_prefix(n + 1);
		} else {
			break;
		}
	}

	auto find = extensions.find("GLX_ARB_create_context");
	GLXContext (*glXCreateContextAttribsARB)(Display *, GLXFBConfig, GLXContext,
											 Bool, const int *) = nullptr;
	if (find != extensions.end()) {
		glXCreateContextAttribsARB = reinterpret_cast<GLXContext (*)(
			Display *, GLXFBConfig, GLXContext, Bool, const int *)>(
			glXGetProcAddress(reinterpret_cast<const GLubyte *>(
				"glXCreateContextAttribsARB")));
	}
	return {extensions_view, glXCreateContextAttribsARB};
}
int translateRenderTypeSetting(GlSettings::RenderType cmp) {
	switch (cmp) {
	case GlSettings::RenderType::RGBA:
		return GLX_RGBA_BIT;
		break;
	}
	return 0;
}
int translateDrawableTypeSetting(GlSettings::DrawableType cmp) {
	int i = 0;
	if (static_cast<int32_t>(cmp) &
		static_cast<int32_t>(GlSettings::DrawableType::WindowBit)) {
		i |= static_cast<int32_t>(GLX_WINDOW_BIT);
	}
	if (static_cast<int32_t>(cmp) &
		static_cast<int32_t>(GlSettings::DrawableType::PixMapBit)) {
		i |= static_cast<int32_t>(GLX_PIXMAP_BIT);
	}
	if (static_cast<int32_t>(cmp) &
		static_cast<int32_t>(GlSettings::DrawableType::PBufferBit)) {
		i |= static_cast<int32_t>(GLX_PBUFFER_BIT);
	}
	return i;
}
} // namespace

XcbGlContext::XcbGlContext(const GlxLibraryExtensions &ext_lib,
						   Own<XcbDevice> &&dev, int visual_id,
						   GLXContext context, GLXFBConfig fb_config)
	: ext_lib{ext_lib}, device{std::move(dev)}, visual_id{visual_id},
	  context{context}, fb_config{fb_config} {}

XcbGlContext::~XcbGlContext() {
	assert(device);
	assert(device->display);
	if (context) {
		::glXMakeContextCurrent(device->display, None, None, nullptr);
		::glXDestroyContext(device->display, context);
	}
	device->flush();
}

void XcbGlContext::bind() {
	assert(device);
	if (device) {
		::glXMakeContextCurrent(device->display, None, None, context);
	}
}

Own<GlWindow> XcbGlContext::createWindow(const VideoMode &video_mode,
										 std::string_view title_view) {
	assert(device);
	if (!device) {
		return nullptr;
	}
	gin::Own<XcbWindow> window =
		device->createXcbWindow(video_mode, title_view, visual_id);
	if (!window) {
		return nullptr;
	}

	::GLXWindow glx_window = glXCreateWindow(device->display, fb_config,
											 window->xcb_window, nullptr);

	return gin::heap<XcbGlWindow>(std::move(window), *this, glx_window);
}

void XcbGlContext::flush() {
	assert(device);
	if (device) {
		device->flush();
	}
}

Own<GlContext> createGlContext(AsyncIoProvider &provider,
							   const GlSettings &settings) {
	Own<XcbDevice> device = createXcbDevice(provider);
	if (!device) {
		return nullptr;
	}

	/*
	 * Translate all attributes
	 */
	std::vector<int> attributes;
	attributes.reserve(33);

	attributes.push_back(GLX_X_RENDERABLE);
	attributes.push_back(settings.renderable ? True : False);

	attributes.push_back(GLX_RENDER_TYPE);
	attributes.push_back(translateRenderTypeSetting(settings.render_type));

	attributes.push_back(GLX_RED_SIZE);
	attributes.push_back(settings.red_bits);

	attributes.push_back(GLX_GREEN_SIZE);
	attributes.push_back(settings.green_bits);

	attributes.push_back(GLX_BLUE_SIZE);
	attributes.push_back(settings.blue_bits);

	attributes.push_back(GLX_ALPHA_SIZE);
	attributes.push_back(settings.alpha_bits);

	attributes.push_back(GLX_DEPTH_SIZE);
	attributes.push_back(settings.depth_bits);

	attributes.push_back(GLX_STENCIL_SIZE);
	attributes.push_back(settings.stencil_bits);

	attributes.push_back(GLX_DOUBLEBUFFER);
	attributes.push_back(settings.double_buffer ? True : False);

	attributes.push_back(GLX_DRAWABLE_TYPE);
	attributes.push_back(translateDrawableTypeSetting(settings.drawable_type));

	attributes.push_back(GLX_X_VISUAL_TYPE);
	attributes.push_back(GLX_TRUE_COLOR);

	attributes.push_back(None);

	int num_fb_configs = 0;

	GlxLibraryExtensions lib_ext = glxLibraryExtensions(
		glXQueryExtensionsString(device->display, device->screen));

	GLXFBConfig *fb_configs = glXChooseFBConfig(
		device->display, device->screen, &attributes[0], &num_fb_configs);
	if (!fb_configs || num_fb_configs == 0) {
		/// @todo log errors
		return nullptr;
	}

	if (lib_ext.glXCreateContextAttribsARB) {
		::GLXFBConfig fb_config = fb_configs[0];

		::GLXContext context;

		std::vector<int> glx_attribs;
		glx_attribs.reserve(11);

		glx_attribs.push_back(GLX_CONTEXT_MAJOR_VERSION_ARB);
		glx_attribs.push_back(settings.gl_major);
		glx_attribs.push_back(GLX_CONTEXT_MINOR_VERSION_ARB);
		glx_attribs.push_back(settings.gl_minor);
		glx_attribs.push_back(GLX_CONTEXT_PROFILE_MASK_ARB);
		glx_attribs.push_back(settings.core_profile
								  ? GLX_CONTEXT_CORE_PROFILE_BIT_ARB
								  : GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB);
		glx_attribs.push_back(None);

		context = lib_ext.glXCreateContextAttribsARB(
			device->display, fb_config, NULL, True, &glx_attribs[0]);
		::XFree(fb_configs);
		if (!context) {
			return nullptr;
		}

		int visual_id = 0;
		glXGetFBConfigAttrib(device->display, fb_config, GLX_VISUAL_ID,
							 &visual_id);
		return heap<XcbGlContext>(lib_ext, std::move(device), visual_id,
								  context, fb_config);
	}

	return nullptr;
}
} // namespace gin
