#pragma once

static const char* MotionTypeTable[] =
{
	"None",
	"RandomJitter",
	"Reset",
	"Invalid"
};

struct MotionType
{
	enum EMotionType
	{
		None,
		RandomJitter,
		Reset,
		Count,
		Invalid = Count
	};

	union
	{
		EMotionType name;
		unsigned int value;
	};

	MotionType(EMotionType name) : name(name) {}
	MotionType(unsigned int value) : value(value) {}
	MotionType() : value(Invalid) {}
	operator EMotionType() const { return name; }
	const char* ToString() const { return MotionTypeTable[value]; }
	static MotionType FromString(const char* str) { return mace::TableLookup(str, MotionTypeTable, Count); }
};

static const char* WorldKeyTable[] =
{
	"TotalMeshes",
	"TotalInstances",
	"TotalVertices",
	"TotalRealTriangles",
	"TotalInstancedTriangles",
	"InstanceCount",
	"MotionType",
	"Invalid"
};

struct WorldKey
{
	enum EWorldKey
	{
		TotalMeshes,
		TotalInstances,
		TotalVertices,
		TotalRealTriangles,
		TotalInstancedTriangles,
		InstanceCount,
		MotionType,
		Count,
		Invalid = Count
	};

	union
	{
		EWorldKey name;
		unsigned int value;
	};

	WorldKey(EWorldKey name) : name(name) {}
	WorldKey(unsigned int value) : value(value) {}
	WorldKey() : value(Invalid) {}
	operator EWorldKey() const { return name; }
	const char* ToString() const { return WorldKeyTable[value]; }
	static WorldKey FromString(const char* str) { return mace::TableLookup(str, WorldKeyTable, Count); }
};

using WorldProperties = AnyValue<WorldKey>;
using WorldPropsRef = std::shared_ptr<WorldProperties>;

const uint64_t DEFAULT_MESH_COUNT = 0;
const uint64_t DEFAULT_TRIANGLE_COUNT = 0;
const uint64_t DEFAULT_INSTANCES_COUNT = 0;
const uint64_t DEFAULT_INSTANCED_TRIANGLE_COUNT = 0;
const MotionType DEFAULT_MOTION_TYPE = MotionType(MotionType::None);
