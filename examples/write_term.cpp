#include <iostream>
#include <string>
#include <cstdint>

#include <glaze/beve/write.hpp>
#include <glaze/json/write.hpp>

#include <erlterm/write.hpp>

#include "term.h"

int main(int, char**)
{
	std::string buff;

	[[maybe_unused]] Term t{123, "abc", "def", {4,5,6}, {7,"ghi",3.1415926}};

	[[maybe_unused]] glz::error_ctx err;
	// err = erlterm::write_term(t, buff);
	// err = erlterm::write_term(true, buff);
	// err = erlterm::write_term<std::uint64_t>(std::numeric_limits<std::uint64_t>::max(), buff);
	// err = erlterm::write_term(std::int16_t{2}, buff);
	err = erlterm::write_term(3.1415926, buff);

	std::cerr << "<<";
	for (auto c : buff)
	{
		std::cerr << static_cast<int>(uint8_t(c)) << ",";
	}
	std::cerr << ">>\n";
	return 0;
}
