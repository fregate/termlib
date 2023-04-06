#pragma once

#include <functional>
#include <span>
#include <unordered_map>

#include <ei.h>

namespace termlib
{

class TermParser
{
public:
	using BinaryBlock = std::span<const char>;

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

	std::string atom(const Iterator & it) const;
	std::int32_t int32(const Iterator & it) const;
	std::uint32_t uint32(const Iterator & it) const;
	std::int64_t int64(const Iterator & it) const;
	std::uint64_t uint64(const Iterator & it) const;
	double real(const Iterator & it) const;
	std::string str(const Iterator & it) const;
	long binary(const Iterator & it, void * dest, size_t size) const;

	TermParser complex(const Iterator & it) const;

private:
	// std::unordered_map<char, std::function<int()>>
	BinaryBlock view_;

	mutable std::array<char, MAXATOMLEN> atom_buffer{0};
};

}  // namespace termlib
