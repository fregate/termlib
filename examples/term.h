#pragma once

struct Term
{
	int value; // 0 - len:5
	std::string aaa; // 1 - len:3
	std::string str; // 2 - len:3
	std::vector<int> single_type_list; // 3 - len:16
	std::tuple<int, std::string, double> various_types_list; // 4 - len:18
};
