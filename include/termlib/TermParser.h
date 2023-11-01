#pragma once

#include <cstdint>
#include <span>
#include <string>

#include <ei.h>

namespace termlib
{

class TermParser
{
public:
	using BlockData = unsigned char;
	using BinaryBlock = std::span<const BlockData>;

	struct Iterator
	{
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = std::pair<int, BinaryBlock>;
		using data_pointer = TermParser;

		Iterator(int index, BinaryBlock view);

		[[nodiscard]] data_pointer operator*();
		[[nodiscard]] value_type * operator->();
		[[nodiscard]] const value_type * operator->() const;

		Iterator & operator++();
		[[nodiscard]] Iterator operator++(int);

		[[nodiscard]] friend bool operator==(
			const termlib::TermParser::Iterator & lhs,
			const termlib::TermParser::Iterator & rhs)
		{
			if (lhs.index_ < 0 && rhs.index_ < 0)
				return true;

			// check empty list
			if (lhs.value_.first == ERL_NIL_EXT && rhs.index_ < 0)
				return true;

			return lhs.index_ == rhs.index_ && lhs.view_.data() == rhs.view_.data()
				&& lhs.view_.size() == rhs.view_.size();
		}

		[[nodiscard]] friend bool operator!=(
			const termlib::TermParser::Iterator & lhs,
			const termlib::TermParser::Iterator & rhs)
		{
			return !(operator==(lhs, rhs));
		}

	private:
		void fill_value();

	private:
		int index_;
		BinaryBlock view_;

		value_type value_;
	};

	explicit TermParser(const void * data, std::size_t size);
	explicit TermParser(BinaryBlock view);
	TermParser(TermParser & rhs);
	TermParser(TermParser && rhs);

	[[nodiscard]] const Iterator begin() const;
	[[nodiscard]] const Iterator end() const;
	[[nodiscard]] Iterator begin();
	[[nodiscard]] Iterator end();

private:
	BinaryBlock view_;
};

namespace parse
{

[[nodiscard]] std::string atom(const TermParser::Iterator & it);
[[nodiscard]] bool is_atom(const TermParser::Iterator & it);

[[nodiscard]] std::int32_t int32(const TermParser::Iterator & it);
[[nodiscard]] bool is_int32(const TermParser::Iterator & it);

[[nodiscard]] std::uint32_t uint32(const TermParser::Iterator & it);
[[nodiscard]] bool is_uint32(const TermParser::Iterator & it);

[[nodiscard]] std::int64_t int64(const TermParser::Iterator & it);
[[nodiscard]] bool is_int64(const TermParser::Iterator & it);

[[nodiscard]] std::uint64_t uint64(const TermParser::Iterator & it);
[[nodiscard]] bool is_uint64(const TermParser::Iterator & it);

[[nodiscard]] double real(const TermParser::Iterator & it);
[[nodiscard]] bool is_real(const TermParser::Iterator & it);

[[nodiscard]] std::string str(const TermParser::Iterator & it);
[[nodiscard]] std::string u8str(const TermParser::Iterator & it);
[[nodiscard]] bool is_str(const TermParser::Iterator & it);

[[nodiscard]] long binary(const TermParser::Iterator & it, void * dest, size_t size);
[[nodiscard]] bool is_binary(const TermParser::Iterator & it);

[[deprecated("Use tuple(), list() or map() instead.")]] [[nodiscard]] TermParser complex(
	const TermParser::Iterator & it);

[[nodiscard]] TermParser tuple(const TermParser::Iterator & it);
[[nodiscard]] TermParser list(const TermParser::Iterator & it);
[[nodiscard]] TermParser map(const TermParser::Iterator & it);
[[nodiscard]] bool is_complex(const TermParser::Iterator & it);
[[nodiscard]] bool is_tuple(const TermParser::Iterator & it);
[[nodiscard]] bool is_list(const TermParser::Iterator & it);
[[nodiscard]] bool is_map(const TermParser::Iterator & it);

} // namespace parse

} // namespace termlib
