#pragma once

#include <cstdint>

#include <erlterm/core/types.hpp>

namespace erlterm
{

inline constexpr std::uint32_t ERLANG = 20000;

} // namespace erlterm

namespace glz
{

template <class T>
concept string2_t =
	detail::str_t<T> && !std::same_as<std::decay_t<T>, std::string_view> && resizable<T> && has_data<T>;

template <class T>
concept atom_t = string2_t<T> && std::same_as<typename T::tag, erlterm::tag_atom>;

template <class T>
concept erl_str_t = string2_t<T> && std::same_as<typename T::tag, erlterm::tag_string>;


} // namespace glz
