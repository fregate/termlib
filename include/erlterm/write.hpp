#pragma once

#include <string>

#include <glaze/core/write.hpp>

#include <erlterm/core/defs.hpp>
#include <erlterm/ei/write.hpp>

namespace erlterm
{

template <glz::write_term_supported T, glz::output_buffer Buffer>
[[nodiscard]] glz::error_ctx write_term(T && value, Buffer && buffer) noexcept
{
	return glz::write<glz::opts{.format = ERLANG}>(std::forward<T>(value), std::forward<Buffer>(buffer));
}

template <glz::write_term_supported T, glz::raw_buffer Buffer>
[[nodiscard]] glz::expected<size_t, glz::error_ctx> write_term(T && value, Buffer && buffer) noexcept
{
	return glz::write<glz::opts{.format = ERLANG}>(std::forward<T>(value), std::forward<Buffer>(buffer));
}

template <glz::write_term_supported T>
[[nodiscard]] glz::expected<std::string, glz::error_ctx> write_term(T && value) noexcept
{
	return glz::write<glz::opts{.format = ERLANG}>(std::forward<T>(value));
}

} // namespace erlterm
