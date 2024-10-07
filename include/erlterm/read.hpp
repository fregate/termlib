#pragma once

#include <glaze/core/read.hpp>
#include <glaze/core/reflect.hpp>

#include <erlterm/ei/ei.hpp>

namespace erlterm
{

struct atom
{
	// disable relection
	atom() = default;

	using underlying = std::string;
	underlying value;
};

} // namespace erlterm

namespace glz
{

inline constexpr uint32_t ERLANG = 20000;

template <class T>
concept has_resize = requires(T t, size_t sz)
{
	t.resize(sz);
};

template <class T>
concept erl_string_t =
	detail::str_t<T> && !std::same_as<std::decay_t<T>, std::string_view> && has_resize<T> && has_data<T>;

template <typename T>
concept atom_t = std::same_as<std::decay_t<T>, erlterm::atom> && has_resize<typename T::underlying>
	&& has_data<typename T::underlying>;

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
	requires(not has_no_header(Opts)) GLZ_ALWAYS_INLINE
		static void op(T && value, Ctx && ctx, It0 && it, It1 && end) noexcept
	{
		// decode version
		erlterm::decode_version(std::forward<Ctx>(ctx), std::forward<It0>(it));
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

template <class T>
requires(reflectable<T>) struct from<ERLANG, T> final
{
	template <auto Opts>
	static void op(auto && value, is_context auto && ctx, auto && it, auto && end) noexcept
	{
		GLZ_END_CHECK();

		[[maybe_unused]] const auto tag = erlterm::get_type(ctx, it);
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

		for (size_t i = 0; i < 5; ++i)
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

				const auto n = 1; // erlterm::term_size(ctx, it, idx);
				if (bool(ctx.error)) [[unlikely]]
				{
					return;
				}

				if (it + n > end || it == end) [[unlikely]]
				{
					ctx.error = error_code::unexpected_end;
					return;
				}

				const auto index = decode_hash_with_size<ERLANG, T, HashInfo, HashInfo.type>::op(it, end, n);
				if (index < N) [[likely]]
				{
					if constexpr (is_partial_read<T> || Opts.partial_read)
					{
						fields[index] = true;
					}

					const sv key{it, n};
					it += n;

					jump_table<N>(
						[&]<size_t I>()
						{
							static constexpr auto TargetKey = get<I>(reflect<T>::keys);
							static constexpr auto Length = TargetKey.size();
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
					return;
			}
		}
	}
};

template <num_t T>
struct from<ERLANG, T> final
{
	template <auto Opts, is_context Ctx, class It0, class It1>
	static void op(auto && value, Ctx && ctx, It0 && it, It1 && end) noexcept
	{
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
	static void op(auto && value, Ctx && ctx, It0 && it, It1 && end) noexcept
	{
		erlterm::decode_string(
			std::forward<T>(value),
			std::forward<Ctx>(ctx),
			std::forward<It0>(it),
			std::forward<It1>(end));
	}
};

template <atom_t T>
struct from<ERLANG, T> final
{
	template <auto Opts, is_context Ctx, class It0, class It1>
	static void op(auto && value, Ctx && ctx, It0 && it, It1 && end) noexcept
	{
		erlterm::decode_atom(
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
	requires(has_no_header(Opts)) static void op(
		auto && /* value */,
		Tag && /* tag */,
		Ctx && /* ctx */,
		It0 && /* it */,
		It1 && /* end */) noexcept
	{
		std::cerr << "here\n";
	}

	template <auto Opts, is_context Ctx, class It0, class It1>
	requires(not has_no_header(
		Opts)) static void op(auto && /* value */, Ctx && /* ctx */, It0 && /* it */, It1 && /* end */) noexcept
	{
		std::cerr << "here\n";
	}
};

} // namespace detail

template <class T>
concept read_term_supported = requires
{
	detail::from<ERLANG, std::remove_cvref_t<T>>{};
};

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
