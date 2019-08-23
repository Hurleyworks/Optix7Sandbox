// This header file was auto-generated by ClassMate++
// Created: 16 Sep 2017 5:37:16 pm
// Copyright (c) 2017, HurleyWorks

#pragma once

static const char* MeshOptionsTable[] =
{
	"CenterVertices",
	"NormalizeSize",
	"ReduceMesh",
	"RestOnGround",
	"LoadStrategy",
	"Invalid"
};

struct MeshOptions
{
	enum EMeshOptions
	{
		CenterVertices = 1 << 0,
		NormalizeSize = 1 << 1,
		ReduceMesh = 1 << 2,
		RestOnGround = 1 << 3,
		LoadStrategy = 1 << 4,
		Invalid = 1 << 5
	};

	union
	{
		EMeshOptions name;
		unsigned value;
	};

	MeshOptions(EMeshOptions name) : name(name) {}
	MeshOptions(unsigned value) : value(value) {}
	MeshOptions() : value(0) {}
	void clear() { value = 0; }
	operator EMeshOptions() const { return name; }
	unsigned operator|=(MeshOptions m) { return value |= m; }
	unsigned operator&=(MeshOptions m) { return value &= m; }
	unsigned operator^=(MeshOptions m) { return value ^= m; }

	std::string toString()
	{
		std::ostringstream ostr;
		ostr << "  MeshOptions";

		if (value & CenterVertices)
			ostr << "::CenterVertices";

		if (value & NormalizeSize)
			ostr << "::NormalizeSize:";

		if (value & ReduceMesh)
			ostr << "::ReduceMesh:";

		if (value & LoadStrategy)
			ostr << "::LoadStrategy:";

		if (value & Invalid)
			ostr << "::Invalid:";

		return ostr.str();
	}
};
