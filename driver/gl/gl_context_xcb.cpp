#include "gl_context_xcb.h"

namespace gin {
void XcbGlContext::bind(){

}

Own<GlWindow> XcbGlContext::createWindow(const VideoMode&, std::string_view){
	return nullptr;
}

void XcbGlContext::flush(){

}
}
