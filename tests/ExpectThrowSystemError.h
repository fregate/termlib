#pragma once

#define EXPECT_THROW_SYSTEM_ERROR(statement) EXPECT_THROW(statement, std::system_error)

#define EXPECT_THROW_SYSTEM_ERROR_CODE(statement, expected_code) \
	try \
	{ \
		statement; \
		FAIL() << "Expected: " #statement " throws an exception of type std::system_error.\n" \
				  "  Actual: it throws nothing."; \
	} \
	catch (const std::system_error & e) \
	{ \
		EXPECT_EQ(expected_code, e.code()); \
	} \
	catch (...) \
	{ \
		FAIL() << "Expected: " #statement " throws an exception of type std::system_error.\n" \
				  "  Actual: it throws a different type."; \
	}
