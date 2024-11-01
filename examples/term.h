#pragma once

#include <erlterm/core/types.hpp>

#include <glaze/glaze.hpp>

struct Term
{
	int value; // 0 - len:5
	erlterm::atom aaa; // 1 - len:3
	erlterm::string str; // 2 - len:3
	std::vector<int> single_type_list; // 3 - len:16
	std::tuple<int, erlterm::string, double> various_types_list; // 4 - len:18
};


struct MetaTerm
{
	MetaTerm()
		: v{0}
	{}

	int v;
	std::string atom;
	erlterm::string str;
	std::vector<int> ints;
	std::tuple<int, erlterm::string, double> t;

	void read_aaa(const erlterm::atom & a)
	{
		atom = std::string(a);
	}

	erlterm::atom write_aaa() const
	{
		return erlterm::atom{atom};
	}
};

template <>
struct glz::meta<MetaTerm> {
   using T = MetaTerm;
   static constexpr auto value = glz::object(
      "value", &T::v,
      "single_type_list", &T::ints,
      "aaa", glz::custom<&T::read_aaa, &T::write_aaa>,
	  &T::str,
      "various_types_list", &T::t
   );
};
