#pragma once

#include <glaze/core/read.hpp>

#include <erlterm/core/defs.hpp>
#include <erlterm/ei/read.hpp>

namespace erlterm
{

template <glz::read_term_supported T, class Buffer>
[[nodiscard]] inline glz::error_ctx read_term(T && value, Buffer && buffer) noexcept
{
	return glz::read<glz::opts{.format = ERLANG}>(value, std::forward<Buffer>(buffer));
}

template <glz::read_term_supported T, glz::is_buffer Buffer>
[[nodiscard]] glz::expected<T, glz::error_ctx> read_term(Buffer && buffer) noexcept
{
	T value{};
	glz::context ctx{};
	const glz::error_ctx ec = glz::read<glz::opts{.format = ERLANG}>(value, std::forward<Buffer>(buffer), ctx);
	if (ec)
	{
		return glz::unexpected<glz::error_ctx>(ec);
	}
	return value;
}

} // namespace erlterm
