#include <gtest/gtest.h>

#include <termlib/TermBuilder.h>
#include <termlib/TermParser.h>

namespace termlib::tests
{

class TermParserTest : public ::testing::Test
{
protected:
	TermBuilder builder_;
};

TEST_F(TermParserTest, Iterator_WhileList)
{
	const std::size_t count{3};

	EXPECT_NO_THROW(builder_.start_list(count));
	for (std::size_t idx = 0; idx < count; ++idx) {
		EXPECT_NO_THROW(builder_.add_uint64(idx));
	}

	const auto buffer = builder_.buffer();
	const size_t size = builder_.index();

	TermParser parser({reinterpret_cast<const unsigned char*>(buffer), size});
	const auto list = termlib::parse::complex(parser.begin());
	std::size_t c{0};

	auto it = list.begin();
	while (it != list.end()) {
		const auto v = termlib::parse::uint64((*it).begin());
		EXPECT_EQ(v, c);
		c++;
		++it;
	}

	EXPECT_EQ(c, count);

	// check boundary
	for (int over = 10; over > 0; --over) {
		EXPECT_TRUE(it++ == list.end());
	}
}

TEST_F(TermParserTest, Iterator_ForList)
{
	const std::size_t count{3};

	EXPECT_NO_THROW(builder_.start_list(count));
	for (std::size_t idx = 0; idx < count; ++idx) {
		EXPECT_NO_THROW(builder_.add_uint64(idx));
	}

	const auto buffer = builder_.buffer();
	const size_t size = builder_.index();

	TermParser parser({reinterpret_cast<const unsigned char*>(buffer), size});
	const auto list = termlib::parse::complex(parser.begin());
	std::size_t c{0};
	for (auto p : list) {
		const auto v = termlib::parse::uint64(p.begin());
		EXPECT_EQ(v, c);
		c++;
	}

	EXPECT_EQ(c, count);
}

TEST_F(TermParserTest, Iterator_WhileTuple)
{
	const std::size_t count{3};

	EXPECT_NO_THROW(builder_.start_tuple(count));
	for (std::size_t idx = 0; idx < count; ++idx) {
		EXPECT_NO_THROW(builder_.add_uint64(idx));
	}

	const auto buffer = builder_.buffer();
	const size_t size = builder_.index();

	TermParser parser({reinterpret_cast<const unsigned char*>(buffer), size});
	const auto tuple = termlib::parse::complex(parser.begin());
	std::size_t c{0};

	auto it = tuple.begin();
	while (it != tuple.end()) {
		const auto v = termlib::parse::uint64((*it).begin());
		EXPECT_EQ(v, c);
		c++;
		++it;
	}

	EXPECT_EQ(c, count);

	// check boundary
	for (int over = 10; over > 0; --over) {
		EXPECT_TRUE(it++ == tuple.end());
	}
}

TEST_F(TermParserTest, Iterator_ForTuple)
{
	const std::size_t count{3};

	EXPECT_NO_THROW(builder_.start_tuple(count));
	for (std::size_t idx = 0; idx < count; ++idx) {
		EXPECT_NO_THROW(builder_.add_uint64(idx));
	}

	const auto buffer = builder_.buffer();
	const size_t size = builder_.index();

	TermParser parser({reinterpret_cast<const unsigned char*>(buffer), size});
	const auto tuple = termlib::parse::complex(parser.begin());
	std::size_t c{0};
	for (auto p : tuple) {
		const auto v = termlib::parse::uint64(p.begin());
		EXPECT_EQ(v, c);
		c++;
	}

	EXPECT_EQ(c, count);
}

TEST_F(TermParserTest, Iterator_WhileMap)
{
	const std::size_t count{3};

	EXPECT_NO_THROW(builder_.start_map(count));
	for (std::size_t idx = 0; idx < count; ++idx) {
		EXPECT_NO_THROW(builder_.add_uint64(idx));
		EXPECT_NO_THROW(builder_.add_uint64(idx));
	}

	const auto buffer = builder_.buffer();
	const size_t size = builder_.index();

	TermParser parser({reinterpret_cast<const unsigned char*>(buffer), size});
	const auto map = termlib::parse::complex(parser.begin());
	std::size_t c{0};

	auto it = map.begin();
	while (it != map.end()) {
		const auto k =termlib::parse::uint64(it++);
		const auto v =termlib::parse::uint64(it);
		EXPECT_EQ(k, c);
		EXPECT_EQ(v, c);
		++c;
		++it;
	}

	EXPECT_EQ(c, count);

	// check boundary
	for (int over = 10; over > 0; --over) {
		EXPECT_TRUE(it++ == map.end());
	}
}

}  // namespace termlib::tests
