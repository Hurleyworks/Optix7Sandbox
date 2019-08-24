// This header file was auto-generated by ClassMate++
// Created: 16 Sep 2017 5:37:16 pm
// Copyright (c) 2017, HurleyWorks

#pragma once

static const char* PRenderableStateTable[] =
{
	"Pickable",
	"Selected",
	"Deletable",
	"Invalid"
};

struct PRenderableState
{
	enum EPRenderableState
	{
		Pickable = 1 << 0,
		Selected = 1 << 1,
		Deletable = 1 << 2,
		Invalid = 1 << 3
	};

	union
	{
		EPRenderableState name;
		unsigned long value;
	};

	PRenderableState(EPRenderableState name) : name(name) {}
	PRenderableState(unsigned long value) : value(value) {}
	PRenderableState() : value(0) {}
	void clear() { value = 0; }
	operator EPRenderableState() const { return name; }
	unsigned long operator|=(PRenderableState m) { return value |= m; }
	unsigned long operator&=(PRenderableState m) { return value &= m; }
	unsigned long operator^=(PRenderableState m) { return value ^= m; }

	std::string toString()
	{
		std::ostringstream ostr;
		ostr << "  PRenderableState::";

		if (value & Pickable)
			ostr << "Pickable:";

		if (value & Selected)
			ostr << "Selected:";

		if (value & Deletable)
			ostr << "Deletable:";

		if (value & Invalid)
			ostr << "Invalid:";

		return ostr.str();
	}
};

struct RenderableState
{
	bool isSelected() const { return (state & PRenderableState::Selected) == PRenderableState::Selected; }
	bool isPickable() const { return (state & PRenderableState::Pickable) == PRenderableState::Pickable; }
	bool isDeletable() const { return (state & PRenderableState::Deletable) == PRenderableState::Deletable; }

	std::string toString() { return state.toString(); }

	PRenderableState state = 0;

}; // end class PhysicsBodyState
