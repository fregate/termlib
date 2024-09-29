#pragma once

#include <iostream>

#include <ei.h>

#include <glaze/core/context.hpp>

namespace erlterm
{

template <class It>
bool decode_version(glz::is_context auto && ctx, It && it)
{
	int index{};
	int version{};
	if (ei_decode_version(it, &index, &version) < 0)
	{
		ctx.error = glz::error_code::syntax_error;
		return false;
	}

	std::cerr << "term version: " << version << "\n";

	std::advance(it, index);
	return true;
}

template <class It>
auto get_type(glz::is_context auto && ctx, It && it)
{
	int type{};
	int size{};
	int index{};
	if (ei_get_type(it, &index, &type, &size) < 0)
	{
		ctx.error = glz::error_code::syntax_error;
		return std::pair<int,std::size_t>{-1,-1};
	}

	return std::pair<int,std::size_t>{type, index};
}

template <class It>
auto skip_term(glz::is_context auto && ctx, It && it, int index)
{
	if (ei_skip_term(it, &index) < 0)
	{
		ctx.error = glz::error_code::syntax_error;
		index = 0;
	}

	return it + index;
}

template <class It>
auto term_size(glz::is_context auto && ctx, It && it, int index)
{
	if (ei_skip_term(it, &index) < 0)
	{
		ctx.error = glz::error_code::syntax_error;
		index = 0;
	}

	return static_cast<std::size_t>(index);
}

}
