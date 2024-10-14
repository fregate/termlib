#pragma once

#include <iostream>

#include <ei.h>

#include <glaze/concepts/container_concepts.hpp>
#include <glaze/core/common.hpp>
#include <glaze/core/context.hpp>

#include <erlterm/core/defs.hpp>
#include <erlterm/ei/types.hpp>

namespace erlterm
{

#define CHECK_OFFSET(off) \
	if ((it + (off)) > end) [[unlikely]] \
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

	CHECK_OFFSET(index);
	std::advance(it, index);
}

template <glz::output_buffer B, class IX>
[[nodiscard]] GLZ_ALWAYS_INLINE int resize_buffer(B && b, IX && ix, int index)
{
	if (b.size() < static_cast<std::size_t>(index))
	{
		b.resize(index);
	}

	return static_cast<int>(ix);
}

template <class F, glz::is_context Ctx, class B, class IX>
GLZ_ALWAYS_INLINE void encode_impl(F && func, Ctx && ctx, B && b, IX && ix)
{
	int index{static_cast<int>(ix)};
	if (func(nullptr, &index) < 0)
	{
		ctx.error = glz::error_code::seek_failure;
		return;
	}

	index = resize_buffer(b, ix, index);
	if (func(b.data(), &index) < 0)
	{
		ctx.error = glz::error_code::seek_failure;
		return;
	}

	ix = index;
}

} // namespace detail

