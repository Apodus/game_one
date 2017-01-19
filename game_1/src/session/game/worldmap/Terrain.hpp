
#pragma once

// TODO: Names according to theme
enum class Terrain
{
	Fields = 1 << 0,
	Woods = 1 << 2,
	Hills = 1 << 3,
	Mountains = 1 << 4,
	Swamp = 1 << 5,
	Desert = 1 << 6
};

inline uint32_t& operator |= (uint32_t& a, Terrain b) {
	a |= static_cast<uint32_t>(b);
	return a;
}

inline Terrain operator | (Terrain a, Terrain b) {
	return static_cast<Terrain>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}