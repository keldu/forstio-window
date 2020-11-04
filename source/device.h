#pragma once

#include <kelgin/async.h>
#include <kelgin/common.h>

#include <string_view>
#include <variant>

#include "window.h"

namespace gin {
class Device {
public:
	virtual ~Device() = default;

	virtual Own<Window> createWindow(const VideoMode &mode,
									 std::string_view title_view) = 0;
	virtual void flush() = 0;
};

class AsyncIoProvider;
Own<Device> createDevice(AsyncIoProvider &provider);
} // namespace gin
