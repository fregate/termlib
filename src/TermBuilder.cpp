#include <cstring>
#include <stdexcept>

#include <iostream>

#include <termlib/Exception.h>
#include <termlib/TermBuilder.h>

namespace termlib
{

TermBuilder::TermBuilder()
{
	ei_x_new_with_version(&buff_);
}

TermBuilder::TermBuilder(TermBuilder && rhs)
	: buff_{rhs.buff_}
	, arities_{std::move(rhs.arities_)}
{
	std::memset(&rhs.buff_, 0, sizeof(rhs.buff_));
}

TermBuilder::~TermBuilder()
{
	ei_x_free(&buff_);
}

TermBuilder & TermBuilder::operator=(TermBuilder && rhs)
{
	buff_ = rhs.buff_;
	arities_ = std::move(rhs.arities_);
	std::memset(&rhs.buff_, 0, sizeof(rhs.buff_));
	return *this;
}

char * TermBuilder::buffer() const
{
	if (!arities_.empty())
	{
		throw std::system_error(std::make_error_code(TermBuilderErrors::InvalidArity), "invalid terms structure");
	}

	return buff_.buff;
}

int TermBuilder::index() const
{
	return buff_.index;
}

bool TermBuilder::empty() const
{
	return buff_.index == 1; // version
}

void TermBuilder::add_atom(const std::string & atom)
{
	if (atom.size() >= MAXATOMLEN)
	{
		throw std::system_error(std::make_error_code(TermBuilderErrors::AtomTooLong));
	}

	ei_x_encode_atom_len_as(&buff_, atom.c_str(), atom.size(), ERLANG_LATIN1, ERLANG_UTF8);
	update_arity();
}

void TermBuilder::add_int32(std::int32_t value)
{
	ei_x_encode_long(&buff_, value);
	update_arity();
}

void TermBuilder::add_uint32(std::uint32_t value)
{
	ei_x_encode_ulong(&buff_, value);
	update_arity();
}

void TermBuilder::add_int64(std::int64_t value)
{
	ei_x_encode_longlong(&buff_, value);
	update_arity();
}

void TermBuilder::add_uint64(std::uint64_t value)
{
	ei_x_encode_ulonglong(&buff_, value);
	update_arity();
}

void TermBuilder::add_binary(const void * buff, std::size_t len)
{
	ei_x_encode_binary(&buff_, buff, len);
	update_arity();
}

void TermBuilder::add_string(const std::string & value)
{
	ei_x_encode_string_len(&buff_, value.c_str(), value.size());
	update_arity();
}

void TermBuilder::add_double(double value)
{
	ei_x_encode_double(&buff_, value);
	update_arity();
}

void TermBuilder::start_list(std::size_t arity)
{
	update_arity();
	if (arity != 0)
	{
		arities_.emplace_back(arity, ComplexStruct::List);
		ei_x_encode_list_header(&buff_, arity);
	}
	else
	{
		ei_x_encode_empty_list(&buff_);
	}
}

void TermBuilder::start_map(std::size_t arity)
{
	update_arity();
	if (arity != 0)
	{
		arities_.emplace_back(arity * 2, ComplexStruct::Map);
	}
	ei_x_encode_map_header(&buff_, arity);
}

void TermBuilder::start_tuple(std::size_t arity)
{
	update_arity();
	if (arity != 0)
	{
		arities_.emplace_back(arity, ComplexStruct::Tuple);
	}
	ei_x_encode_tuple_header(&buff_, arity);
}

void TermBuilder::update_arity()
{
	if (arities_.empty())
	{
		throw std::system_error(std::make_error_code(TermBuilderErrors::InvalidArity), "invalid term build order");
	}

	const auto current = --arities_.back().first;
	if (current != 0)
		return;

	const auto ds = arities_.back().second;
	arities_.pop_back();
	if (ds == ComplexStruct::List)
	{
		ei_x_encode_empty_list(&buff_);
	}
}

} // namespace termlib
