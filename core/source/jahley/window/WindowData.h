#pragma once

#include <mace_core/mace_core.h>

struct DesktopWindowSettings
{
	int width = DEFAULT_DESKTOP_WINDOW_WIDTH;
	int height = DEFAULT_DESKTOP_WINDOW_HEIGHT;
	std::string name = DEFAULT_DESKTOP_WINDOW_NAME;
	int refreshRate = DEFAULT_DESKTOP_WINDOW_REFRESH_RATE;
	bool resizable = DEFAULT_DESKTOP_WINDOW_RESIZABLE;
};