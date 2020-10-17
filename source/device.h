#pragma once

#include <kelgin/common.h>

#include "window.h"

namespace gin {
class Device {
public:
	virtual ~Device() = default;

	virtual Own<Window> createWindow() = 0;
	virtual void flush() = 0;
};

Own<Device> createDevice();
}
