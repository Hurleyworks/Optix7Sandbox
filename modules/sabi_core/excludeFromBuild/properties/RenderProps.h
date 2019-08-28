#pragma once

static const char* RenderKeyTable[] =
{
	"BackgroundColor",
	"ResourceFolder",
	"CommonFolder",
	"ShowPerformanceGraph",
	"RenderError",
	"Invalid"
};

struct RenderKey
{
	enum ERenderKey
	{
		BackgroundColor,
		ResourceFolder,
		CommonFolder,
		ShowPerformanceGraph,
		RenderError,
		Count,
		Invalid = Count
	};

	union
	{
		ERenderKey name;
		unsigned int value;
	};

	RenderKey(ERenderKey name) : name(name) {}
	RenderKey(unsigned int value) : value(value) {}
	RenderKey() : value(Invalid) {}
	operator ERenderKey() const { return name; }
	const char* ToString() const { return RenderKeyTable[value]; }
	static RenderKey FromString(const char* str) { return mace::TableLookup(str, RenderKeyTable, Count); }
};

using RenderProperties = AnyValue<RenderKey>;
using RenderPropsRef = std::shared_ptr<RenderProperties>;
