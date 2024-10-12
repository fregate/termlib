#pragma once

#include <string>

namespace erlterm
{

struct tag_atom {};

template <typename Tag>
struct tagged_string : std::string
{
	using tag = Tag;
};

using atom = tagged_string<tag_atom>;

} // namespace erlterm
