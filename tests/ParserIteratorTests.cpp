#include <unordered_map>

#include <gtest/gtest.h>

#include <termlib/TermBuilder.h>
#include <termlib/TermParser.h>

namespace termlib::tests
{

class ParserIteratorTest : public ::testing::Test
{
protected:
	TermBuilder builder_;

	const std::unordered_map<unsigned char, std::vector<unsigned char>> terms{
		{ERL_ATOM_EXT, {131, ERL_ATOM_EXT, 0, 1, 97}},
		{ERL_SMALL_ATOM_EXT, {131, ERL_SMALL_ATOM_EXT, 0, 1, 97}},
		{ERL_ATOM_UTF8_EXT, {131, ERL_ATOM_UTF8_EXT, 0, 1, 97}},
		{ERL_SMALL_ATOM_UTF8_EXT, {131, ERL_SMALL_ATOM_UTF8_EXT, 0, 1, 97}},
		{ERL_SMALL_INTEGER_EXT, {131, ERL_SMALL_INTEGER_EXT, 1}},
		{ERL_INTEGER_EXT, {131, ERL_INTEGER_EXT, 78, 239, 150, 171}},
		{ERL_SMALL_BIG_EXT, {131, ERL_SMALL_BIG_EXT, 8, 0, 255, 255, 255, 255, 255, 255, 255, 127}},
		{ERL_LARGE_BIG_EXT, {131, ERL_LARGE_BIG_EXT, 8, 0, 255, 255, 255, 255, 255, 255, 255, 127}},
		{ERL_FLOAT_EXT, {131, ERL_FLOAT_EXT, 64, 140, 208, 252, 102, 124, 191, 76}},
		{NEW_FLOAT_EXT, {131, NEW_FLOAT_EXT, 64, 140, 208, 252, 102, 124, 191, 76}},
		{ERL_STRING_EXT, {131, ERL_STRING_EXT, 0, 4, 50, 50, 50, 50}},
		{ERL_NIL_EXT, {131, ERL_NIL_EXT}},
		{ERL_BINARY_EXT, {131, ERL_BINARY_EXT, 0, 0, 0, 5, 1, 2, 3, 4, 5}},
		{ERL_MAP_EXT, {131, ERL_MAP_EXT, 0, 0, 0, 1, 97, 1, 97, 1}},
		{ERL_SMALL_TUPLE_EXT, {131, ERL_SMALL_TUPLE_EXT, 0}},
		{ERL_LARGE_TUPLE_EXT, {131, ERL_LARGE_TUPLE_EXT, 0, 0, 0, 0}},
		{ERL_LIST_EXT, {131, ERL_LIST_EXT, 0, 1, 1}},
	};
};

TEST_F(ParserIteratorTest, CheckAtom)
{
	constexpr std::array<const unsigned char, 4> exts{
		ERL_ATOM_EXT,
		ERL_SMALL_ATOM_EXT,
		ERL_ATOM_UTF8_EXT,
		ERL_SMALL_ATOM_UTF8_EXT};

	for (const auto ext : exts)
	{
		const auto & term = terms.at(ext);
		TermParser p({reinterpret_cast<const unsigned char *>(term.data()), term.size()});
		EXPECT_TRUE(parse::is_atom(p.begin()));
	}

	constexpr std::array<const unsigned char, 13> invalid_exts{
		ERL_SMALL_INTEGER_EXT,
		ERL_INTEGER_EXT,
		ERL_FLOAT_EXT,
		NEW_FLOAT_EXT,
		ERL_SMALL_TUPLE_EXT,
		ERL_LARGE_TUPLE_EXT,
		ERL_NIL_EXT,
		ERL_STRING_EXT,
		ERL_LIST_EXT,
		ERL_BINARY_EXT,
		ERL_SMALL_BIG_EXT,
		ERL_LARGE_BIG_EXT,
		ERL_MAP_EXT,
	};

	for (const auto ext : invalid_exts)
	{
		const auto & term = terms.at(ext);
		TermParser p({reinterpret_cast<const unsigned char *>(term.data()), term.size()});
		EXPECT_FALSE(parse::is_atom(p.begin()));
	}
}

TEST_F(ParserIteratorTest, CheckInt97)
{
	const auto & term = terms.at(ERL_SMALL_INTEGER_EXT);
	TermParser p({reinterpret_cast<const unsigned char *>(term.data()), term.size()});
	EXPECT_TRUE(parse::is_int32(p.begin()));
	EXPECT_TRUE(parse::is_uint32(p.begin()));

	constexpr std::array<const unsigned char, 13> invalid_exts{
		ERL_FLOAT_EXT,
		NEW_FLOAT_EXT,
		ERL_ATOM_EXT,
		ERL_SMALL_ATOM_EXT,
		ERL_ATOM_UTF8_EXT,
		ERL_SMALL_ATOM_UTF8_EXT,
		ERL_SMALL_TUPLE_EXT,
		ERL_LARGE_TUPLE_EXT,
		ERL_NIL_EXT,
		ERL_STRING_EXT,
		ERL_LIST_EXT,
		ERL_BINARY_EXT,
		ERL_MAP_EXT,
	};

	for (const auto ext : invalid_exts)
	{
		const auto & term = terms.at(ext);
		TermParser p({reinterpret_cast<const unsigned char *>(term.data()), term.size()});
		EXPECT_FALSE(parse::is_int32(p.begin()));
		EXPECT_FALSE(parse::is_uint32(p.begin()));
	}
}

TEST_F(ParserIteratorTest, CheckInt98)
{
	const auto & term = terms.at(ERL_INTEGER_EXT);
	TermParser p({reinterpret_cast<const unsigned char *>(term.data()), term.size()});
	EXPECT_TRUE(parse::is_int32(p.begin()));
	EXPECT_TRUE(parse::is_uint32(p.begin()));

	constexpr std::array<const unsigned char, 13> invalid_exts{
		ERL_FLOAT_EXT,
		NEW_FLOAT_EXT,
		ERL_ATOM_EXT,
		ERL_SMALL_ATOM_EXT,
		ERL_ATOM_UTF8_EXT,
		ERL_SMALL_ATOM_UTF8_EXT,
		ERL_SMALL_TUPLE_EXT,
		ERL_LARGE_TUPLE_EXT,
		ERL_NIL_EXT,
		ERL_STRING_EXT,
		ERL_LIST_EXT,
		ERL_BINARY_EXT,
		ERL_MAP_EXT,
	};

	for (const auto ext : invalid_exts)
	{
		const auto & term = terms.at(ext);
		TermParser p({reinterpret_cast<const unsigned char *>(term.data()), term.size()});
		EXPECT_FALSE(parse::is_int32(p.begin()));
		EXPECT_FALSE(parse::is_uint32(p.begin()));
	}
}

TEST_F(ParserIteratorTest, CheckLongLong)
{
	constexpr std::array<const unsigned char, 2> exts{ERL_SMALL_BIG_EXT, ERL_LARGE_BIG_EXT};
	for (const auto ext : exts)
	{
		const auto & term = terms.at(ext);
		TermParser p({reinterpret_cast<const unsigned char *>(term.data()), term.size()});
		EXPECT_TRUE(parse::is_int64(p.begin()));
		EXPECT_TRUE(parse::is_uint64(p.begin()));
	}

	constexpr std::array<const unsigned char, 13> invalid_exts{
		ERL_FLOAT_EXT,
		NEW_FLOAT_EXT,
		ERL_ATOM_EXT,
		ERL_SMALL_ATOM_EXT,
		ERL_ATOM_UTF8_EXT,
		ERL_SMALL_ATOM_UTF8_EXT,
		ERL_SMALL_TUPLE_EXT,
		ERL_LARGE_TUPLE_EXT,
		ERL_NIL_EXT,
		ERL_STRING_EXT,
		ERL_LIST_EXT,
		ERL_BINARY_EXT,
		ERL_MAP_EXT,
	};

	for (const auto ext : invalid_exts)
	{
		const auto & term = terms.at(ext);
		TermParser p({reinterpret_cast<const unsigned char *>(term.data()), term.size()});
		EXPECT_FALSE(parse::is_int64(p.begin()));
		EXPECT_FALSE(parse::is_uint64(p.begin()));
	}
}

TEST_F(ParserIteratorTest, CheckFloat)
{
	constexpr std::array<const unsigned char, 2> exts{ERL_FLOAT_EXT, NEW_FLOAT_EXT};
	for (const auto ext : exts)
	{
		const auto & term = terms.at(ext);
		TermParser p({reinterpret_cast<const unsigned char *>(term.data()), term.size()});
		EXPECT_TRUE(parse::is_real(p.begin()));
	}

	constexpr std::array<const unsigned char, 15> invalid_exts{
		ERL_SMALL_INTEGER_EXT,
		ERL_INTEGER_EXT,
		ERL_ATOM_EXT,
		ERL_SMALL_ATOM_EXT,
		ERL_ATOM_UTF8_EXT,
		ERL_SMALL_ATOM_UTF8_EXT,
		ERL_SMALL_TUPLE_EXT,
		ERL_LARGE_TUPLE_EXT,
		ERL_NIL_EXT,
		ERL_STRING_EXT,
		ERL_LIST_EXT,
		ERL_BINARY_EXT,
		ERL_SMALL_BIG_EXT,
		ERL_LARGE_BIG_EXT,
		ERL_MAP_EXT,
	};

	for (const auto ext : invalid_exts)
	{
		const auto & term = terms.at(ext);
		TermParser p({reinterpret_cast<const unsigned char *>(term.data()), term.size()});
		EXPECT_FALSE(parse::is_real(p.begin()));
	}
}

TEST_F(ParserIteratorTest, CheckString)
{
	const auto & term = terms.at(ERL_STRING_EXT);
	TermParser p({reinterpret_cast<const unsigned char *>(term.data()), term.size()});
	EXPECT_TRUE(parse::is_str(p.begin()));

	constexpr std::array<const unsigned char, 15> invalid_exts{
		ERL_FLOAT_EXT,
		NEW_FLOAT_EXT,
		ERL_SMALL_INTEGER_EXT,
		ERL_INTEGER_EXT,
		ERL_ATOM_EXT,
		ERL_SMALL_ATOM_EXT,
		ERL_ATOM_UTF8_EXT,
		ERL_SMALL_ATOM_UTF8_EXT,
		ERL_SMALL_TUPLE_EXT,
		ERL_LARGE_TUPLE_EXT,
		ERL_LIST_EXT,
		ERL_BINARY_EXT,
		ERL_SMALL_BIG_EXT,
		ERL_LARGE_BIG_EXT,
		ERL_MAP_EXT,
	};

	for (const auto ext : invalid_exts)
	{
		const auto & term = terms.at(ext);
		TermParser p({reinterpret_cast<const unsigned char *>(term.data()), term.size()});
		EXPECT_FALSE(parse::is_str(p.begin()));
	}
}

TEST_F(ParserIteratorTest, CheckEmptyString)
{
	const auto & term = terms.at(ERL_NIL_EXT);
	TermParser p({reinterpret_cast<const unsigned char *>(term.data()), term.size()});
	EXPECT_TRUE(parse::is_str(p.begin()));

	constexpr std::array<const unsigned char, 15> invalid_exts{
		ERL_FLOAT_EXT,
		NEW_FLOAT_EXT,
		ERL_SMALL_INTEGER_EXT,
		ERL_INTEGER_EXT,
		ERL_ATOM_EXT,
		ERL_SMALL_ATOM_EXT,
		ERL_ATOM_UTF8_EXT,
		ERL_SMALL_ATOM_UTF8_EXT,
		ERL_SMALL_TUPLE_EXT,
		ERL_LARGE_TUPLE_EXT,
		ERL_LIST_EXT,
		ERL_BINARY_EXT,
		ERL_SMALL_BIG_EXT,
		ERL_LARGE_BIG_EXT,
		ERL_MAP_EXT,
	};

	for (const auto ext : invalid_exts)
	{
		const auto & term = terms.at(ext);
		TermParser p({reinterpret_cast<const unsigned char *>(term.data()), term.size()});
		EXPECT_FALSE(parse::is_str(p.begin()));
	}
}

TEST_F(ParserIteratorTest, CheckBinary)
{
	const auto & term = terms.at(ERL_BINARY_EXT);
	TermParser p({reinterpret_cast<const unsigned char *>(term.data()), term.size()});
	EXPECT_TRUE(parse::is_binary(p.begin()));

	constexpr std::array<const unsigned char, 16> invalid_exts{
		ERL_SMALL_INTEGER_EXT,
		ERL_INTEGER_EXT,
		ERL_FLOAT_EXT,
		NEW_FLOAT_EXT,
		ERL_ATOM_EXT,
		ERL_SMALL_ATOM_EXT,
		ERL_ATOM_UTF8_EXT,
		ERL_SMALL_ATOM_UTF8_EXT,
		ERL_SMALL_TUPLE_EXT,
		ERL_LARGE_TUPLE_EXT,
		ERL_NIL_EXT,
		ERL_STRING_EXT,
		ERL_LIST_EXT,
		ERL_SMALL_BIG_EXT,
		ERL_LARGE_BIG_EXT,
		ERL_MAP_EXT,
	};

	for (const auto ext : invalid_exts)
	{
		const auto & term = terms.at(ext);
		TermParser p({reinterpret_cast<const unsigned char *>(term.data()), term.size()});
		EXPECT_FALSE(parse::is_binary(p.begin()));
	}
}

TEST_F(ParserIteratorTest, CheckMap)
{
	const auto & term = terms.at(ERL_MAP_EXT);
	TermParser p({reinterpret_cast<const unsigned char *>(term.data()), term.size()});
	EXPECT_TRUE(parse::is_map(p.begin()));

	constexpr std::array<const unsigned char, 16> invalid_exts{
		ERL_SMALL_INTEGER_EXT,
		ERL_INTEGER_EXT,
		ERL_FLOAT_EXT,
		NEW_FLOAT_EXT,
		ERL_ATOM_EXT,
		ERL_SMALL_ATOM_EXT,
		ERL_ATOM_UTF8_EXT,
		ERL_SMALL_ATOM_UTF8_EXT,
		ERL_SMALL_TUPLE_EXT,
		ERL_LARGE_TUPLE_EXT,
		ERL_NIL_EXT,
		ERL_STRING_EXT,
		ERL_LIST_EXT,
		ERL_BINARY_EXT,
		ERL_SMALL_BIG_EXT,
		ERL_LARGE_BIG_EXT,
	};

	for (const auto ext : invalid_exts)
	{
		const auto & term = terms.at(ext);
		TermParser p({reinterpret_cast<const unsigned char *>(term.data()), term.size()});
		EXPECT_FALSE(parse::is_map(p.begin()));
	}
}

TEST_F(ParserIteratorTest, CheckTuple)
{
	constexpr std::array<const unsigned char, 2> exts{ERL_SMALL_TUPLE_EXT, ERL_LARGE_TUPLE_EXT};
	for (const auto ext : exts)
	{
		const auto & term = terms.at(ext);
		TermParser p({reinterpret_cast<const unsigned char *>(term.data()), term.size()});
		EXPECT_TRUE(parse::is_tuple(p.begin()));
	}

	constexpr std::array<const unsigned char, 15> invalid_exts{
		ERL_SMALL_INTEGER_EXT,
		ERL_INTEGER_EXT,
		ERL_FLOAT_EXT,
		NEW_FLOAT_EXT,
		ERL_ATOM_EXT,
		ERL_SMALL_ATOM_EXT,
		ERL_ATOM_UTF8_EXT,
		ERL_SMALL_ATOM_UTF8_EXT,
		ERL_NIL_EXT,
		ERL_STRING_EXT,
		ERL_LIST_EXT,
		ERL_BINARY_EXT,
		ERL_SMALL_BIG_EXT,
		ERL_LARGE_BIG_EXT,
		ERL_MAP_EXT,
	};

	for (const auto ext : invalid_exts)
	{
		const auto & term = terms.at(ext);
		TermParser p({reinterpret_cast<const unsigned char *>(term.data()), term.size()});
		EXPECT_FALSE(parse::is_tuple(p.begin()));
	}
}

TEST_F(ParserIteratorTest, CheckList)
{
	constexpr std::array<const unsigned char, 2> exts{ERL_LIST_EXT, ERL_NIL_EXT};
	for (const auto ext : exts)
	{
		const auto & term = terms.at(ext);
		TermParser p({reinterpret_cast<const unsigned char *>(term.data()), term.size()});
		EXPECT_TRUE(parse::is_list(p.begin()));
	}

	constexpr std::array<const unsigned char, 14> invalid_exts{
		ERL_SMALL_INTEGER_EXT,
		ERL_INTEGER_EXT,
		ERL_FLOAT_EXT,
		NEW_FLOAT_EXT,
		ERL_ATOM_EXT,
		ERL_SMALL_ATOM_EXT,
		ERL_ATOM_UTF8_EXT,
		ERL_SMALL_ATOM_UTF8_EXT,
		ERL_SMALL_TUPLE_EXT,
		ERL_LARGE_TUPLE_EXT,
		ERL_BINARY_EXT,
		ERL_SMALL_BIG_EXT,
		ERL_LARGE_BIG_EXT,
		ERL_MAP_EXT,
	};

	for (const auto ext : invalid_exts)
	{
		const auto & term = terms.at(ext);
		TermParser p({reinterpret_cast<const unsigned char *>(term.data()), term.size()});
		EXPECT_FALSE(parse::is_list(p.begin()));
	}
}

TEST_F(ParserIteratorTest, CheckComplex)
{
	constexpr std::array<const unsigned char, 5> exts{
		ERL_LIST_EXT,
		ERL_NIL_EXT,
		ERL_LARGE_TUPLE_EXT,
		ERL_SMALL_TUPLE_EXT,
		ERL_MAP_EXT};
	for (const auto ext : exts)
	{
		const auto & term = terms.at(ext);
		TermParser p({reinterpret_cast<const unsigned char *>(term.data()), term.size()});
		EXPECT_TRUE(parse::is_complex(p.begin()));
	}

	constexpr std::array<const unsigned char, 11> invalid_exts{
		ERL_SMALL_INTEGER_EXT,
		ERL_INTEGER_EXT,
		ERL_FLOAT_EXT,
		NEW_FLOAT_EXT,
		ERL_ATOM_EXT,
		ERL_SMALL_ATOM_EXT,
		ERL_ATOM_UTF8_EXT,
		ERL_SMALL_ATOM_UTF8_EXT,
		ERL_BINARY_EXT,
		ERL_SMALL_BIG_EXT,
		ERL_LARGE_BIG_EXT,
	};

	for (const auto ext : invalid_exts)
	{
		const auto & term = terms.at(ext);
		TermParser p({reinterpret_cast<const unsigned char *>(term.data()), term.size()});
		EXPECT_FALSE(parse::is_list(p.begin()));
	}
}

TEST_F(ParserIteratorTest, Iterator_WhileList)
{
	const std::size_t count{3};

	EXPECT_NO_THROW(builder_.start_list(count));
	for (std::size_t idx = 0; idx < count; ++idx)
	{
		EXPECT_NO_THROW(builder_.add_uint64(idx));
	}

	const auto buffer = builder_.buffer();
	const size_t size = builder_.index();

	TermParser parser({reinterpret_cast<const unsigned char *>(buffer), size});
	const auto list = termlib::parse::list(parser.begin());
	std::size_t c{0};

	auto it = list.begin();
	while (it != list.end())
	{
		const auto v = termlib::parse::uint64((*it).begin());
		EXPECT_EQ(v, c);
		c++;
		++it;
	}

	EXPECT_EQ(c, count);

	// check boundary
	for (int over = 10; over > 0; --over)
	{
		EXPECT_TRUE(it++ == list.end());
	}
}

TEST_F(ParserIteratorTest, Iterator_ForList)
{
	const std::size_t count{3};

	EXPECT_NO_THROW(builder_.start_list(count));
	for (std::size_t idx = 0; idx < count; ++idx)
	{
		EXPECT_NO_THROW(builder_.add_uint64(idx));
	}

	const auto buffer = builder_.buffer();
	const size_t size = builder_.index();

	TermParser parser({reinterpret_cast<const unsigned char *>(buffer), size});
	const auto list = termlib::parse::list(parser.begin());
	std::size_t c{0};
	for (auto p : list)
	{
		const auto v = termlib::parse::uint64(p.begin());
		EXPECT_EQ(v, c);
		c++;
	}

	EXPECT_EQ(c, count);
}

TEST_F(ParserIteratorTest, Iterator_WhileTuple)
{
	const std::size_t count{3};

	EXPECT_NO_THROW(builder_.start_tuple(count));
	for (std::size_t idx = 0; idx < count; ++idx)
	{
		EXPECT_NO_THROW(builder_.add_uint64(idx));
	}

	const auto buffer = builder_.buffer();
	const size_t size = builder_.index();

	TermParser parser({reinterpret_cast<const unsigned char *>(buffer), size});
	const auto tuple = termlib::parse::tuple(parser.begin());
	std::size_t c{0};

	auto it = tuple.begin();
	while (it != tuple.end())
	{
		const auto v = termlib::parse::uint64((*it).begin());
		EXPECT_EQ(v, c);
		c++;
		++it;
	}

	EXPECT_EQ(c, count);

	// check boundary
	for (int over = 10; over > 0; --over)
	{
		EXPECT_TRUE(it++ == tuple.end());
	}
}

TEST_F(ParserIteratorTest, Iterator_ForTuple)
{
	const std::size_t count{3};

	EXPECT_NO_THROW(builder_.start_tuple(count));
	for (std::size_t idx = 0; idx < count; ++idx)
	{
		EXPECT_NO_THROW(builder_.add_uint64(idx));
	}

	const auto buffer = builder_.buffer();
	const size_t size = builder_.index();

	TermParser parser({reinterpret_cast<const unsigned char *>(buffer), size});
	const auto tuple = termlib::parse::tuple(parser.begin());
	std::size_t c{0};
	for (auto p : tuple)
	{
		const auto v = termlib::parse::uint64(p.begin());
		EXPECT_EQ(v, c);
		c++;
	}

	EXPECT_EQ(c, count);
}

TEST_F(ParserIteratorTest, Iterator_WhileMap)
{
	const std::size_t count{3};

	EXPECT_NO_THROW(builder_.start_map(count));
	for (std::size_t idx = 0; idx < count; ++idx)
	{
		EXPECT_NO_THROW(builder_.add_uint64(idx));
		EXPECT_NO_THROW(builder_.add_uint64(idx));
	}

	const auto buffer = builder_.buffer();
	const size_t size = builder_.index();

	TermParser parser({reinterpret_cast<const unsigned char *>(buffer), size});
	const auto map = termlib::parse::map(parser.begin());
	std::size_t c{0};

	auto it = map.begin();
	while (it != map.end())
	{
		const auto k = termlib::parse::uint64(it++);
		const auto v = termlib::parse::uint64(it);
		EXPECT_EQ(k, c);
		EXPECT_EQ(v, c);
		++c;
		++it;
	}

	EXPECT_EQ(c, count);

	// check boundary
	for (int over = 10; over > 0; --over)
	{
		EXPECT_TRUE(it++ == map.end());
	}
}

TEST_F(ParserIteratorTest, Utf8_1)
{
	constexpr std::array<const unsigned char, 37> utf_string{131, 108, 0,  0,  0,  6,  98, 0,  0,  0,  48, 98, 0,
															 0,   0,   49, 98, 0,  0,  0,  50, 98, 0,  0,  0,  51,
															 98,  0,   0,  0,  52, 98, 0,  0,  0,  53, 106};

	TermParser parser({utf_string.data(), utf_string.size()});
	const auto it = parser.begin();
	std::string u8 = parse::u8str(it);
	EXPECT_STREQ(u8.c_str(), "012345");
}

TEST_F(ParserIteratorTest, Utf8_2)
{
	constexpr std::array<const unsigned char, 37> utf_string{131, 108, 0,  0,  0,  6,  98, 0,  0,  4,  79, 98, 0,
															 0,   4,   49, 98, 0,  0,  4,  59, 98, 0,  0,  4,  62,
															 98,  0,   0,  4,  58, 98, 0,  0,  4,  62, 106};

	TermParser parser({utf_string.data(), utf_string.size()});
	const auto it = parser.begin();
	std::string u8 = parse::u8str(it);
	EXPECT_STREQ(u8.c_str(), "яблоко");
}

TEST_F(ParserIteratorTest, Utf8_3)
{
	constexpr std::array<const unsigned char, 22> utf_string{
		131, 108, 0, 0, 0, 3, 98, 0, 0, 84, 80, 98, 0, 0, 53, 194, 98, 0, 0, 213, 92, 106};

	TermParser parser({utf_string.data(), utf_string.size()});
	const auto it = parser.begin();
	std::string u8 = parse::u8str(it);
	EXPECT_STREQ(u8.c_str(), "呐㗂한");
}

TEST_F(ParserIteratorTest, Utf8_4)
{
	constexpr std::array<const unsigned char, 12> utf_string{131, 108, 0, 0, 0, 1, 98, 0, 1, 3, 72, 106};

	TermParser parser({utf_string.data(), utf_string.size()});
	const auto it = parser.begin();
	std::string u8 = parse::u8str(it);
	EXPECT_STREQ(u8.c_str(), "𐍈");
}

TEST_F(ParserIteratorTest, MapParse)
{
	constexpr std::array<const unsigned char, 33> map_term{131, 116, 0,   0,   0,   2,   100, 0,   4,  104, 119,
														   105, 100, 106, 100, 0,   12,  108, 105, 99, 101, 110,
														   115, 101, 95,  105, 110, 102, 111, 107, 0,  1,   97};

	TermParser parser({map_term.data(), map_term.size()});
	TermParser map_parser = termlib::parse::map(parser.begin());

	bool hwid_check = false;
	bool license_check = false;
	auto it = map_parser.begin();
	while (it != map_parser.end())
	{
		const auto k = termlib::parse::atom(it++);
		if (k == "hwid")
		{
			const auto v = termlib::parse::str(it++);
			EXPECT_STREQ(v.c_str(), "");
			hwid_check = true;
		}
		else if (k == "license_info")
		{
			const auto v = termlib::parse::str(it++);
			EXPECT_STREQ(v.c_str(), "a");
			license_check = true;
		}
		else
		{
			EXPECT_TRUE(false);
		}
	}

	EXPECT_TRUE(hwid_check && license_check);
}

} // namespace termlib::tests
