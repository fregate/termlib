#include <gtest/gtest.h>

#include <termlib/Exception.h>
#include <termlib/TermParser.h>

#include "ExpectThrowParserError.h"

namespace termlib::tests
{

TEST(TermParserTest, NullBlock)
{
	TermParser::BinaryBlock::pointer data = nullptr;
	EXPECT_THROW_PARSER_ERROR_CODE(
		TermParser(TermParser::BinaryBlock{data, 100}),
		std::make_error_code(TermParserErrors::NullBlock));
}

} // namespace termlib::tests
