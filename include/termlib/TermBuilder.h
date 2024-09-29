#pragma once

#include <ei.h>

#include <cstdint>
#include <list>
#include <string>

namespace termlib
{

class TermBuilder final
{
public:
	TermBuilder();
	TermBuilder(const TermBuilder &) = delete;
	TermBuilder(TermBuilder &&);
	~TermBuilder();

	TermBuilder & operator=(TermBuilder &) = delete;
	TermBuilder & operator=(TermBuilder &&);

	[[nodiscard]] char * buffer() const;
	[[nodiscard]] int index() const;

	[[nodiscard]] bool empty() const;

	void add_atom(const std::string & atom);
	void add_int32(std::int32_t value);
	void add_uint32(std::uint32_t value);
	void add_int64(std::int64_t value);
	void add_uint64(std::uint64_t value);
	void add_binary(const void * buff, std::size_t len);
	void add_string(const std::string & value);
	void add_double(double value);

	void start_list(std::size_t arity);
	void start_map(std::size_t arity);
	void start_tuple(std::size_t arity);

private:
	void update_arity();

private:
	ei_x_buff buff_;

	enum class ComplexStruct
	{
		None,
		List,
		Map,
		Tuple,
	};

	std::list<std::pair<std::size_t, ComplexStruct>> arities_{
		{1, ComplexStruct::None}}; // only one term can be added, for complex data have to use lists, maps or tuples
};

} // namespace termlib
