#pragma once

#include <exception>
#include <system_error>

namespace termlib
{

enum class TermBuilderErrors
{
	InvalidArity,
	AtomTooLong,
};

enum class TermParserErrors
{
	NullBlock,
	TypeError,
};

class parser_error : public std::system_error
{
public:
	parser_error(std::error_code ec)
		: std::system_error(ec)
		, expected_{}
		, given_{}
	{
	}

	parser_error(std::error_code ec, std::string expected, int given)
		: std::system_error(ec)
		, expected_{std::move(expected)}
		, given_{given}
	{
		err_ = "invalid type code for parsing function. expected: " + expected_ + ", given: " + std::to_string(given_);
	}

	[[nodiscard]] const std::string & expected() const
	{
		return expected_;
	}

	[[nodiscard]] int given() const
	{
		return given_;
	}

	[[nodiscard]] const char * what() const noexcept override
	{
		return err_.c_str();
	}

private:
	std::string expected_;
	std::string err_;
	int given_;
};

} // namespace termlib

namespace
{

struct TermBuilderErrorsCategory : std::error_category
{
	[[nodiscard]] const char * name() const noexcept override
	{
		return "TermBuilder";
	}

	[[nodiscard]] std::string message(int ev) const override
	{
		switch (static_cast<termlib::TermBuilderErrors>(ev))
		{
		case termlib::TermBuilderErrors::InvalidArity:
			return "invalid arity for term";

		case termlib::TermBuilderErrors::AtomTooLong:
			return "atom exceeds 256 symbols";

		default:
			return "unknown error code: " + std::to_string(ev);
		}
	}
};
const TermBuilderErrorsCategory theTermBuilderErrorsCategory{};

struct TermParserErrorsCategory : std::error_category
{
	[[nodiscard]] const char * name() const noexcept override
	{
		return "TermParser";
	}

	[[nodiscard]] std::string message(int ev) const override
	{
		switch (static_cast<termlib::TermParserErrors>(ev))
		{
		case termlib::TermParserErrors::NullBlock:
			return "trying to parse nullptr block";

		case termlib::TermParserErrors::TypeError:
			return "invalid type code for parsing function";

		default:
			return "unknown error code: " + std::to_string(ev);
		}
	}
};
const TermParserErrorsCategory theTermParserErrorsCategory{};

} // namespace

namespace std
{

template <>
struct is_error_code_enum<termlib::TermBuilderErrors> : true_type
{
};
[[nodiscard]] error_code make_error_code(termlib::TermBuilderErrors error);

template <>
struct is_error_code_enum<termlib::TermParserErrors> : true_type
{
};
[[nodiscard]] error_code make_error_code(termlib::TermParserErrors error);

} // namespace std
