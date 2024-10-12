#include <iostream>
#include <vector>

#include <glaze/beve/read.hpp>
#include <glaze/json/read.hpp>

#include <erlterm/read.hpp>

#include "term.h"

/*
#{aaa => abc,
  single_type_list => [4,5,6],
  str => "def",value => 123,
  various_types_list => {7,"ghi",3.1415926}}
*/
[[maybe_unused]] constexpr std::array<std::uint8_t, 105> test_term{
	131, 
	// map
	116, 0,   0,   0,   5,   
		// aaa                          =>           abc
		100, 0,   3,   97,  97,  97,  100, 0,   3,   97,  98,  99,  
		// single_type_list          =>
		100, 0,   16,	115, 105, 110, 103, 108, 101, 95,  116, 121, 112, 101, 95,  108, 105, 115, 116, 
			// [4, 5, 6]
			107, 0,   3,   4,   5,	6,   
		// str =>                             =>    ["def"]
		100, 0,   3,   115, 116, 114, 107, 0,   3,   100, 101, 102, 
		// value                   =>               123
		100, 0,   5,   118, 97,  108, 117, 101,	97,  123,
		// various_types_list       =>
		100, 0,   18,  118, 97,  114, 105, 111, 117, 115, 95,  116, 121, 112, 101, 115, 95,  108, 105, 115, 116,
			// {           7,                  "ghi",            3.1415926 }
			104, 3,   97,  7,   107, 0,   3,   103, 104, 105, 70,  64,  9,   33,  251, 77,  18,  216, 74};

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
// list: [1,2,3,4,5,6]
[[maybe_unused]] constexpr std::array<std::uint8_t, 10> test_list_as_str{131, 107, 0, 6, 1, 2, 3, 4, 5, 6};
// list: [1,2,300,4,5,6]
[[maybe_unused]] constexpr std::array<std::uint8_t, 22> test_list_as_items{
	131, 108, 0, 0, 0, 6, 97, 1, 97, 2, 98, 0, 0, 1, 44, 97, 4, 97, 5, 97, 6, 106};
// list: ["abc","def","ghi"]
[[maybe_unused]] constexpr std::array<std::uint8_t, 25> test_list_str{
	131, 108, 0, 0, 0, 3, 107, 0, 3, 97, 98, 99, 107, 0, 3, 100, 101, 102, 107, 0, 3, 103, 104, 105, 106};
// tuple: {1,2,3,4,5,6}
[[maybe_unused]] constexpr std::array<std::uint8_t, 15>
	test_tuple_int{131, 104, 6, 97, 1, 97, 2, 97, 3, 97, 4, 97, 5, 97, 6};
// tuple: {1,"abc",2.71828}
[[maybe_unused]] constexpr std::array<std::uint8_t, 20> test_tuple{
	131, 104, 3, 97, 1, 107, 0, 3, 97, 98, 99, 70, 64, 5, 191, 9, 149, 170, 247, 144};

// JSON
[[maybe_unused]] constexpr std::string_view test_json{
	R"({"value":123,"aaa":"abc","str":"def","single_type_list":[4,5,6],"various_types_list":[7,"ghi",3.1415926]})"};
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
	[[maybe_unused]] std::string a;
	[[maybe_unused]] erlterm::atom atm;
	[[maybe_unused]] bool b;
	[[maybe_unused]] std::vector<char> bin;
	[[maybe_unused]] std::array<int, 5> arr;
	[[maybe_unused]] std::vector<int> l;
	[[maybe_unused]] std::vector<std::string> lstr;
	[[maybe_unused]] std::tuple<int, int, int, int, int, int> tup6;
	[[maybe_unused]] std::tuple<int, std::string, double> tup3;

	[[maybe_unused]] glz::error_ctx x;
	// x = glz::read_beve(t, test_beve);
	// x = erlterm::read_term(bin, test_binary);
	// x = erlterm::read_term(arr, test_binary);
	// x = erlterm::read_term(b, test_bool);
	// x = erlterm::read_term(atm, test_atom);
	// x = erlterm::read_term(str, test_string);
	// x = erlterm::read_term(l, test_list_as_items);
	// x = erlterm::read_term(lstr, test_list_str);
	// x = erlterm::read_term(tup6, test_tuple_int);
	// x = erlterm::read_term(tup3, test_tuple);
	x = erlterm::read_term(t, test_term);
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
