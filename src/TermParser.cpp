#include <limits>
#include <stdexcept>
#include <string>

#include <termlib/Exception.h>
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
	{
		return;
	}

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
	{
		return *this;
	}

	auto res = ei_skip_term(reinterpret_cast<const char *>(view_.data()), &index_);
	if (res < 0 || static_cast<size_t>(index_) >= view_.size())
	{
		index_ = std::numeric_limits<int>::min();
		return *this;
	}

	fill_value();
	return *this;
}

TermParser::Iterator TermParser::Iterator::operator++(int)
{
	auto tmp = *this;
	++(*this);
	return tmp;
}

// TermParser

TermParser::TermParser(const void * data, std::size_t size)
	: TermParser(BinaryBlock{reinterpret_cast<const BlockData *>(data), size})
{
}

TermParser::TermParser(BinaryBlock view)
	: view_{view}
{
	if (view_.data() == nullptr)
	{
		throw parser_error(std::make_error_code(TermParserErrors::NullBlock));
	}

	int index{0};
	int version;
	if (ei_decode_version(reinterpret_cast<const char *>(view_.data()), &index, &version) == 0)
	{
		view_ = view_.subspan(index);
	}
}

TermParser::TermParser(TermParser & rhs)
	: view_{rhs.view_}
{
}

TermParser::TermParser(TermParser && rhs)
	: view_{std::move(rhs.view_)}
{
}

const TermParser::Iterator TermParser::begin() const
{
	// check if no data (empty tuple, empty map)
	if (view_.empty())
	{
		return end();
	}

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
	{
		return end();
	}

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
	if (!is_atom(it))
	{
		throw parser_error(std::make_error_code(TermParserErrors::TypeError), "atom", it->first);
	}

	static std::array<char, MAXATOMLEN> atom_buffer{0};

	int idx{0};
	ei_decode_atom(reinterpret_cast<const char *>(it->second.data()), &idx, &atom_buffer.front());
	return std::string(&atom_buffer.front());
}

bool is_atom(const TermParser::Iterator & it)
{
	return is(it->first, in{ERL_ATOM_EXT, ERL_SMALL_ATOM_EXT, ERL_ATOM_UTF8_EXT, ERL_SMALL_ATOM_UTF8_EXT});
}

std::int32_t int32(const TermParser::Iterator & it)
{
	if (!is_int32(it))
	{
		throw parser_error(std::make_error_code(TermParserErrors::TypeError), "int32", it->first);
	}

	int idx{0};
	long val;
	ei_decode_long(reinterpret_cast<const char *>(it->second.data()), &idx, &val);
	return static_cast<std::int32_t>(val);
}

bool is_integer(const TermParser::Iterator & it)
{
	return is(it->first, in{ERL_INTEGER_EXT, ERL_SMALL_INTEGER_EXT, ERL_SMALL_BIG_EXT, ERL_LARGE_BIG_EXT});
}

bool is_int32(const TermParser::Iterator & it)
{
	return is_integer(it);
}

std::uint32_t uint32(const TermParser::Iterator & it)
{
	if (!is_uint32(it))
	{
		throw parser_error(std::make_error_code(TermParserErrors::TypeError), "uint32", it->first);
	}

	int idx{0};
	unsigned long val;
	ei_decode_ulong(reinterpret_cast<const char *>(it->second.data()), &idx, &val);
	return static_cast<std::uint32_t>(val);
}

bool is_uint32(const TermParser::Iterator & it)
{
	return is_integer(it);
}

std::int64_t int64(const TermParser::Iterator & it)
{
	if (!is_int64(it))
	{
		throw parser_error(std::make_error_code(TermParserErrors::TypeError), "int64", it->first);
	}

	int idx{0};
	long long val;
	ei_decode_longlong(reinterpret_cast<const char *>(it->second.data()), &idx, &val);
	return static_cast<std::int64_t>(val);
}

bool is_int64(const TermParser::Iterator & it)
{
	return is_integer(it);
}

std::uint64_t uint64(const TermParser::Iterator & it)
{
	if (!is_uint64(it))
	{
		throw parser_error(std::make_error_code(TermParserErrors::TypeError), "uint64", it->first);
	}

	int idx{0};
	unsigned long long val;
	ei_decode_ulonglong(reinterpret_cast<const char *>(it->second.data()), &idx, &val);
	return static_cast<std::uint64_t>(val);
}

bool is_uint64(const TermParser::Iterator & it)
{
	return is_integer(it);
}

double real(const TermParser::Iterator & it)
{
	if (!is_real(it))
	{
		throw parser_error(std::make_error_code(TermParserErrors::TypeError), "double", it->first);
	}

	int idx{0};
	double val;
	ei_decode_double(reinterpret_cast<const char *>(it->second.data()), &idx, &val);
	return val;
}

bool is_real(const TermParser::Iterator & it)
{
	return is(it->first, in{ERL_FLOAT_EXT, NEW_FLOAT_EXT});
}

