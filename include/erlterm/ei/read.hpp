#pragma once

#include <erlterm/core/defs.hpp>
#include <erlterm/ei/ei.hpp>

namespace glz
{

template <class T>
concept erl_string_t =
	detail::str_t<T> && !std::same_as<std::decay_t<T>, std::string_view> && resizable<T> && has_data<T>;

consteval bool has_format(opts o, std::uint32_t format)
{
	return o.format == format;
}

template <opts Opts, bool Padded = false>
requires(has_format(Opts, erlterm::ERLANG))
auto read_iterators(is_context auto && ctx, contiguous auto && buffer) noexcept
{
	auto [s, e] = detail::read_iterators_impl<Padded>(ctx, buffer);

	// decode version
	erlterm::decode_version(ctx, s);
	return std::pair{s, e};
}

namespace detail
{

// TODO skip value
template <>
struct skip_value<erlterm::ERLANG>
{
	template <auto Opts>
	inline static void op(is_context auto && /* ctx */, auto && /* it */, auto && /* end */) noexcept
	{
	}
};

template <>
struct read<erlterm::ERLANG>
{
	template <auto Opts, class T, is_context Ctx, class It0, class It1>
	requires(not has_no_header(Opts))
	GLZ_ALWAYS_INLINE static void op(T && value, Ctx && ctx, It0 && it, It1 && end) noexcept
	{
		if (bool(ctx.error))
		{
			return;
		}

		if constexpr (std::is_const_v<std::remove_reference_t<T>>)
		{
			if constexpr (Opts.error_on_const_read)
			{
				ctx.error = error_code::attempt_const_read;
			}
			else
			{
				// do not read anything into the const value
				skip_value<erlterm::ERLANG>::op<Opts>(std::forward<Ctx>(ctx), std::forward<It0>(it), std::forward<It1>(end));
			}
		}
		else
		{
			using V = std::remove_cvref_t<T>;
			from<erlterm::ERLANG, V>::template op<Opts>(
				std::forward<T>(value),
				std::forward<Ctx>(ctx),
				std::forward<It0>(it),
				std::forward<It1>(end));
		}
	}
};

template <readable_array_t T>
struct from<erlterm::ERLANG, T> final
{
	template <auto Opts, is_context Ctx, class It0, class It1>
	GLZ_ALWAYS_INLINE static void op(auto && value, Ctx && ctx, It0 && it, It1 && end) noexcept
	{
		GLZ_END_CHECK();
		erlterm::decode_sequence<Opts>(
			std::forward<T>(value),
			std::forward<Ctx>(ctx),
			std::forward<It0>(it),
			std::forward<It1>(end));
	}
};

template <boolean_like T>
struct from<erlterm::ERLANG, T>
{
	template <auto Opts, is_context Ctx, class It0, class It1>
	GLZ_ALWAYS_INLINE static void op(auto && value, Ctx && ctx, It0 && it, It1 && end) noexcept
	{
		GLZ_END_CHECK();
		erlterm::decode_boolean(
			std::forward<T>(value),
			std::forward<Ctx>(ctx),
			std::forward<It0>(it),
			std::forward<It1>(end));
	}
};

template <num_t T>
struct from<erlterm::ERLANG, T> final
{
	template <auto Opts, is_context Ctx, class It0, class It1>
	GLZ_ALWAYS_INLINE static void op(auto && value, Ctx && ctx, It0 && it, It1 && end) noexcept
	{
		GLZ_END_CHECK();

		erlterm::decode_number(
			std::forward<T>(value),
			std::forward<Ctx>(ctx),
			std::forward<It0>(it),
			std::forward<It1>(end));
	}
};

template <erl_string_t T>
struct from<erlterm::ERLANG, T> final
{
	template <auto Opts, is_context Ctx, class It0, class It1>
	GLZ_ALWAYS_INLINE static void op(auto && value, Ctx && ctx, It0 && it, It1 && end) noexcept
	{
		GLZ_END_CHECK();

		erlterm::decode_token(
			std::forward<T>(value),
			std::forward<Ctx>(ctx),
			std::forward<It0>(it),
			std::forward<It1>(end));
	}
};

template <class T>
struct from<erlterm::ERLANG, T> final
{
	template <auto Opts, class Tag, is_context Ctx, class It0, class It1>
	requires(has_no_header(Opts))
	static void op(auto && /* value */, Tag && /* tag */, Ctx && /* ctx */, It0 && /* it */, It1 && /* end */) noexcept
	{
		std::cerr << "here\n";
	}

