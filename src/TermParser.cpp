#include <limits>
#include <stdexcept>
#include <string>

#include <termlib/TermParser.h>

#include <cstring>

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
	ei_get_type(reinterpret_cast<const char *>(view_.data()), &index_, &type, &size);
	int idx{index_};
	ei_skip_term(reinterpret_cast<const char *>(view_.data()), &idx);
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

	auto res = ei_skip_term(reinterpret_cast<const char *>(view_.data()), &index_);
	if (res < 0 || static_cast<size_t>(index_) >= view_.size()) {
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
	if (ei_decode_version(reinterpret_cast<const char *>(view_.data()), &index, &version) == 0) {
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

namespace parse
{

std::string atom(const TermParser::Iterator & it)
{
	if (!is(it->first, in{ERL_ATOM_EXT, ERL_SMALL_ATOM_EXT, ERL_ATOM_UTF8_EXT, ERL_SMALL_ATOM_UTF8_EXT}))
		throw std::runtime_error(std::string("atom type error: ") + std::to_string(it->first));

	static std::array<char, MAXATOMLEN> atom_buffer{0};

	int idx{0};
	ei_decode_atom(reinterpret_cast<const char *>(it->second.data()), &idx, &atom_buffer.front());
	return std::string(&atom_buffer.front());
}

std::int32_t int32(const TermParser::Iterator & it)
{
	if (!is(it->first, in{ERL_INTEGER_EXT, ERL_SMALL_INTEGER_EXT, ERL_SMALL_BIG_EXT}))
		throw std::runtime_error(std::string("int32 type error: ") + std::to_string(it->first));

	int idx{0};
	long val;
	ei_decode_long(reinterpret_cast<const char *>(it->second.data()), &idx, &val);
	return static_cast<std::int32_t>(val);
}

std::uint32_t uint32(const TermParser::Iterator & it)
{
	if (!is(it->first, in{ERL_INTEGER_EXT, ERL_SMALL_INTEGER_EXT, ERL_SMALL_BIG_EXT}))
		throw std::runtime_error(std::string("uint32 type error: ") + std::to_string(it->first));

	int idx{0};
	unsigned long val;
	ei_decode_ulong(reinterpret_cast<const char *>(it->second.data()), &idx, &val);
	return static_cast<std::uint32_t>(val);
}

std::int64_t int64(const TermParser::Iterator & it)
{
	if (!is(it->first, in{ERL_INTEGER_EXT, ERL_SMALL_INTEGER_EXT, ERL_SMALL_BIG_EXT}))
		throw std::runtime_error(std::string("int64 type error: ") + std::to_string(it->first));

	int idx{0};
	long long val;
	ei_decode_longlong(reinterpret_cast<const char *>(it->second.data()), &idx, &val);
	return static_cast<std::int64_t>(val);
}

std::uint64_t uint64(const TermParser::Iterator & it)
{
	if (!is(it->first, in{ERL_INTEGER_EXT, ERL_SMALL_INTEGER_EXT, ERL_SMALL_BIG_EXT}))
		throw std::runtime_error(std::string("uint64 type error: ") + std::to_string(it->first));

	int idx{0};
	unsigned long long val;
	ei_decode_ulonglong(reinterpret_cast<const char *>(it->second.data()), &idx, &val);
	return static_cast<std::uint64_t>(val);
}

double real(const TermParser::Iterator & it)
{
	if (!is(it->first, in{ERL_FLOAT_EXT, NEW_FLOAT_EXT}))
		throw std::runtime_error(std::string("real type error: ") + std::to_string(it->first));

	int idx{0};
	double val;
	ei_decode_double(reinterpret_cast<const char *>(it->second.data()), &idx, &val);
	return val;
}

std::string str(const TermParser::Iterator & it)
{
	if (!is(it->first, in{ERL_STRING_EXT}))
		throw std::runtime_error(std::string("string type error: ") + std::to_string(it->first));

	int type;
	int size;
	int idx{0};
	ei_get_type(reinterpret_cast<const char *>(it->second.data()), &idx, &type, &size);

	std::string res;
	res.resize(size);
	ei_decode_string(reinterpret_cast<const char *>(it->second.data()), &idx, res.data());
	return res;
}

long binary(const TermParser::Iterator & it, void * dest, size_t size)
{
	if (!is(it->first, in{ERL_BINARY_EXT}))
		throw std::runtime_error(std::string("binary type error: ") + std::to_string(it->first));

	constexpr auto binary_header_size = sizeof(char) + sizeof(std::uint32_t);
	long binary_size = static_cast<long>(it->second.size() - 0 - binary_header_size);
	if (size < static_cast<size_t>(binary_size))
		return binary_size;

	int idx{0};
	ei_decode_binary(reinterpret_cast<const char *>(it->second.data()), &idx, dest, &binary_size);
	return binary_size;
}

TermParser complex(const TermParser::Iterator & it)
{
	if (!is(it->first,
			in{ERL_LIST_EXT, ERL_STRING_EXT, ERL_SMALL_TUPLE_EXT, ERL_LARGE_TUPLE_EXT, ERL_MAP_EXT, ERL_NIL_EXT}))
		throw std::runtime_error(std::string("complex type error: ") + std::to_string(it->first));

	int idx{0};
	int arity = 0;
	switch (it->first) {
		case ERL_LIST_EXT:
			ei_decode_list_header(reinterpret_cast<const char *>(it->second.data()), &idx, &arity);
			break;

		// TODO
		// case ERL_STRING_EXT:
		// 	ei_decode_string();
		// 	break;

		case ERL_SMALL_TUPLE_EXT:
		case ERL_LARGE_TUPLE_EXT:
			ei_decode_tuple_header(reinterpret_cast<const char *>(it->second.data()), &idx, &arity);
			break;

		case ERL_MAP_EXT:
			ei_decode_map_header(reinterpret_cast<const char *>(it->second.data()), &idx, &arity);
			break;

		case ERL_NIL_EXT:
			break;

		default:
			throw std::runtime_error(std::string("unknown complex type: ") + std::to_string(it->first));
	}

	return TermParser({it->second.data() + idx, it->second.size() - idx});
}

}  // namespace parse

}  // namespace termlib
