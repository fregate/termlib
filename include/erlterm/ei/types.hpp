#pragma once

#include <array>
#include <cstdint>

#include <ei.h>

#include <erlterm/utils/cmp.hpp>

namespace erlterm
{

template <typename Tag>
constexpr bool is_atom(const Tag & tag)
{
	return cmp::is<in, ERL_ATOM_EXT, ERL_SMALL_ATOM_EXT, ERL_ATOM_UTF8_EXT, ERL_SMALL_ATOM_UTF8_EXT>(tag);
}

template <typename Tag>
constexpr bool is_integer(const Tag & tag)
{
	return cmp::is<in, ERL_INTEGER_EXT, ERL_SMALL_INTEGER_EXT, ERL_SMALL_BIG_EXT, ERL_LARGE_BIG_EXT>(tag);
}

template <typename Tag>
constexpr bool is_floating_point(const Tag & tag)
{
	return cmp::is<in, ERL_FLOAT_EXT, NEW_FLOAT_EXT>(tag);
}

template <typename Tag>
constexpr bool is_string(const Tag & tag)
{
	return cmp::is<in, ERL_STRING_EXT, ERL_NIL_EXT>(tag);
}

template <typename Tag>
constexpr bool is_tuple(const Tag & tag)
{
	return cmp::is<in, ERL_SMALL_TUPLE_EXT, ERL_LARGE_TUPLE_EXT>(tag);
}

template <typename Tag>
constexpr bool is_list(const Tag & tag)
{
	return cmp::is<in, ERL_LIST_EXT, ERL_STRING_EXT, ERL_NIL_EXT>(tag);
}

template <typename Tag>
constexpr bool is_map(const Tag & tag)
{
	return cmp::is<in, ERL_MAP_EXT>(tag);
}

template <typename Tag>
constexpr bool is_binary(const Tag & tag)
{
	return cmp::is<in, ERL_BINARY_EXT>(tag);
}

} // namespace erlterm
