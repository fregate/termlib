#pragma once

#include <glaze/core/reflect.hpp>

#include <erlterm/core/defs.hpp>

namespace glz::detail
{

/* template <class T, auto HashInfo>
struct decode_hash<ERLANG, T, HashInfo, hash_type::unique_index>
{
	static constexpr auto N = reflect<T>::size;
	static constexpr auto bsize = bucket_size(hash_type::unique_index, N);
	static constexpr auto uindex = HashInfo.unique_index;

	GLZ_ALWAYS_INLINE static constexpr size_t op(auto && it, auto && end) noexcept
	{
         if constexpr (HashInfo.sized_hash) {
			erlterm::atom key;
			from<ERLANG>()
            // const auto* c = quote_memchr<HashInfo.min_length>(it, end);
            // if (c) [[likely]] {
            //    const auto n = size_t(static_cast<std::decay_t<decltype(it)>>(c) - it);
            //    if (n == 0 || n > HashInfo.max_length) {
            //       return N; // error
            //    }

            //    const auto h = bitmix(uint16_t(it[HashInfo.unique_index]) | (uint16_t(n) << 8), HashInfo.seed);
            //    return HashInfo.table[h % bsize];
            // }
            // else [[unlikely]] {
            //    return N;
            // }
         }
         else {
            if constexpr (N == 2) {
               if constexpr (uindex > 0) {
                  if ((it + uindex) >= end) [[unlikely]] {
                     return N; // error
                  }
               }
               // Avoids using a hash table
               if (std::is_constant_evaluated()) {
                  constexpr auto first_key_char = reflect<T>::keys[0][uindex];
                  return size_t(bool(it[uindex] ^ first_key_char));
               }
               else {
                  static constexpr auto first_key_char = reflect<T>::keys[0][uindex];
                  return size_t(bool(it[uindex] ^ first_key_char));
               }
            }
            else {
               if constexpr (uindex > 0) {
                  if ((it + uindex) >= end) [[unlikely]] {
                     return N; // error
                  }
               }
               return HashInfo.table[uint8_t(it[uindex])];
            }
         }
	}
};
 */
// template <class T, auto HashInfo>
// struct decode_hash_with_size<ERLANG, T, HashInfo, hash_type::unique_index>
// {
// 	static constexpr auto N = reflect<T>::size;
// 	static constexpr auto bsize = bucket_size(hash_type::unique_index, N);
// 	static constexpr auto uindex = HashInfo.unique_index;

// 	GLZ_ALWAYS_INLINE static constexpr size_t op(auto && it, auto && end, const size_t n) noexcept
// 	{
// 		return 0;
// 	}
// };

} // namespace glz::detail
