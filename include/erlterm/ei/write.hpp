#pragma once

#include <glaze/core/context.hpp>
#include <erlterm/ei/ei.hpp>
#include <erlterm/core/defs.hpp>

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
		to<erlterm::ERLANG, std::remove_cvref_t<T>>::template op<Opts>(
			std::forward<T>(value),
			std::forward<Args>(args)...);
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
	GLZ_ALWAYS_INLINE static void op(auto && value, Args &&... args) noexcept
	{
		erlterm::encode_number(value, std::forward<Args>(args)...);
	}
};

template <atom_t T>
struct to<erlterm::ERLANG, T> final
{
	template <auto Opts, class... Args>
	GLZ_ALWAYS_INLINE static void op(auto && value, Args &&... args) noexcept
	{
		erlterm::encode_atom(value, std::forward<Args>(args)...);
	}
};

// using for write reflectable map keys
template <str_t T>
struct to<erlterm::ERLANG, T> final
{
	template <auto Opts, class... Args>
	GLZ_ALWAYS_INLINE static void op(auto && value, Args &&... args) noexcept
	{
		erlterm::encode_atom_len(value, value.size(), std::forward<Args>(args)...);
	}
};

template <erl_str_t T>
struct to<erlterm::ERLANG, T> final
{
	template <auto Opts, class... Args>
	GLZ_ALWAYS_INLINE static void op(auto && value, Args &&... args) noexcept
	{
		erlterm::encode_string(value, std::forward<Args>(args)...);
	}
};

template <class T>
requires(tuple_t<T> || is_std_tuple<T>)
struct to<erlterm::ERLANG, T> final
{
	template <auto Opts, is_context Ctx, class... Args>
	GLZ_ALWAYS_INLINE static void op(auto && value, Ctx && ctx, Args &&... args) noexcept
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

template <writable_array_t T>
struct to<erlterm::ERLANG, T> final
{
	template <auto Opts, is_context Ctx, class... Args>
	GLZ_ALWAYS_INLINE static void op(auto && value, Ctx && ctx, Args &&... args) noexcept
	{
		const auto n = value.size();
		erlterm::encode_list_header(n, ctx, std::forward<Args>(args)...);
		if (bool(ctx.error)) [[unlikely]]
		{
			return;
		}

		for (auto & i : value)
		{
			write<erlterm::ERLANG>::op<Opts>(i, ctx, args...);
		}

		erlterm::encode_list_tail(ctx, std::forward<Args>(args)...);
	}
};

template <writable_map_t T>
struct to<erlterm::ERLANG, T> final
{
	template <auto Opts, is_context Ctx, class... Args>
	GLZ_ALWAYS_INLINE static void op(T && value, Ctx && ctx, Args &&... args) noexcept
	{
		const auto n = value.size();
		erlterm::encode_map_header(n, ctx, std::forward<Args>(args)...);
		if (bool(ctx.error)) [[unlikely]]
		{
			return;
		}

		for (auto && [k, v] : value)
		{
			write<erlterm::ERLANG>::op<Opts>(k, ctx, args...);
			write<erlterm::ERLANG>::op<Opts>(v, ctx, args...);
		}
	}
};

template <reflectable T>
struct to<erlterm::ERLANG, T> final
{
	template <auto Opts, is_context Ctx, class... Args>
	GLZ_ALWAYS_INLINE static void op(T && value, Ctx && ctx, Args &&... args) noexcept
	{
		static constexpr auto N = reflect<T>::size;
		[[maybe_unused]] decltype(auto) t = [&]() -> decltype(auto)
		{
			if constexpr (reflectable<T>)
			{
				return to_tuple(value);
			}
			else
			{
				return nullptr;
			}
		}();

		erlterm::encode_map_header(N, ctx, std::forward<Args>(args)...);
		if (bool(ctx.error)) [[unlikely]]
		{
			return;
		}

		invoke_table<N>(
			[&]<size_t I>()
			{
				static constexpr sv key = reflect<T>::keys[I];
				write<erlterm::ERLANG>::op<Opts>(key, ctx, args...);

				decltype(auto) member = [&]() -> decltype(auto)
				{
					if constexpr (reflectable<T>)
					{
						return get<I>(t);
					}
					else
					{
						return get<I>(reflect<T>::values);
					}
				}();

				write<erlterm::ERLANG>::op<Opts>(get_member(value, member), ctx, args...);
			});
	}
};

template <class T>
struct to<erlterm::ERLANG, T> final
{
	// template <auto Opts, is_context Ctx, output_buffer B>
	// GLZ_ALWAYS_INLINE static void op(T && /* value */, Ctx && /* ctx */, B && /* b */) noexcept
	// {
	// 	// std::cerr << "here\n";
	// }
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
