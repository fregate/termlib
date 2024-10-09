#include <iostream>
#include <vector>

#include <erlterm/read.hpp>
#include <glaze/beve/read.hpp>
#include <glaze/beve/write.hpp>
#include <glaze/json/read.hpp>

struct Term
{
	int value;
	std::string atom;
	std::string str;
	std::vector<int> single_type_list;
	std::tuple<int, std::string, double> various_types_list;
};

[[maybe_unused]] constexpr std::array<std::uint8_t, 109> test_term{
	131, 116, 0,   0,   0,   5,   119, 9,   115, 111, 109, 101, 95,  97,  116, 111, 109, 119, 9,   115, 111, 109,
	101, 95,  97,  116, 111, 109, 119, 9,   105, 110, 116, 95,  118, 97,  108, 117, 101, 97,  123, 119, 12,  115,
	116, 114, 105, 110, 103, 95,  118, 97,  108, 117, 101, 107, 0,   3,   97,  98,  99,  119, 10,  108, 105, 115,
	116, 95,  118, 97,  108, 117, 101, 107, 0,   3,   4,   5,   6,   119, 11,  116, 117, 112, 108, 101, 95,  118,
	97,  108, 117, 101, 104, 3,   97,  7,   107, 0,   1,   100, 70,  64,  9,   33,  251, 77,  18,  216, 74};

// 1
[[maybe_unused]] constexpr std::array<std::uint8_t, 3> test_int{131, 97, 1};
// 3.1415926
[[maybe_unused]] constexpr std::array<std::uint8_t, 10> test_double{131, 70, 64, 9, 33, 251, 77, 18, 216, 74};
// someatom
[[maybe_unused]] constexpr std::array<std::uint8_t, 12>
	test_atom{131, 100, 0, 8, 115, 111, 109, 101, 97, 116, 111, 109};
// "some string"
[[maybe_unused]] constexpr std::array<std::uint8_t, 15>
	test_string{131, 107, 0, 11, 115, 111, 109, 101, 32, 115, 116, 114, 105, 110, 103};
// bool: true
[[maybe_unused]] constexpr std::array<std::uint8_t, 8> test_bool{131, 100, 0, 4, 116, 114, 117, 101};
// binary: 1,2,3,4,5
[[maybe_unused]] constexpr std::array<std::uint8_t, 11> test_binary{131, 109, 0, 0, 0, 5, 1, 2, 3, 4, 5};

// JSON
[[maybe_unused]] constexpr std::string_view test_json{
	R"({"value":123,"atom":"some_atom","str":"some_string","single_type_list":[4,5,6],"various_types_list":[7,"def",3.1415926]})"};
// BEVE
[[maybe_unused]] constexpr std::array<std::uint8_t, 117> test_beve{
	3,   20,  20,  118, 97,  108, 117, 101, 73,  123, 0,   0,   0,   16,  97,  116, 111, 109, 2,   36,
	115, 111, 109, 101, 95,  97,  116, 111, 109, 12,  115, 116, 114, 2,   44,  115, 111, 109, 101, 95,
	115, 116, 114, 105, 110, 103, 64,  115, 105, 110, 103, 108, 101, 95,  116, 121, 112, 101, 95,  108,
	105, 115, 116, 76,  12,  4,   0,   0,   0,   5,   0,   0,   0,   6,   0,   0,   0,   72,  118, 97,
	114, 105, 111, 117, 115, 95,  116, 121, 112, 101, 115, 95,  108, 105, 115, 116, 5,   12,  73,  7,
	0,   0,   0,   2,   12,  100, 101, 102, 97,  74,  216, 18,  77,  251, 33,  9,   64,
};

int main(int, char **)
{
	[[maybe_unused]] Term t;
	[[maybe_unused]] int i;
	[[maybe_unused]] double real;
	[[maybe_unused]] std::string str;
	[[maybe_unused]] erlterm::atom a;
	[[maybe_unused]] bool b;
	[[maybe_unused]] std::vector<char> bin;
	[[maybe_unused]] std::array<int, 5> arr;

	[[maybe_unused]] glz::error_ctx x;
	// x = glz::read_beve(t, test_beve);
	x = erlterm::read_term(bin, test_binary);
	// x = erlterm::read_term(arr, test_binary);
	// x = erlterm::read_term(b, test_bool);
	// x = erlterm::read_term(a, test_atom);
	// x = erlterm::read_term(str, test_string);
	// x = erlterm::read_term(t, test_term);
	// x = erlterm::read_term(i, test_int);
	// x = erlterm::read_term(real, test_double);
	// x = glz::read_json(t, test_json);
	// std::vector<char> out;
	// x = glz::write_beve(t, out);
	// std::for_each(
	// 	out.begin(),
	// 	out.end(),
	// 	[](const auto & c)
	// 	{
	// 		std::cerr << static_cast<unsigned>(static_cast<unsigned char>(c)) << ",";
	// 	});
	return 0;
}
