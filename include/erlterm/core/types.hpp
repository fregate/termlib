#pragma once

#include <string>

namespace erlterm
{

struct tag_atom {};
struct tag_string {};

template <typename Tag>
struct tagged_string : std::string
{
	using tag = Tag;
};

using atom = tagged_string<tag_atom>;
constexpr atom operator""_atom(const char * str, std::size_t sz)
{
	// TODO check if valid atom
    return atom(std::string(str, sz));
}

using string = tagged_string<tag_string>;
constexpr string operator""_estr(const char * str, std::size_t sz)
{
    return string(std::string(str, sz));
}

} // namespace erlterm
