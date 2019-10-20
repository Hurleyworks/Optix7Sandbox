#pragma once

#include <mace_core/mace_core.h>

static const char* LayerTypeTable[] =
{
	"Nanogui",
	"Optix",
	"Invalid"
};

struct LayerType
{
	enum ELayerType
	{
		Nanogui,
		Optix,
		Count,
		Invalid = Count
	};

	union
	{
		ELayerType name;
		unsigned int value;
	};

	LayerType(ELayerType name) : name(name) {}
	LayerType(unsigned int value) : value(value) {}
	LayerType() : value(Invalid) {}
	operator ELayerType() const { return name; }
	const char* ToString() const { return LayerTypeTable[value]; }
	static LayerType FromString(const char* str) { return mace::TableLookup(str, LayerTypeTable, Count); }
};

struct DesktopWindowSettings
{
	int width = DEFAULT_DESKTOP_WINDOW_WIDTH;
	int height = DEFAULT_DESKTOP_WINDOW_HEIGHT;
	std::string name = DEFAULT_DESKTOP_WINDOW_NAME;
	int refreshRate = DEFAULT_DESKTOP_WINDOW_REFRESH_RATE;
	bool resizable = DEFAULT_DESKTOP_WINDOW_RESIZABLE;
};