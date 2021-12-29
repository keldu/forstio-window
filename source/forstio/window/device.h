#pragma once

#include <forstio/async.h>
#include <forstio/common.h>

#include <string_view>
#include <variant>

#include "window.h"

namespace saw {
class Device {
public:
	virtual ~Device() = default;

	virtual Own<Window> createWindow(const VideoMode &mode,
									 std::string_view title_view) = 0;
	virtual void flush() = 0;
};

class IoProvider;
Own<Device> createDevice(IoProvider &provider);
} // namespace saw