	template <auto Opts, is_context Ctx, class It0, class It1>
	requires(not has_no_header(Opts))
	static void op(auto && /* value */, Ctx && /* ctx */, It0 && /* it */, It1 && /* end */) noexcept
	{
		std::cerr << "here\n";
	}
};

template <class T>
requires(tuple_t<T> || is_std_tuple<T>)
struct from<erlterm::ERLANG, T> final
{
	template <auto Opts>
	static void op(auto && value, is_context auto && ctx, auto && it, auto && end) noexcept
	{
		GLZ_END_CHECK();

		auto [fields_count, index] = erlterm::decode_tuple_header(ctx, it);
		if (bool(ctx.error)) [[unlikely]]
		{
			return;
		}

		if (it + index > end)
		{
			ctx.error = error_code::unexpected_end;
			return;
		}

		it += index;

		using V = std::decay_t<T>;
		constexpr auto N = glz::tuple_size_v<V>;

		if (fields_count != N)
		{
			ctx.error = error_code::syntax_error;
			return;
		}

		if constexpr (is_std_tuple<T>)
		{
			invoke_table<N>(
				[&]<size_t I>()
				{
					read<erlterm::ERLANG>::op<Opts>(std::get<I>(value), ctx, it, end);
				});
		}
		else
		{
			invoke_table<N>(
				[&]<size_t I>()
				{
					read<erlterm::ERLANG>::op<Opts>(glz::get<I>(value), ctx, it, end);
				});
		}
	}
};

template <class T>
requires(reflectable<T>)
struct from<erlterm::ERLANG, T> final
{
	template <auto Opts, is_context Ctx, class It0, class It1>
	static void op(auto && value, Ctx && ctx, It0 && it, It1 && end) noexcept
	{
		GLZ_END_CHECK();

		const auto tag = erlterm::get_type(ctx, it);
		if (bool(ctx.error)) [[unlikely]]
		{
			return;
		}

		static constexpr auto N = reflect<T>::size;

		std::size_t fields_count{0};
		if (erlterm::is_map(tag)) [[likely]]
		{
			[[maybe_unused]] auto [arity, idx] = erlterm::decode_map_header(std::forward<Ctx>(ctx), it);
			if (bool(ctx.error)) [[unlikely]]
			{
				return;
			}

			if (it + idx > end) [[unlikely]]
			{
				ctx.error = error_code::unexpected_end;
				return;
			}

			it += idx;
			fields_count = arity;
		}
		else if (erlterm::is_tuple(tag))
		{
			// parse tuple
		}
		else if (erlterm::is_list(tag))
		{
			// parse list
		}
		else [[unlikely]]
		{
			// error
			ctx.error = error_code::elements_not_convertible_to_design;
			return;
		}

		// empty term
		if (fields_count == 0)
		{
			return;
		}

		for (size_t i = 0; i < fields_count; ++i)
		{
			if constexpr (N > 0)
			{
				static constexpr auto HashInfo = hash_info<T>;

				// TODO this is only for erlmap + atom keys
				std::string mkey;
				from<erlterm::ERLANG, std::string>::op<Opts>(mkey, ctx, it, end);
				if (bool(ctx.error)) [[unlikely]]
				{
					return;
				}

				const auto n = mkey.size();
				const auto index = decode_hash_with_size<erlterm::ERLANG, T, HashInfo, HashInfo.type>::op(mkey.data(), end, n);
				if (index < N) [[likely]]
				{
					const sv key{mkey.data(), n};

					jump_table<N>(
						[&]<size_t I>()
						{
							static constexpr auto TargetKey = get<I>(reflect<T>::keys);
							static constexpr auto Length = TargetKey.size();
							if ((Length == n) && compare<Length>(TargetKey.data(), key.data())) [[likely]]
							{
								if constexpr (detail::reflectable<T>)
								{
									read<erlterm::ERLANG>::op<Opts>(
										get_member(value, get<I>(detail::to_tuple(value))),
										ctx,
										it,
										end);
								}
								else
								{
									read<erlterm::ERLANG>::op<Opts>(get_member(value, get<I>(reflect<T>::values)), ctx, it, end);
								}
							}
							else
							{
								if constexpr (Opts.error_on_unknown_keys)
								{
									ctx.error = error_code::unknown_key;
									return;
								}
								else
								{
									skip_value<erlterm::ERLANG>::op<Opts>(ctx, it, end);
									if (bool(ctx.error)) [[unlikely]]
										return;
								}
							}
						},
						index);

					if (bool(ctx.error)) [[unlikely]]
					{
						return;
					}
				}
				else [[unlikely]]
				{
					if constexpr (Opts.error_on_unknown_keys)
					{
						ctx.error = error_code::unknown_key;
						return;
					}
					else
					{
						it += n;
						skip_value<erlterm::ERLANG>::op<Opts>(ctx, it, end);
						if (bool(ctx.error)) [[unlikely]]
							return;
					}
				}
			}
			else if constexpr (Opts.error_on_unknown_keys)
			{
				ctx.error = error_code::unknown_key;
				return;
			}
			else
			{
				skip_value<erlterm::ERLANG>::op<Opts>(ctx, it, end);
				if (bool(ctx.error)) [[unlikely]]
				{
					return;
				}
			}
		}
	}
};

} // namespace detail

template <class T>
concept read_term_supported = requires { detail::from<erlterm::ERLANG, std::remove_cvref_t<T>>{}; };

template <class T>
struct is_custom_format_supported<erlterm::ERLANG, T>
{
	static const auto value = read_term_supported<T>;
};

} // namespace glz
