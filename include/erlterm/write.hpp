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
	//  class T, is_context Ctx, output_buffer B, class IX
	template <auto Opts, class T, class... Args>
	requires(has_no_header(Opts))
	GLZ_ALWAYS_INLINE static void op(T && value, Args &&... args) noexcept
	{
		using V = std::remove_cvref_t<T>;
		to<erlterm::ERLANG, V>::template op<Opts>(std::forward<V>(value), std::forward<Args>(args)...);
	}

	template <auto Opts, class T, is_context Ctx, output_buffer B, class IX>
	requires(not has_no_header(Opts))
	GLZ_ALWAYS_INLINE static void op(T && value, Ctx && ctx, B && b, IX && ix) noexcept
	{
		erlterm::encode_version(ctx, b, ix);
		if (bool(ctx.error)) [[unlikely]]
		{
			return;
		}

		using V = std::remove_cvref_t<T>;
		to<erlterm::ERLANG, V>::template op<no_header_on<Opts>()>(
			std::forward<V>(value),
			std::forward<Ctx>(ctx),
			std::forward<B>(b),
			std::forward<IX>(ix));
	}
};

template <boolean_like T>
struct to<erlterm::ERLANG, T>
{
	template <auto Opts, class... Args>
	GLZ_ALWAYS_INLINE static void op(const bool value, Args &&... args) noexcept
	{
		erlterm::encode_boolean(value, std::forward<Args>(args)...);
	}
};

template <num_t T>
struct to<erlterm::ERLANG, T> final
{
	template <auto Opts, class... Args>
	GLZ_ALWAYS_INLINE static void op(T && value, Args &&... args) noexcept
	{
		erlterm::encode_number(value, std::forward<Args>(args)...);
	}
};

template <atom_t T>
struct to<erlterm::ERLANG, T> final
{
	template <auto Opts, class... Args>
	GLZ_ALWAYS_INLINE static void op(T && value, Args &&... args) noexcept
	{
		erlterm::encode_atom(std::forward<T>(value), std::forward<Args>(args)...);
	}
};

template <erl_str_t T>
struct to<erlterm::ERLANG, T> final
{
	template <auto Opts, class... Args>
	GLZ_ALWAYS_INLINE static void op(T && value, Args &&... args) noexcept
	{
		erlterm::encode_string(std::forward<T>(value), std::forward<Args>(args)...);
	}
};

template <class T>
requires(tuple_t<T> || is_std_tuple<T>)
struct to<erlterm::ERLANG, T> final
{
	template <auto Opts, is_context Ctx, class... Args>
	GLZ_ALWAYS_INLINE static void op(T && value, Ctx && ctx, Args &&... args) noexcept
	{
		static constexpr auto N = glz::tuple_size_v<T>;

		erlterm::encode_tuple_header(N, ctx, std::forward<Args>(args)...);
		if (bool(ctx.error)) [[unlikely]]
		{
			return;
		}

		if constexpr (is_std_tuple<T>)
		{
			[&]<size_t... I>(std::index_sequence<I...>)
			{
				(write<erlterm::ERLANG>::op<Opts>(std::get<I>(value), ctx, args...), ...);
			}(std::make_index_sequence<N>{});
		}
		else
		{
			[&]<size_t... I>(std::index_sequence<I...>)
			{
				(write<erlterm::ERLANG>::op<Opts>(glz::get<I>(value), ctx, args...), ...);
			}(std::make_index_sequence<N>{});
		}
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
