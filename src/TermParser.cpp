#include <limits>
#include <stdexcept>
#include <string>

#include <termlib/TermParser.h>

#include "cmp.h"

namespace termlib
{

TermParser::Iterator::Iterator(int index, BinaryBlock view)
	: index_{index}
	, view_{view}
{
	fill_value();
}

void TermParser::Iterator::fill_value()
{
	if (index_ < 0)
		return;

	int type;
	int size = 0;
	ei_get_type(view_.data(), &index_, &type, &size);
	int idx{index_};
	ei_skip_term(view_.data(), &idx);
	value_ = std::make_pair(type, view_.subspan(index_, idx - index_));
}

TermParser::Iterator::data_pointer TermParser::Iterator::operator*()
{
	return TermParser(value_.second);
}

TermParser::Iterator::value_type * TermParser::Iterator::operator->()
{
	return &value_;
}

const TermParser::Iterator::value_type * TermParser::Iterator::operator->() const
{
	return &value_;
}

TermParser::Iterator & TermParser::Iterator::operator++()
{
	if (index_ < 0)
		return *this;

	auto res = ei_skip_term(view_.data(), &index_);
	if (res < 0 || index_ >= view_.size()) {
		index_ = std::numeric_limits<int>::min();
		return *this;
	}

	fill_value();
	if (value_.first == ERL_NIL_EXT) {
		index_ = std::numeric_limits<int>::min();
	}
	return *this;
}

TermParser::Iterator TermParser::Iterator::operator++(int)
{
	auto tmp = *this;
	++(*this);
	return tmp;
}

bool TermParser::Iterator::operator==(const Iterator & rhs)
{
	if (index_ < 0 && rhs.index_ < 0)
		return true;

	// check empty list
	if (value_.first == ERL_NIL_EXT && rhs.index_ < 0)
		return true;

	return index_ == rhs.index_ && view_.data() == view_.data() && view_.size() == view_.size();
}

bool TermParser::Iterator::operator!=(const Iterator & rhs)
{
	return !(operator==(rhs));
}

// TermParser

TermParser::TermParser(BinaryBlock view)
	: view_{view}
{
	int index{0};
	int version;
	if (ei_decode_version(view_.data(), &index, &version) == 0) {
		view_ = view_.subspan(index);
	}
}

const TermParser::Iterator TermParser::begin() const
{
	// check if no data (empty tuple, empty map)
	if (view_.empty())
		return end();

	return Iterator(0, view_);
}

const TermParser::Iterator TermParser::end() const
{
	return Iterator(std::numeric_limits<int>::min(), view_);
}

TermParser::Iterator TermParser::begin()
{
	// check if no data (empty tuple, empty map)
	if (view_.empty())
		return end();

	return Iterator(0, view_);
}

TermParser::Iterator TermParser::end()
{
	return Iterator(std::numeric_limits<int>::min(), view_);
}

std::string TermParser::atom(const Iterator & it) const
{
	if (!is(it->first, in{ERL_ATOM_EXT, ERL_SMALL_ATOM_EXT, ERL_ATOM_UTF8_EXT, ERL_SMALL_ATOM_UTF8_EXT}))
		throw std::runtime_error("atom type error: " + std::to_string(it->first));

	int idx{0};
	ei_decode_atom(it->second.data(), &idx, &atom_buffer.front());
	return std::string(&atom_buffer.front());
}

std::int32_t TermParser::int32(const Iterator & it) const
{
	if (!is(it->first, in{ERL_INTEGER_EXT, ERL_SMALL_INTEGER_EXT, ERL_SMALL_BIG_EXT, ERL_LARGE_BIG_EXT}))
		throw std::runtime_error("int32 type error: " + std::to_string(it->first));

	int idx{0};
	long val;
	ei_decode_long(it->second.data(), &idx, &val);
	return static_cast<std::int32_t>(val);
}

std::uint32_t TermParser::uint32(const Iterator & it) const
{
	if (!is(it->first, in{ERL_INTEGER_EXT, ERL_SMALL_INTEGER_EXT, ERL_SMALL_BIG_EXT, ERL_LARGE_BIG_EXT}))
		throw std::runtime_error("uint32 type error: {}" + std::to_string(it->first));

	int idx{0};
	unsigned long val;
	ei_decode_ulong(it->second.data(), &idx, &val);
	return static_cast<std::uint32_t>(val);
}

std::int64_t TermParser::int64(const Iterator & it) const
{
	if (!is(it->first, in{ERL_INTEGER_EXT, ERL_SMALL_INTEGER_EXT, ERL_SMALL_BIG_EXT, ERL_LARGE_BIG_EXT}))
		throw std::runtime_error("int64 type error: " + std::to_string(it->first));

	int idx{0};
	long long val;
	ei_decode_longlong(it->second.data(), &idx, &val);
	return static_cast<std::int64_t>(val);
}

std::uint64_t TermParser::uint64(const Iterator & it) const
{
	if (!is(it->first, in{ERL_INTEGER_EXT, ERL_SMALL_INTEGER_EXT, ERL_SMALL_BIG_EXT, ERL_LARGE_BIG_EXT}))
		throw std::runtime_error("uint64 type error: " + std::to_string(it->first));

	int idx{0};
	unsigned long long val;
	ei_decode_ulonglong(it->second.data(), &idx, &val);
	return static_cast<std::uint64_t>(val);
}

double TermParser::real(const Iterator & it) const
{
	if (!is(it->first, in{ERL_FLOAT_EXT, NEW_FLOAT_EXT}))
		throw std::runtime_error("real type error: {}" + std::to_string(it->first));

	int idx{0};
	double val;
	ei_decode_double(it->second.data(), &idx, &val);
	return val;
}

std::string TermParser::str(const Iterator & it) const
{
	if (!is(it->first, in{ERL_STRING_EXT}))
		throw std::runtime_error("string type error: " + std::to_string(it->first));

	int idx{0};
	char * buffer;
	bool release = false;
	if (it->second.size() > atom_buffer.size()) {
		buffer = new char[it->second.size()]{0};
		release = true;
	} else {
		buffer = &atom_buffer.front();
	}
	ei_decode_string(it->second.data(), &idx, buffer);
	std::string res(buffer);
	if (release) {
		delete[] buffer;
	}
	return res;
}

long TermParser::binary(const Iterator & it, void * dest, size_t size) const
{
	if (!is(it->first, in{ERL_BINARY_EXT}))
		throw std::runtime_error("binary type error: " + std::to_string(it->first));

	constexpr auto binary_header_size = sizeof(char) + sizeof(std::uint32_t);
	long binary_size = static_cast<long>(it->second.size() - 0 - binary_header_size);
	if (size < binary_size)
		throw std::length_error("destination size is too small, required: " + std::to_string(binary_size)
								+ "buffer: " + std::to_string(size));

	int idx{0};
	ei_decode_binary(it->second.data(), &idx, dest, &binary_size);
	return binary_size;
}

TermParser TermParser::complex(const Iterator & it) const
{
	if (!is(it->first, in{ERL_LIST_EXT, ERL_SMALL_TUPLE_EXT, ERL_LARGE_TUPLE_EXT, ERL_MAP_EXT, ERL_NIL_EXT}))
		throw std::runtime_error("complex type error: " + std::to_string(it->first));

	int idx{0};
	int arity = 0;
	switch (it->first) {
		case ERL_LIST_EXT:
			ei_decode_list_header(it->second.data(), &idx, &arity);
			break;

		case ERL_SMALL_TUPLE_EXT:
		case ERL_LARGE_TUPLE_EXT:
			ei_decode_tuple_header(it->second.data(), &idx, &arity);
			break;

		case ERL_MAP_EXT:
			ei_decode_map_header(it->second.data(), &idx, &arity);
			break;

		case ERL_NIL_EXT:
			break;

		default:
			throw std::runtime_error("unknown complex type: " + std::to_string(it->first));
	}

	return TermParser({it->second.data() + idx, it->second.size() - idx});
}

}  // namespace termlib
