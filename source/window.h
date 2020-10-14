#pragma once

#include <kelgin/common.h>

namespace gin {
class Window {
public:
	virtual ~Window() = default;
};

class Device {
public:
	virtual ~Device() = default;

	virtual Own<Window> createWindow();
};

Own<Device> createDevice();
}