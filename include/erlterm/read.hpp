#pragma once

#include <glaze/core/read.hpp>
#include <glaze/core/reflect.hpp>

#include <erlterm/ei/ei.hpp>
#include <erlterm/core/defs.hpp>
#include <erlterm/core/reflect.hpp>

namespace glz
{

template <class T>
concept erl_string_t =
	detail::str_t<T> && !std::same_as<std::decay_t<T>, std::string_view> && resizable<T> && has_data<T>;


consteval bool has_format(opts o, std::uint32_t format) { return o.format == format; }

template <opts Opts, bool Padded = false>
requires(has_format(Opts, ERLANG))
auto read_iterators(is_context auto && ctx, contiguous auto && buffer) noexcept
{
	auto [s, e] = detail::read_iterators_impl<Padded>(ctx, buffer);

	// decode version
	erlterm::decode_version(ctx, s);
	return std::pair{s,e};
}

namespace detail
{

template <>
struct skip_value<ERLANG>
{
	template <auto Opts>
	inline static void op(is_context auto && /* ctx */, auto && /* it */, auto && /* end */) noexcept
	{
	}
};

template <>
struct read<ERLANG>
{
	// template <auto Opts, class T, class Tag, is_context Ctx, class It0, class It1>
	// requires(has_no_header(Opts))
	// GLZ_ALWAYS_INLINE static void op(T && value, Tag && tag, Ctx && ctx, It0 && it, It1 && end) noexcept
	// {
	// 	if constexpr (std::is_const_v<std::remove_reference_t<T>>) [[unlikely]]
	// 	{
	// 		if constexpr (Opts.error_on_const_read)
	// 		{
	// 			ctx.error = error_code::attempt_const_read;
	// 		}
	// 		else
	// 		{
	// 			// do not read anything into the const value
	// 			skip_value<ERLANG>::op<Opts>(std::forward<Ctx>(ctx), std::forward<It0>(it), std::forward<It1>(end));
	// 		}
	// 	}
	// 	else
	// 	{
	// 		using V = std::remove_cvref_t<T>;
	// 		from<ERLANG, V>::template op<no_header_on<Opts>>(
	// 			std::forward<T>(value),
	// 			std::forward<Tag>(tag),
	// 			std::forward<Ctx>(ctx),
	// 			std::forward<It0>(it),
	// 			std::forward<It1>(end));
	// 	}
	// }

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
				skip_value<ERLANG>::op<Opts>(std::forward<Ctx>(ctx), std::forward<It0>(it), std::forward<It1>(end));
			}
		}
		else
		{
			using V = std::remove_cvref_t<T>;
			from<ERLANG, V>::template op<Opts>(
				std::forward<T>(value),
				std::forward<Ctx>(ctx),
				std::forward<It0>(it),
				std::forward<It1>(end));
		}
	}
};

template <readable_array_t T>
struct from<ERLANG, T> final
{
	template <auto Opts, is_context Ctx, class It0, class It1>
	GLZ_ALWAYS_INLINE static void op(auto && value, Ctx && ctx, It0 && it, It1 && end) noexcept
	{
		GLZ_END_CHECK();
		erlterm::decode_binary<Opts>(
			std::forward<T>(value),
			std::forward<Ctx>(ctx),
			std::forward<It0>(it),
			std::forward<It1>(end));
	}
};

template <boolean_like T>
struct from<ERLANG, T>
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
struct from<ERLANG, T> final
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
struct from<ERLANG, T> final
{
	template <auto Opts, is_context Ctx, class It0, class It1>
	GLZ_ALWAYS_INLINE static void op(auto && value, Ctx && ctx, It0 && it, It1 && end) noexcept
	{
		GLZ_END_CHECK();

		erlterm::decode_string(
			std::forward<T>(value),
			std::forward<Ctx>(ctx),
			std::forward<It0>(it),
			std::forward<It1>(end));
	}
};

template <class T>
struct from<ERLANG, T> final
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
requires(reflectable<T>)
struct from<ERLANG, T> final
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
		static constexpr bit_array<N> all_fields = []
		{
			bit_array<N> arr{};
			for (size_t i = 0; i < N; ++i)
			{
				arr[i] = true;
			}
			return arr;
		}();

		decltype(auto) fields = [&]() -> decltype(auto)
		{
			if constexpr (is_partial_read<T> || Opts.partial_read)
			{
				return bit_array<N>{};
			}
			else
			{
				return nullptr;
			}
		}();

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
			if constexpr (is_partial_read<T> || Opts.partial_read)
			{
				if ((all_fields & fields) == all_fields)
				{
					return;
				}
			}

			if constexpr (N > 0)
			{
				static constexpr auto HashInfo = hash_info<T>;

				// TODO this is only for erlmap + atom keys
				std::string mkey;
				from<ERLANG, std::string>::op<Opts>(mkey, ctx, it, end);
				if (bool(ctx.error)) [[unlikely]]
				{
					return;
				}

				// for (size_t idx = 0; idx < HashInfo.table.size(); idx++)
				// {
				// 	if (HashInfo.table[idx] == N)
				// 		continue;

				// 	std::cerr << "idx: " << idx << ", V:" << static_cast<int>(HashInfo.table[idx]) << "\n";
				// }

				// const auto refkeys = ;
				for (const auto & k: reflect<T>::keys)
				{
					std::cerr << "k: " << k.data() << ", sz: " << k.size() << "\n";
				}

				const auto n = mkey.size();
				const auto index =
					decode_hash_with_size<ERLANG, T, HashInfo, HashInfo.type>::op(mkey.data(), end, n);
				if (index < N) [[likely]]
				{
					if constexpr (is_partial_read<T> || Opts.partial_read)
					{
						fields[index] = true;
					}

					const sv key{mkey.data(), n};

					jump_table<N>(
						[&]<size_t I>()
						{
							static constexpr auto TargetKey = get<I>(reflect<T>::keys);
							static constexpr auto Length = TargetKey.size();
							std::cerr << "TK: " << TargetKey << ", L: " << Length << "\n";
							if ((Length == n) && compare<Length>(TargetKey.data(), key.data())) [[likely]]
							{
								if constexpr (detail::reflectable<T>)
								{
									read<ERLANG>::op<Opts>(
										get_member(value, get<I>(detail::to_tuple(value))),
										ctx,
										it,
										end);
								}
								else
								{
									read<ERLANG>::op<Opts>(get_member(value, get<I>(reflect<T>::values)), ctx, it, end);
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
									skip_value<ERLANG>::op<Opts>(ctx, it, end);
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
						skip_value<ERLANG>::op<Opts>(ctx, it, end);
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
				skip_value<ERLANG>::op<Opts>(ctx, it, end);
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
concept read_term_supported = requires { detail::from<ERLANG, std::remove_cvref_t<T>>{}; };

template <class T>
struct is_custom_format_supported<ERLANG, T>
{
	static const auto value = read_term_supported<T>;
};

} // namespace glz

namespace erlterm
{

template <glz::read_term_supported T, class Buffer>
[[nodiscard]] inline glz::error_ctx read_term(T && value, Buffer && buffer) noexcept
{
	using namespace glz;
	return read<opts{.format = ERLANG}>(value, std::forward<Buffer>(buffer));
}

} // namespace erlterm
