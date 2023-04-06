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
	using BinaryBlock = std::span<const unsigned char>;

	struct Iterator
	{
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = std::pair<int, BinaryBlock>;
		using data_pointer = TermParser;

		Iterator(int index, BinaryBlock view);

		data_pointer operator*();
		value_type * operator->();
		const value_type * operator->() const;

		Iterator & operator++();
		Iterator operator++(int);

		bool operator==(const Iterator & rhs);
		bool operator!=(const Iterator & rhs);

	private:
		void fill_value();

	private:
		int index_;
		BinaryBlock view_;

		value_type value_;
	};

	TermParser(BinaryBlock view);

	const Iterator begin() const;
	const Iterator end() const;
	Iterator begin();
	Iterator end();

private:
	BinaryBlock view_;
};

namespace parse
{

std::string atom(const TermParser::Iterator & it);
std::int32_t int32(const TermParser::Iterator & it);
std::uint32_t uint32(const TermParser::Iterator & it);
std::int64_t int64(const TermParser::Iterator & it);
std::uint64_t uint64(const TermParser::Iterator & it);
double real(const TermParser::Iterator & it);
std::string str(const TermParser::Iterator & it);
long binary(const TermParser::Iterator & it, void * dest, size_t size);
TermParser complex(const TermParser::Iterator & it);

}  // namespace parse

}  // namespace termlib
