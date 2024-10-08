#pragma once

#include <array>
#include <cstdint>

#include <ei.h>

namespace erlterm::detail
{

constexpr std::array<std::uint8_t, 4> atom_tags{
	ERL_ATOM_EXT,
	ERL_SMALL_ATOM_EXT,
	ERL_ATOM_UTF8_EXT,
	ERL_SMALL_ATOM_UTF8_EXT};

template <class Tag>
constexpr bool is_atom(Tag && tag)
{
	return atom_tags.find(tag) != atom_tags.end();
}

constexpr std::array<std::uint8_t, 4> integer_tags{
	ERL_INTEGER_EXT,
	ERL_SMALL_INTEGER_EXT,
	ERL_SMALL_BIG_EXT,
	ERL_LARGE_BIG_EXT};

constexpr std::array<std::uint8_t, 2> floating_point_tags{ERL_FLOAT_EXT, NEW_FLOAT_EXT};

constexpr std::array<std::uint8_t, 2> string_tags{ERL_STRING_EXT, ERL_NIL_EXT};

constexpr std::array<std::uint8_t, 1> binary_tags{ERL_BINARY_EXT};

constexpr std::array<std::uint8_t, 2> tuple_tags{ERL_SMALL_TUPLE_EXT, ERL_LARGE_TUPLE_EXT};

constexpr std::array<std::uint8_t, 3> list_tags{ERL_LIST_EXT, ERL_STRING_EXT, ERL_NIL_EXT};

constexpr std::array<std::uint8_t, 1> map_tags{ERL_MAP_EXT};

} // namespace erlterm::detail
