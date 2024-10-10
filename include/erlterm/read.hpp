#pragma once

#include <glaze/core/read.hpp>

#include <erlterm/core/defs.hpp>
#include <erlterm/ei/read.hpp>

namespace erlterm
{

template <glz::read_term_supported T, class Buffer>
[[nodiscard]] inline glz::error_ctx read_term(T && value, Buffer && buffer) noexcept
{
	return glz::read<glz::opts{.format = glz::ERLANG}>(value, std::forward<Buffer>(buffer));
}

} // namespace erlterm
