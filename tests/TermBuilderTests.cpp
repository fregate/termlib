#include <gtest/gtest.h>

#include <termlib/Exception.h>
#include <termlib/TermBuilder.h>
#include <termlib/TermParser.h>

#include "ExpectThrowSystemError.h"

namespace termlib::tests
{

TEST(TermBuilder, MoveCtor)
{
	const std::string atom{"atom"};

	TermBuilder builder_rhs;
	builder_rhs.add_atom(atom);

	void * builder_buffer = nullptr;
	int builder_index = 0;
	EXPECT_NO_THROW([[maybe_unused]] const auto check = builder_rhs.buffer(); builder_buffer = builder_rhs.buffer();
					builder_index = builder_rhs.index(););

	TermBuilder builder{std::move(builder_rhs)};
	EXPECT_NO_THROW([[maybe_unused]] const auto check = builder_rhs.buffer(); EXPECT_EQ(check, nullptr));
	EXPECT_NO_THROW([[maybe_unused]] const auto check = builder.buffer(); EXPECT_EQ(builder.buffer(), builder_buffer);
					EXPECT_EQ(builder.index(), builder_index););
}

TEST(TermBuilder, MoveAssigment)
{
	const std::string atom{"atom"};

	TermBuilder builder_rhs;
	builder_rhs.add_atom(atom);

	void * builder_buffer = nullptr;
	int builder_index = 0;
	EXPECT_NO_THROW([[maybe_unused]] const auto check = builder_rhs.buffer(); builder_buffer = builder_rhs.buffer();
					builder_index = builder_rhs.index(););

	TermBuilder builder = std::move(builder_rhs);
	EXPECT_NO_THROW([[maybe_unused]] const auto check = builder_rhs.buffer(); EXPECT_EQ(check, nullptr));
	EXPECT_NO_THROW([[maybe_unused]] const auto check = builder.buffer(); EXPECT_EQ(builder.buffer(), builder_buffer);
					EXPECT_EQ(builder.index(), builder_index););
}

class TermBuilderTest : public ::testing::Test
{
protected:
	TermBuilder builder_;
};

TEST_F(TermBuilderTest, Atom)
{
	const std::string atom{"atom"};

	builder_.add_atom(atom);

	EXPECT_NO_THROW([[maybe_unused]] const auto check = builder_.buffer());
	const auto buffer = builder_.buffer();
	const size_t size = builder_.index();

	EXPECT_NE(buffer, nullptr);
	EXPECT_GT(size, 0);

	TermParser parser({reinterpret_cast<const unsigned char *>(buffer), size});
	const auto it = parser.begin();
	EXPECT_NO_THROW(const auto a = termlib::parse::atom(it); EXPECT_EQ(atom, a));
}

TEST_F(TermBuilderTest, AtomExceed128)
{
	const std::string atom{"012345678901234567890123456789012345678901234567890123456789"
						   "012345678901234567890123456789012345678901234567890123456789"
						   "012345678901234567890123456789012345678901234567890123456789"
						   "012345678901234567890123456789012345678901234567890123456789"
						   "012345678901234567890123456789012345678901234567890123456789"
						   "012345678901234567890123456789012345678901234567890123456789"};

	EXPECT_THROW_SYSTEM_ERROR_CODE(builder_.add_atom(atom), std::make_error_code(TermBuilderErrors::AtomTooLong));
}

TEST_F(TermBuilderTest, Int32)
{
	const std::int32_t val = std::numeric_limits<std::int32_t>::min();

	builder_.add_int32(val);

	EXPECT_NO_THROW([[maybe_unused]] const auto check = builder_.buffer());
	const auto buffer = builder_.buffer();
	const size_t size = builder_.index();

	EXPECT_NE(buffer, nullptr);
	EXPECT_GT(size, 0);

	TermParser parser({reinterpret_cast<const unsigned char *>(buffer), size});
	const auto it = parser.begin();
	EXPECT_NO_THROW(const auto a = termlib::parse::int32(it); EXPECT_EQ(val, a));
}

TEST_F(TermBuilderTest, UInt32)
{
	const std::uint32_t val = std::numeric_limits<std::uint32_t>::max();

	builder_.add_uint32(val);

	EXPECT_NO_THROW([[maybe_unused]] const auto check = builder_.buffer());
	const auto buffer = builder_.buffer();
	const size_t size = builder_.index();

	EXPECT_NE(buffer, nullptr);
	EXPECT_GT(size, 0);

	TermParser parser({reinterpret_cast<const unsigned char *>(buffer), size});
	const auto it = parser.begin();
	EXPECT_NO_THROW(const auto a = termlib::parse::uint32(it); EXPECT_EQ(val, a));
}

TEST_F(TermBuilderTest, Int64)
{
	const std::int64_t val = std::numeric_limits<std::int64_t>::min();

	builder_.add_int64(val);

	EXPECT_NO_THROW([[maybe_unused]] const auto check = builder_.buffer());
	const auto buffer = builder_.buffer();
	const size_t size = builder_.index();

	EXPECT_NE(buffer, nullptr);
	EXPECT_GT(size, 0);

	TermParser parser({reinterpret_cast<const unsigned char *>(buffer), size});
	const auto it = parser.begin();
	EXPECT_NO_THROW(const auto a = termlib::parse::int64(it); EXPECT_EQ(val, a));
}

TEST_F(TermBuilderTest, UInt64)
{
	const std::uint64_t val = std::numeric_limits<std::uint64_t>::max();

	builder_.add_uint64(val);

	EXPECT_NO_THROW([[maybe_unused]] const auto check = builder_.buffer());
	const auto buffer = builder_.buffer();
	const size_t size = builder_.index();

	EXPECT_NE(buffer, nullptr);
	EXPECT_GT(size, 0);

	TermParser parser({reinterpret_cast<const unsigned char *>(buffer), size});
	const auto it = parser.begin();
	EXPECT_NO_THROW(const auto a = termlib::parse::uint64(it); EXPECT_EQ(val, a));
}

TEST_F(TermBuilderTest, Double)
{
	const double val = 0.0;

	builder_.add_double(val);

	EXPECT_NO_THROW([[maybe_unused]] const auto check = builder_.buffer());
	const auto buffer = builder_.buffer();
	const size_t size = builder_.index();

	EXPECT_NE(buffer, nullptr);
	EXPECT_GT(size, 0);

	TermParser parser({reinterpret_cast<const unsigned char *>(buffer), size});
	const auto it = parser.begin();
	EXPECT_NO_THROW(const auto a = termlib::parse::real(it); EXPECT_EQ(val, a));
}

TEST_F(TermBuilderTest, ShortString)
{
	const std::string str{"short string"}; // less than MAXATOM

	builder_.add_string(str);

	EXPECT_NO_THROW([[maybe_unused]] const auto check = builder_.buffer());
	const auto buffer = builder_.buffer();
	const size_t size = builder_.index();

	EXPECT_NE(buffer, nullptr);
	EXPECT_GT(size, 0);

	TermParser parser({reinterpret_cast<const unsigned char *>(buffer), size});
	const auto it = parser.begin();
	EXPECT_NO_THROW(const auto a = termlib::parse::str(it); EXPECT_EQ(str, a));
}

TEST_F(TermBuilderTest, LongString)
{
	const std::string str(MAXATOMLEN * 3, '=');

	builder_.add_string(str);

	EXPECT_NO_THROW([[maybe_unused]] const auto check = builder_.buffer());
	const auto buffer = builder_.buffer();
	const size_t size = builder_.index();

	EXPECT_NE(buffer, nullptr);
	EXPECT_GT(size, 0);

	TermParser parser({reinterpret_cast<const unsigned char *>(buffer), size});
	const auto it = parser.begin();
	EXPECT_NO_THROW(const auto a = termlib::parse::str(it); EXPECT_EQ(str, a));
}

TEST_F(TermBuilderTest, EmptyString)
{
	const std::string str{};

	builder_.add_string(str);

	EXPECT_NO_THROW([[maybe_unused]] const auto check = builder_.buffer());
	const auto buffer = builder_.buffer();
	const size_t size = builder_.index();

	EXPECT_NE(buffer, nullptr);
	EXPECT_GT(size, 0);

	TermParser parser({reinterpret_cast<const unsigned char *>(buffer), size});
	const auto it = parser.begin();
	EXPECT_NO_THROW(const auto a = termlib::parse::str(it); EXPECT_EQ(str, a));
}

TEST_F(TermBuilderTest, Binary)
{
	std::array<char, MAXATOMLEN * 3> data;
	data.fill('=');

	builder_.add_binary(data.data(), data.size());

	EXPECT_NO_THROW([[maybe_unused]] const auto check = builder_.buffer());
	const auto buffer = builder_.buffer();
	const size_t size = builder_.index();

	EXPECT_NE(buffer, nullptr);
	EXPECT_GT(size, 0);

	TermParser parser({reinterpret_cast<const unsigned char *>(buffer), size});
	const auto it = parser.begin();

	std::array<char, MAXATOMLEN * 3> recv;
	EXPECT_NO_THROW(const auto len = termlib::parse::binary(it, recv.data(), recv.size()); EXPECT_EQ(len, data.size()));
	EXPECT_EQ(recv, data);
}

TEST_F(TermBuilderTest, BinaryNotEnoughSpace)
{
	std::array<char, MAXATOMLEN * 3> data{'='};

	builder_.add_binary(data.data(), data.size());

	EXPECT_NO_THROW([[maybe_unused]] const auto check = builder_.buffer());
	const auto buffer = builder_.buffer();
	const size_t size = builder_.index();

	EXPECT_NE(buffer, nullptr);
	EXPECT_GT(size, 0);

	TermParser parser({reinterpret_cast<const unsigned char *>(buffer), size});
	const auto it = parser.begin();

	std::array<char, MAXATOMLEN> recv;
	EXPECT_NO_THROW(const auto len = termlib::parse::binary(it, recv.data(), recv.size()); EXPECT_NE(len, recv.size());
					EXPECT_EQ(len, data.size()));
}

TEST_F(TermBuilderTest, BinaryBiggerBuffer)
{
	std::array<char, MAXATOMLEN> data{'='};

	builder_.add_binary(data.data(), data.size());

	EXPECT_NO_THROW([[maybe_unused]] const auto check = builder_.buffer());
	const auto buffer = builder_.buffer();
	const size_t size = builder_.index();

	EXPECT_NE(buffer, nullptr);
	EXPECT_GT(size, 0);

	TermParser parser({reinterpret_cast<const unsigned char *>(buffer), size});
	const auto it = parser.begin();

	std::array<char, MAXATOMLEN * 3> recv;
	EXPECT_NO_THROW(const auto len = termlib::parse::binary(it, recv.data(), recv.size()); EXPECT_EQ(len, data.size()));
	EXPECT_TRUE(memcmp(recv.data(), data.data(), data.size()) == 0);
}

TEST_F(TermBuilderTest, InvalidTerm_WithoutArity)
{
	const std::int32_t val = std::numeric_limits<std::int32_t>::min();

	EXPECT_NO_THROW(builder_.add_int32(val));
	EXPECT_THROW_SYSTEM_ERROR_CODE(builder_.add_int32(val), std::make_error_code(TermBuilderErrors::InvalidArity));
}

TEST_F(TermBuilderTest, InvalidTerm_NoElements)
{
	EXPECT_THROW_SYSTEM_ERROR_CODE(
		[[maybe_unused]] const auto buffer = builder_.buffer(),
		std::make_error_code(TermBuilderErrors::InvalidArity));
}

TEST_F(TermBuilderTest, ListArityError_TooMany)
{
	const std::int32_t val = std::numeric_limits<std::int32_t>::min();

	EXPECT_NO_THROW(builder_.start_list(2));
	EXPECT_NO_THROW(builder_.add_int32(val));
	EXPECT_NO_THROW(builder_.add_int32(val));
	EXPECT_THROW_SYSTEM_ERROR_CODE(builder_.add_int32(val), std::make_error_code(TermBuilderErrors::InvalidArity));
}

TEST_F(TermBuilderTest, ListArityError_TooLess)
{
	const std::int32_t val = std::numeric_limits<std::int32_t>::min();

	EXPECT_NO_THROW(builder_.start_list(2));
	EXPECT_NO_THROW(builder_.add_int32(val));

	EXPECT_THROW_SYSTEM_ERROR_CODE(
		[[maybe_unused]] const auto buffer = builder_.buffer(),
		std::make_error_code(TermBuilderErrors::InvalidArity));
}

TEST_F(TermBuilderTest, List)
{
	const std::int32_t val = std::numeric_limits<std::int32_t>::min();

	EXPECT_NO_THROW(builder_.start_list(2));
	EXPECT_NO_THROW(builder_.add_int32(val));
	EXPECT_NO_THROW(builder_.add_int32(val));

	EXPECT_NO_THROW([[maybe_unused]] const auto check = builder_.buffer());
	const auto buffer = builder_.buffer();
	const size_t size = builder_.index();

	EXPECT_NE(buffer, nullptr);
	EXPECT_GT(size, 0);

	TermParser parser({reinterpret_cast<const unsigned char *>(buffer), size});
	const auto it = parser.begin();

	const auto list = termlib::parse::list(it);
	auto list_it = list.begin();
	EXPECT_NO_THROW(const auto a = termlib::parse::int32(list_it); EXPECT_EQ(val, a));
	EXPECT_NO_THROW(++list_it);
	EXPECT_NO_THROW(const auto a = termlib::parse::int32(list_it); EXPECT_EQ(val, a));
}

TEST_F(TermBuilderTest, EmptyList)
{
	EXPECT_NO_THROW(builder_.start_list(0));

	EXPECT_NO_THROW([[maybe_unused]] const auto check = builder_.buffer());
	const auto buffer = builder_.buffer();
	const size_t size = builder_.index();

	EXPECT_NE(buffer, nullptr);
	EXPECT_GT(size, 0);

	TermParser parser({reinterpret_cast<const unsigned char *>(buffer), size});
	const auto it = parser.begin();

	const auto list = termlib::parse::list(it);
	const auto list_it = list.begin();
	EXPECT_TRUE(list_it == list.end());
}

TEST_F(TermBuilderTest, Tuple)
{
	const std::int32_t val = -1;

	EXPECT_NO_THROW(builder_.start_tuple(2));
	EXPECT_NO_THROW(builder_.add_int32(val));
	EXPECT_NO_THROW(builder_.add_int32(val));

	EXPECT_NO_THROW([[maybe_unused]] const auto check = builder_.buffer());
	const auto buffer = builder_.buffer();
	const size_t size = builder_.index();

	EXPECT_NE(buffer, nullptr);
	EXPECT_GT(size, 0);

	TermParser parser({reinterpret_cast<const unsigned char *>(buffer), size});
	const auto it = parser.begin();

	const auto tuple = termlib::parse::tuple(it);
	auto tuple_it = tuple.begin();
	EXPECT_NO_THROW(const auto a = termlib::parse::int32(tuple_it); EXPECT_EQ(val, a));
	EXPECT_NO_THROW(++tuple_it);
	EXPECT_NO_THROW(const auto a = termlib::parse::int32(tuple_it); EXPECT_EQ(val, a));
}

TEST_F(TermBuilderTest, EmptyTuple)
{
	EXPECT_NO_THROW(builder_.start_tuple(0));

	EXPECT_NO_THROW([[maybe_unused]] const auto check = builder_.buffer());
	const auto buffer = builder_.buffer();
	const size_t size = builder_.index();

	EXPECT_NE(buffer, nullptr);
	EXPECT_GT(size, 0);

	TermParser parser({reinterpret_cast<const unsigned char *>(buffer), size});
	const auto it = parser.begin();
	const auto tuple = termlib::parse::tuple(it);

	const auto tuple_it = tuple.begin();
	EXPECT_TRUE(tuple_it == tuple.end());
}

TEST_F(TermBuilderTest, Map)
{
	const std::string key = "key";
	const std::int32_t val = 1000;

	EXPECT_NO_THROW(builder_.start_map(1));
	EXPECT_NO_THROW(builder_.add_string(key));
	EXPECT_NO_THROW(builder_.add_int32(val));

	EXPECT_NO_THROW([[maybe_unused]] const auto check = builder_.buffer());
	const auto buffer = builder_.buffer();
	const size_t size = builder_.index();

	EXPECT_NE(buffer, nullptr);
	EXPECT_GT(size, 0);

	TermParser parser({reinterpret_cast<const unsigned char *>(buffer), size});
	const auto it = parser.begin();

	const auto map = termlib::parse::map(it);
	auto map_it = map.begin();
	EXPECT_NO_THROW(const auto k = termlib::parse::str(map_it++); EXPECT_EQ(key, k));
	EXPECT_NO_THROW(const auto v = termlib::parse::int32(map_it); EXPECT_EQ(val, v));
}

TEST_F(TermBuilderTest, EmptyMap)
{
	EXPECT_NO_THROW(builder_.start_map(0));

	EXPECT_NO_THROW([[maybe_unused]] const auto check = builder_.buffer());
	const auto buffer = builder_.buffer();
	const size_t size = builder_.index();

	EXPECT_NE(buffer, nullptr);
	EXPECT_GT(size, 0);

	TermParser parser({reinterpret_cast<const unsigned char *>(buffer), size});
	const auto it = parser.begin();
	const auto map = termlib::parse::map(it);
	const auto map_it = map.begin();
	EXPECT_TRUE(map_it == map.end());
}

TEST_F(TermBuilderTest, InvalidMap)
{
	const std::string key = "key";
	EXPECT_NO_THROW(builder_.start_map(1));
	EXPECT_NO_THROW(builder_.add_string(key));

	EXPECT_THROW_SYSTEM_ERROR_CODE(
		[[maybe_unused]] const auto check = builder_.buffer(),
		std::make_error_code(TermBuilderErrors::InvalidArity));
}

TEST_F(TermBuilderTest, Compound)
{
	const double val = -1.23;
	const auto arity = 3;

	// outer tuple
	EXPECT_NO_THROW(builder_.start_tuple(arity));
	EXPECT_NO_THROW(builder_.add_double(val));

	EXPECT_NO_THROW(builder_.start_list(arity));
	{
		EXPECT_NO_THROW(builder_.add_double(val));
		EXPECT_NO_THROW(builder_.start_tuple(arity));
		{
			EXPECT_NO_THROW(builder_.add_double(val));
			EXPECT_NO_THROW(builder_.add_double(val));
			EXPECT_NO_THROW(builder_.add_double(val));
		}
		EXPECT_NO_THROW(builder_.add_double(val));
	}
	EXPECT_NO_THROW(builder_.add_double(val));

	EXPECT_NO_THROW([[maybe_unused]] const auto check = builder_.buffer());
	const auto buffer = builder_.buffer();
	const size_t size = builder_.index();

	EXPECT_NE(buffer, nullptr);
	EXPECT_GT(size, 0);

	TermParser parser({reinterpret_cast<const unsigned char *>(buffer), size});
	const auto it = parser.begin();

	auto outer_tuple = termlib::parse::tuple(it);
	{
		auto outer_tuple_it = outer_tuple.begin();
		EXPECT_NO_THROW(const auto a = termlib::parse::real(outer_tuple_it); EXPECT_EQ(val, a));

		auto inner_list = termlib::parse::list(++outer_tuple_it);
		{
			auto inner_list_it = inner_list.begin();
			EXPECT_NO_THROW(const auto a = termlib::parse::real(inner_list_it); EXPECT_EQ(val, a));

			auto inner_tuple = termlib::parse::tuple(++inner_list_it);
			{
				auto inner_tuple_it = inner_tuple.begin();
				EXPECT_NO_THROW(const auto a = termlib::parse::real(inner_tuple_it); EXPECT_EQ(val, a));
				EXPECT_NO_THROW(const auto a = termlib::parse::real(++inner_tuple_it); EXPECT_EQ(val, a));
				EXPECT_NO_THROW(const auto a = termlib::parse::real(++inner_tuple_it); EXPECT_EQ(val, a));
			}

			EXPECT_NO_THROW(const auto a = termlib::parse::real(++inner_list_it); EXPECT_EQ(val, a));
		}
	}
}

TEST_F(TermBuilderTest, RealCase_1)
{
	builder_.start_tuple(2);
	builder_.add_atom("error");
	builder_.start_tuple(2);
	builder_.add_int32(1);
	builder_.add_string("text");

	EXPECT_NO_THROW([[maybe_unused]] const auto check = builder_.buffer());
	const auto buffer = builder_.buffer();
	const size_t size = builder_.index();

	EXPECT_NE(buffer, nullptr);
	EXPECT_GT(size, 0);

	TermParser parser({reinterpret_cast<const unsigned char *>(buffer), size});
	const auto it = parser.begin();

	auto outer_tuple = termlib::parse::tuple(it);
	{
		auto outer_tuple_it = outer_tuple.begin();
		EXPECT_NO_THROW(const auto a = termlib::parse::atom(outer_tuple_it); EXPECT_EQ(a, "error"));
		auto inner_tuple = termlib::parse::tuple(++outer_tuple_it);
		{
			auto inner_tuple_it = inner_tuple.begin();
			EXPECT_NO_THROW(const auto a = termlib::parse::int32(inner_tuple_it); EXPECT_EQ(1, a));
			EXPECT_NO_THROW(const auto a = termlib::parse::str(++inner_tuple_it); EXPECT_EQ("text", a));
		}
	}
}

} // namespace termlib::tests
