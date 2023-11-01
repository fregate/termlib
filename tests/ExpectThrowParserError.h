#pragma once

#define EXPECT_THROW_PARSER_ERROR(statement) EXPECT_THROW(statement, termlib::parser_error)

#define EXPECT_THROW_PARSER_ERROR_CODE(statement, expected_code) \
	try \
	{ \
		statement; \
		FAIL() << "Expected: " #statement " throws an exception of type termlib::parser_error.\n" \
				  "  Actual: it throws nothing."; \
	} \
	catch (const termlib::parser_error & e) \
	{ \
		EXPECT_EQ(expected_code, e.code()); \
	} \
	catch (...) \
	{ \
		FAIL() << "Expected: " #statement " throws an exception of type termlib::parser_error.\n" \
				  "  Actual: it throws a different type."; \
	}

#define EXPECT_THROW_PARSER_ERROR_CODE_TYPES(statement, expected_code, expect_type, given_type) \
	try \
	{ \
		statement; \
		FAIL() << "Expected: " #statement " throws an exception of type termlib::parser_error.\n" \
				  "  Actual: it throws nothing."; \
	} \
	catch (const termlib::parser_error & e) \
	{ \
		EXPECT_EQ(expected_code, e.code()); \
		EXPECT_EQ(expect_type, e.expected()); \
		EXPECT_EQ(given_type, e.given()); \
	} \
	catch (...) \
	{ \
		FAIL() << "Expected: " #statement " throws an exception of type termlib::parser_error.\n" \
				  "  Actual: it throws a different type."; \
	}
