#pragma once

static const char* ImageKeyTable[] =
{
	"RenderOutputFolder",
	"Invalid"
};

struct ImageKey
{
	enum EImageKey
	{
		RenderOutputFolder,
		Count,
		Invalid = Count
	};

	union
	{
		EImageKey name;
		unsigned int value;
	};

	ImageKey(EImageKey name) : name(name) {}
	ImageKey(unsigned int value) : value(value) {}
	ImageKey() : value(Invalid) {}
	operator EImageKey() const { return name; }
	const char* ToString() const { return ImageKeyTable[value]; }
	static ImageKey FromString(const char* str) { return mace::TableLookup(str, ImageKeyTable, Count); }
};

using ImagingProperties = AnyValue<ImageKey>;
using ImagingPropsRef = std::shared_ptr<ImagingProperties>;

const std::string DEFAULT_RENDER_OUTPUT_FOLDER = "C:/Users/micro/Desktop/output/animation";
const std::string DEFAULT_SCREEN_GRAB_FOLDER = "C:/Users/micro/Desktop/output/screen_grab";
const bool DEFAULT_CAPTURE_SCREEN = false;