std::string str(const TermParser::Iterator & it)
{
	if (!is_str(it))
	{
		throw parser_error(std::make_error_code(TermParserErrors::TypeError), "string", it->first);
	}

	int type;
	int size;
	int idx{0};
	ei_get_type(reinterpret_cast<const char *>(it->second.data()), &idx, &type, &size);

	if (type == ERL_NIL_EXT)
	{
		return std::string{};
	}

	std::string res;
	res.resize(size);
	ei_decode_string(reinterpret_cast<const char *>(it->second.data()), &idx, res.data());
	return res;
}

bool is_str(const TermParser::Iterator & it)
{
	return is(it->first, in{ERL_STRING_EXT, ERL_NIL_EXT});
}

std::string u8str(const TermParser::Iterator & it)
{
	if (!is_list(it))
	{
		throw parser_error(std::make_error_code(TermParserErrors::TypeError), "utf8", it->first);
	}

	std::string res;
	res.reserve(it->second.size());

	const auto list = parse::list(it);
	auto list_it = list.begin();

	while (list_it != list.end())
	{
		auto num = uint32(list_it++);
		if (num < 0x80)
		{
			res.push_back(static_cast<char>(num));
		}
		else if (num < 0x800)
		{
			res.push_back(0xC0 | (num >> 6));
			res.push_back(0x80 | (num & 0x3F));
		}
		else if (num < 0x10000)
		{
			res.push_back(0xE0 | (num >> 12));
			res.push_back(0x80 | ((num >> 6) & 0x3F));
			res.push_back(0x80 | (num & 0x3F));
		}
		else
		{
			res.push_back(0xF0 | (num >> 18));
			res.push_back(0x80 | ((num >> 12) & 0x3F));
			res.push_back(0x80 | ((num >> 6) & 0x3F));
			res.push_back(0x80 | (num & 0x3F));
		}
	}

	return res;
}

long binary(const TermParser::Iterator & it, void * dest, size_t size)
{
	if (!is_binary(it))
	{
		throw parser_error(std::make_error_code(TermParserErrors::TypeError), "binary", it->first);
	}

	constexpr auto binary_header_size = sizeof(char) + sizeof(std::uint32_t);
	long binary_size = static_cast<long>(it->second.size() - binary_header_size);
	if (size < static_cast<size_t>(binary_size))
	{
		return binary_size;
	}

	int idx{0};
	ei_decode_binary(reinterpret_cast<const char *>(it->second.data()), &idx, dest, &binary_size);
	return binary_size;
}

bool is_binary(const TermParser::Iterator & it)
{
	return is(it->first, in{ERL_BINARY_EXT});
}

TermParser complex(const TermParser::Iterator & it)
{
	if (!is_complex(it))
	{
		throw parser_error(std::make_error_code(TermParserErrors::TypeError), "complex", it->first);
	}

	switch (it->first)
	{
	case ERL_LIST_EXT:
	case ERL_STRING_EXT:
	case ERL_NIL_EXT:
		return list(it);

	case ERL_SMALL_TUPLE_EXT:
	case ERL_LARGE_TUPLE_EXT:
		return tuple(it);

	case ERL_MAP_EXT:
		return map(it);

	default:
		break;
	}

	throw parser_error(std::make_error_code(TermParserErrors::TypeError), "complex", it->first);
}

TermParser list(const TermParser::Iterator & it)
{
	if (!is_list(it))
	{
		throw parser_error(std::make_error_code(TermParserErrors::TypeError), "list", it->first);
	}

	int idx{0};
	int arity{0};
	if (it->first != ERL_NIL_EXT)
	{
		ei_decode_list_header(reinterpret_cast<const char *>(it->second.data()), &idx, &arity);
	}
	return TermParser(TermParser::BinaryBlock{it->second.data() + idx, it->second.size() - idx});
}

TermParser map(const TermParser::Iterator & it)
{
	if (!is_map(it))
	{
		throw parser_error(std::make_error_code(TermParserErrors::TypeError), "map", it->first);
	}

	int idx{0};
	int arity{0};
	ei_decode_map_header(reinterpret_cast<const char *>(it->second.data()), &idx, &arity);
	return TermParser(TermParser::BinaryBlock{it->second.data() + idx, it->second.size() - idx});
}

TermParser tuple(const TermParser::Iterator & it)
{
	if (!is_tuple(it))
	{
		throw parser_error(std::make_error_code(TermParserErrors::TypeError), "tuple", it->first);
	}

	int idx{0};
	int arity{0};
	ei_decode_tuple_header(reinterpret_cast<const char *>(it->second.data()), &idx, &arity);
	return TermParser(TermParser::BinaryBlock{it->second.data() + idx, it->second.size() - idx});
}

bool is_complex(const TermParser::Iterator & it)
{
	return is_tuple(it) || is_list(it) || is_map(it);
}

bool is_tuple(const TermParser::Iterator & it)
{
	return is(it->first, in{ERL_SMALL_TUPLE_EXT, ERL_LARGE_TUPLE_EXT});
}

bool is_list(const TermParser::Iterator & it)
{
	return is(it->first, in{ERL_LIST_EXT, ERL_STRING_EXT, ERL_NIL_EXT});
}

bool is_map(const TermParser::Iterator & it)
{
	return is(it->first, in{ERL_MAP_EXT});
}

} // namespace parse

} // namespace termlib
