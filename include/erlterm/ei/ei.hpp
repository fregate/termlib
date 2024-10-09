#pragma once

#include <iostream>

#include <ei.h>

#include <glaze/concepts/container_concepts.hpp>
#include <glaze/core/common.hpp>
#include <glaze/core/context.hpp>

#include <erlterm/ei/types.hpp>

namespace erlterm
{

#define CHECK_OFFSET() \
	if ((it + index) > end) [[unlikely]] \
	{ \
		ctx.error = glz::error_code::unexpected_end; \
		return; \
	}

namespace detail
{

template <class T>
concept float_t = std::floating_point<std::remove_cvref_t<T>>;

template <class F, glz::is_context Ctx, class It0, class It1>
void decode_number_impl(F && func, Ctx && ctx, It0 && it, It1 && end)
{
	int index{};
	if (func(&index) < 0) [[unlikely]]
	{
		ctx.error = glz::error_code::parse_number_failure;
		return;
	}

	CHECK_OFFSET();
	std::advance(it, index);
}

} // namespace detail

template <class It>
void decode_version(glz::is_context auto && ctx, It && it)
{
	int index{};
	int version{};
	if (ei_decode_version(it, &index, &version) < 0)
	{
		ctx.error = glz::error_code::syntax_error;
		return;
	}

	std::cerr << "term version: " << version << "\n";

	std::advance(it, index);
	return;
}

template <class It>
int get_type(glz::is_context auto && ctx, It && it)
{
	int type{};
	int size{};
	int index{};
	if (ei_get_type(it, &index, &type, &size) < 0)
	{
		ctx.error = glz::error_code::syntax_error;
		return -1;
	}

	return type;
}

template <class It>
auto skip_term(glz::is_context auto && ctx, It && it)
{
	int index{};
	if (ei_skip_term(it, &index) < 0)
	{
		ctx.error = glz::error_code::syntax_error;
		index = 0;
	}

	return it + index;
}

template <class It>
auto term_size(glz::is_context auto && ctx, It && it)
{
	int index{0};
	if (ei_skip_term(it, &index) < 0)
	{
		ctx.error = glz::error_code::syntax_error;
		index = 0;
	}

	return static_cast<std::size_t>(index);
}

template <class T, glz::is_context Ctx, class It0, class It1>
void decode_number(T && value, Ctx && ctx, It0 && it, It1 && end);

template <detail::float_t T, glz::is_context Ctx, class It0, class It1>
void decode_number(T && value, Ctx && ctx, It0 && it, It1 && end)
{
	using namespace std::placeholders;

	double v;
	detail::decode_number_impl(
		std::bind(ei_decode_double, it, _1, &v),
		std::forward<Ctx>(ctx),
		std::forward<It0>(it),
		std::forward<It1>(end));
	value = static_cast<std::remove_cvref_t<T>>(v);
}

template <class T, glz::is_context Ctx, class It0, class It1>
requires(glz::detail::num_t<T> && sizeof(T) > sizeof(long))
void decode_number(T && value, Ctx && ctx, It0 && it, It1 && end)
{
	using namespace std::placeholders;

	using V = std::remove_cvref_t<T>;
	if constexpr (std::is_signed_v<V>)
	{
		long long v;
		detail::decode_number_impl(
			std::bind(ei_decode_longlong, it, _1, &v),
			std::forward<Ctx>(ctx),
			std::forward<It0>(it),
			std::forward<It1>(end));
		value = static_cast<T>(v);
	}
	else
	{
		unsigned long long v;
		detail::decode_number_impl(
			std::bind(ei_decode_ulonglong, it, _1, &v),
			std::forward<Ctx>(ctx),
			std::forward<It0>(it),
			std::forward<It1>(end));
		value = static_cast<T>(v);
	}
}

template <glz::detail::num_t T, glz::is_context Ctx, class It0, class It1>
void decode_number(T && value, Ctx && ctx, It0 && it, It1 && end)
{
	using namespace std::placeholders;

	using V = std::remove_cvref_t<T>;
	if constexpr (std::is_signed_v<V>)
	{
		long v;
		detail::decode_number_impl(
			std::bind(ei_decode_long, it, _1, &v),
			std::forward<Ctx>(ctx),
			std::forward<It0>(it),
			std::forward<It1>(end));
		value = static_cast<T>(v);
	}
	else
	{
		unsigned long v;
		detail::decode_number_impl(
			std::bind(ei_decode_ulong, it, _1, &v),
			std::forward<Ctx>(ctx),
			std::forward<It0>(it),
			std::forward<It1>(end));
		value = static_cast<T>(v);
	}
}

template <class It0, class It1>
void decode_string(auto && value, glz::is_context auto && ctx, It0 && it, It1 && end)
{
	int index{};
	int type{};
	int sz{};
	if (ei_get_type(it, &index, &type, &sz) < 0)
	{
		ctx.error = glz::error_code::syntax_error;
		return;
	}

	if ((it + sz) > end) [[unlikely]]
	{
		ctx.error = glz::error_code::unexpected_end;
		return;
	}

	value.resize(sz);
	if (is_atom(type))
	{
		if (ei_decode_atom(it, &index, value.data()) < 0)
		{
			ctx.error = glz::error_code::syntax_error;
			return;
		}
	}
	else
	{
		if (ei_decode_string(it, &index, value.data()) < 0)
		{
			ctx.error = glz::error_code::syntax_error;
			return;
		}
	}

	std::advance(it, index);
}

template <glz::is_context Ctx, class It0, class It1>
void decode_boolean(auto && value, Ctx && ctx, It0 && it, It1 && end)
{
	int index{};
	int v{};
	if (ei_decode_boolean(it, &index, &v) < 0) [[unlikely]]
	{
		ctx.error = glz::error_code::parse_number_failure;
		return;
	}

	CHECK_OFFSET();
	value = v != 0;
	std::advance(it, index);
}

template <auto Opts, class T, class It0, class It1>
void decode_binary(T && value, glz::is_context auto && ctx, It0 && it, It1 && end)
{
	using V = glz::range_value_t<std::decay_t<T>>;

	int index{};
	int type{};
	int sz{};
	if (ei_get_type(it, &index, &type, &sz) < 0)
	{
		ctx.error = glz::error_code::syntax_error;
		return;
	}

	if ((it + sz) > end) [[unlikely]]
	{
		ctx.error = glz::error_code::unexpected_end;
		return;
	}

	if constexpr (glz::resizable<T>)
	{
		value.resize(sz);
		if constexpr (Opts.shrink_to_fit)
		{
			value.shrink_to_fit();
		}
	}
	else
	{
		if (static_cast<std::size_t>(sz) > value.size())
		{
			ctx.error = glz::error_code::syntax_error;
			return;
		}
	}

	long szl{};
	if constexpr (sizeof(V) == sizeof(std::uint8_t))
	{
		if (ei_decode_binary(it, &index, value.data(), &szl) < 0)
		{
			ctx.error = glz::error_code::syntax_error;
			return;
		}
	}
	else
	{
		std::vector<std::uint8_t> buff(sz);
		if (ei_decode_binary(it, &index, buff.data(), &szl) < 0)
		{
			ctx.error = glz::error_code::syntax_error;
			return;
		}

		std::copy(buff.begin(), buff.end(), value.begin());
	}

	std::advance(it, index);
}

template <class It>
auto decode_map_header(glz::is_context auto && ctx, It && it)
{
	int arity{};
	int index{};
	if (ei_decode_map_header(it, &index, &arity) < 0)
	{
		ctx.error = glz::error_code::syntax_error;
		return std::pair<std::size_t, std::size_t>(-1ull, -1ull);
	}

	return std::pair<std::size_t, std::size_t>(static_cast<std::size_t>(arity), static_cast<std::size_t>(index));
}

} // namespace erlterm
