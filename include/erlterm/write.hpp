#pragma once

#include <string>

#include <ei.h>

#include <glaze/core/context.hpp>
#include <glaze/core/write.hpp>

#include <erlterm/core/defs.hpp>
#include <erlterm/ei/ei.hpp>
// #include <erlterm/ei/write.hpp>

namespace glz
{

namespace detail
{

template <>
struct write<erlterm::ERLANG>
{
	template <auto Opts, class T, is_context Ctx, output_buffer B, class IX>
	GLZ_ALWAYS_INLINE static void op(T && value, Ctx && ctx, B && b, IX && ix) noexcept
	{
		ei_x_buff buff;
		ei_x_new_with_version(&buff);

		to<erlterm::ERLANG, std::remove_cvref_t<T>>::template op<Opts>(
			std::forward<T>(value),
			std::forward<Ctx>(ctx),
			buff);

		if (bool(ctx.error)) [[unlikely]]
		{
			return;
		}

		b.resize(buff.index);
		std::memcpy(b.data(), buff.buff, buff.index);
		ix = buff.index;
	}
};

template <boolean_like T>
struct to<erlterm::ERLANG, T>
{
	template <auto Opts, is_context Ctx, class B>
	GLZ_ALWAYS_INLINE static void op(const bool value, Ctx && ctx, B & b) noexcept
	{
		erlterm::encode_boolean(value, std::forward<Ctx>(ctx), b);
	}
};

template <num_t T>
struct to<erlterm::ERLANG, T> final
{
	template <auto Opts, is_context Ctx, class B>
	GLZ_ALWAYS_INLINE static void op(T && value, Ctx && ctx, B & b) noexcept
	{
		erlterm::encode_number(std::forward<T>(value), std::forward<Ctx>(ctx), b);
	}
};

template <class T>
struct to<erlterm::ERLANG, T> final
{
	template <auto Opts, is_context Ctx, output_buffer B>
	GLZ_ALWAYS_INLINE static void op(T && /* value */, Ctx && /* ctx */, B && /* b */) noexcept
	{
		// std::cerr << "here\n";
	}
};

} // namespace detail

template <class T>
concept write_term_supported = requires { detail::to<erlterm::ERLANG, std::remove_cvref_t<T>>{}; };

template <class T>
struct write_custom_format_supported<erlterm::ERLANG, T>
{
	static const auto value = write_term_supported<T>;
};

} // namespace glz

namespace erlterm
{

template <glz::write_term_supported T, glz::output_buffer Buffer>
[[nodiscard]] glz::error_ctx write_term(T && value, Buffer && buffer) noexcept
{
	return glz::write<glz::opts{.format = ERLANG}>(std::forward<T>(value), std::forward<Buffer>(buffer));
}

// template <glz::write_term_supported T, glz::raw_buffer Buffer>
// [[nodiscard]] glz::expected<size_t, glz::error_ctx> write_term(T && value, Buffer && buffer) noexcept
// {
// 	return glz::write<glz::opts{.format = ERLANG}>(std::forward<T>(value), std::forward<Buffer>(buffer));
// }

// template <glz::write_term_supported T>
// [[nodiscard]] glz::expected<std::string, glz::error_ctx> write_term(T && value) noexcept
// {
// 	return glz::write<glz::opts{.format = ERLANG}>(std::forward<T>(value));
// }

} // namespace erlterm
