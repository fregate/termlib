#include <gtest/gtest.h>

#include <erlterm/read.hpp>

namespace erlterm::tests
{

TEST(Read, EmptyBuffer)
{
	std::array<std::uint8_t, 0> b{};
	int a{};
	const auto err = read_term(a, b);
	EXPECT_NE(err.ec, glz::error_code::none);
	EXPECT_EQ(err.ec, glz::error_code::no_read_input);

	const auto ret = read_term<int>(b);
	EXPECT_FALSE(ret);
}

}
