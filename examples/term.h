#pragma once

#include <erlterm/core/types.hpp>

struct Term
{
	int value; // 0 - len:5
	erlterm::atom aaa; // 1 - len:3
	erlterm::string str; // 2 - len:3
	std::vector<int> single_type_list; // 3 - len:16
	std::tuple<int, erlterm::string, double> various_types_list; // 4 - len:18
};
