#pragma once

static const char* WorldKeyTable[] =
{
	"TotalMeshes",
	"TotalInstances",
	"TotalVertices",
	"TotalRealTriangles",
	"TotalInstancedTriangles",
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
