#include <termlib/Exception.h>

namespace std
{

error_code make_error_code(termlib::TermBuilderErrors e)
{
	return {static_cast<int>(e), theTermBuilderErrorsCategory};
}

error_code make_error_code(termlib::TermParserErrors e)
{
	return {static_cast<int>(e), theTermParserErrorsCategory};
}

} // namespace std