template <class It>
void decode_version(glz::is_context auto && ctx, It && it)
{
	int index{};
	int version{};
	if (ei_decode_version(it, &index, &version) < 0) [[unlikely]]
	{
		ctx.error = glz::error_code::syntax_error;
		return;
	}

	std::advance(it, index);
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

void decode_atom(auto && value, glz::is_context auto && ctx, auto && it, auto && end)
{
	int index{};
	value.resize(MAXATOMLEN);
	if (ei_decode_atom(it, &index, value.data()) < 0) [[unlikely]]
	{
		ctx.error = glz::error_code::syntax_error;
		return;
	}

	value.shrink_to_fit();
	CHECK_OFFSET(index);
	std::advance(it, index);
}

template <class It0, class It1>
void decode_token(auto && value, glz::is_context auto && ctx, It0 && it, It1 && end)
{
	int index{};
	int type{};
	int sz{};
	if (ei_get_type(it, &index, &type, &sz) < 0) [[unlikely]]
	{
		ctx.error = glz::error_code::syntax_error;
		return;
	}

	if (is_atom(type))
	{
		decode_atom(value, ctx, it, end);
		return;
	}

	CHECK_OFFSET(sz);

	value.resize(sz);
	if (ei_decode_string(it, &index, value.data()) < 0) [[unlikely]]
	{
		ctx.error = glz::error_code::syntax_error;
		return;
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

	CHECK_OFFSET(index);
	value = v != 0;
	std::advance(it, index);
}

template <auto Opts, class T, class It0>
void decode_binary(T && value, std::size_t sz, glz::is_context auto && ctx, It0 && it, auto && end)
{
	CHECK_OFFSET(sz * sizeof(std::uint8_t));

	using V = glz::range_value_t<std::decay_t<T>>;

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
		if (sz > value.size())
		{
			ctx.error = glz::error_code::syntax_error;
			return;
		}
	}

	int index{};
	long szl{};
	if constexpr (sizeof(V) == sizeof(std::uint8_t))
	{
		if (ei_decode_binary(it, &index, value.data(), &szl) < 0) [[unlikely]]
		{
			ctx.error = glz::error_code::syntax_error;
			return;
		}
	}
	else
	{
		std::vector<std::uint8_t> buff(sz);
		if (ei_decode_binary(it, &index, buff.data(), &szl) < 0) [[unlikely]]
		{
			ctx.error = glz::error_code::syntax_error;
			return;
		}

		std::copy(buff.begin(), buff.end(), value.begin());
	}

	std::advance(it, index);
}

template <auto Opts, class T>
void decode_list(T && value, glz::is_context auto && ctx, auto && it, auto && end)
{
	using V = glz::range_value_t<std::decay_t<T>>;

	int index{};
	int arity{};
	if (ei_decode_list_header(it, &index, &arity) < 0) [[unlikely]]
	{
		ctx.error = glz::error_code::syntax_error;
		return;
	}

	CHECK_OFFSET(index);

	if constexpr (glz::resizable<T>)
	{
		value.resize(arity);
		if constexpr (Opts.shrink_to_fit)
		{
			value.shrink_to_fit();
		}
	}
	else
	{
		if (static_cast<std::size_t>(arity) > value.size())
		{
			ctx.error = glz::error_code::syntax_error;
			return;
		}
	}

	it += index;

	for (int idx = 0; idx < arity; idx++)
	{
		V v;
		glz::detail::from<ERLANG, V>::template op<Opts>(v, ctx, it, end);
		if (bool(ctx.error)) [[unlikely]]
		{
			return;
		}

		value[idx] = std::move(v);
	}

	// TODO handle elang list endings
}

template <auto Opts, class T, glz::is_context Ctx, class It0, class It1>
void decode_sequence(T && value, Ctx && ctx, It0 && it, It1 && end)
{
	int index{};
	int type{};
	int sz{};
	if (ei_get_type(it, &index, &type, &sz) < 0) [[unlikely]]
	{
		ctx.error = glz::error_code::syntax_error;
		return;
	}

	if (is_binary(type))
	{
		decode_binary<Opts>(
			std::forward<T>(value),
			static_cast<std::size_t>(sz),
			std::forward<Ctx>(ctx),
			std::forward<It0>(it),
			std::forward<It1>(end));
	}
	else if (is_list(type))
	{
		if (is_string(type))
		{
			std::string buff;
			decode_token(buff, std::forward<Ctx>(ctx), std::forward<It0>(it), std::forward<It1>(end));
			value.resize(buff.size());
			std::copy(buff.begin(), buff.end(), value.begin());
		}
		else
		{
			decode_list<Opts>(
				std::forward<T>(value),
				std::forward<Ctx>(ctx),
				std::forward<It0>(it),
				std::forward<It1>(end));
		}
	}
	// else if tuple?
	else
	{
		ctx.error = glz::error_code::elements_not_convertible_to_design;
	}
}

template <class It>
auto decode_map_header(glz::is_context auto && ctx, It && it)
{
	int arity{};
	int index{};
	if (ei_decode_map_header(it, &index, &arity) < 0) [[unlikely]]
	{
		ctx.error = glz::error_code::syntax_error;
		return std::pair<std::size_t, std::size_t>(-1ull, -1ull);
	}

	return std::pair<std::size_t, std::size_t>(static_cast<std::size_t>(arity), static_cast<std::size_t>(index));
}

template <class It>
auto decode_tuple_header(glz::is_context auto && ctx, It && it)
{
	int arity{};
	int index{};
	if (ei_decode_tuple_header(it, &index, &arity) < 0) [[unlikely]]
	{
		ctx.error = glz::error_code::syntax_error;
		return std::pair<std::size_t, std::size_t>(-1ull, -1ull);
	}

	return std::pair<std::size_t, std::size_t>(static_cast<std::size_t>(arity), static_cast<std::size_t>(index));
}

template <class B, class IX>
void encode_version(glz::is_context auto && ctx, B && b, IX && ix)
{
	int index{static_cast<int>(ix)};
	if (ei_encode_version(b.data(), &index) < 0) [[unlikely]]
	{
		ctx.error = glz::error_code::unexpected_end;
		return;
	}

	ix = index;
}

template <class... Args>
void encode_boolean(const bool value, Args &&... args)
{
	using namespace std::placeholders;
	detail::encode_impl(std::bind(ei_encode_boolean, _1, _2, value), std::forward<Args>(args)...);
}

template <class T, class... Args>
void encode_number(T && value, Args &&... args)
{
	using namespace std::placeholders;

	using V = std::remove_cvref_t<T>;
	if constexpr (detail::float_t<V>)
	{
		detail::encode_impl(std::bind(ei_encode_double, _1, _2, value), std::forward<Args>(args)...);
	}
	else if constexpr (sizeof(T) > sizeof(long))
	{
		if constexpr (std::is_signed_v<V>)
		{
			detail::encode_impl(std::bind(ei_encode_longlong, _1, _2, value), std::forward<Args>(args)...);
		}
		else
		{
			detail::encode_impl(std::bind(ei_encode_ulonglong, _1, _2, value), std::forward<Args>(args)...);
		}
	}
	else
	{
		if constexpr (std::is_signed_v<V>)
		{
			detail::encode_impl(std::bind(ei_encode_long, _1, _2, value), std::forward<Args>(args)...);
		}
		else
		{
			detail::encode_impl(std::bind(ei_encode_ulong, _1, _2, value), std::forward<Args>(args)...);
		}
	}
}

template <class... Args>
void encode_atom(auto && value, Args&&... args)
{
	using namespace std::placeholders;
	detail::encode_impl(std::bind(ei_encode_atom, _1, _2, value.data()), std::forward<Args>(args)...);
}

template <class... Args>
void encode_string(auto && value, Args&&... args)
{
	using namespace std::placeholders;
	detail::encode_impl(std::bind(ei_encode_string, _1, _2, value.data()), std::forward<Args>(args)...);
}

template <class... Args>
void encode_tuple_header(int arity, Args&&... args)
{
	using namespace std::placeholders;
	detail::encode_impl(std::bind(ei_encode_tuple_header, _1, _2, arity), std::forward<Args>(args)...);
}

} // namespace erlterm
