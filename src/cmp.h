#include <algorithm>
#include <initializer_list>

namespace termlib
{

template <typename _Type>
struct operand
{
	constexpr operand(std::initializer_list<_Type> && p)
		: params_(std::move(p))
	{
	}

	[[nodiscard]] virtual bool check(_Type val) const = 0;

	std::initializer_list<_Type> params_;
};

template <typename _Type>
struct in : public operand<_Type>
{
	using operand<_Type>::params_;

	constexpr in(std::initializer_list<_Type> list)
		: operand<_Type>(std::move(list))
	{
	}

	[[nodiscard]] constexpr bool check(_Type val) const override
	{
		return std::find(params_.begin(), params_.end(), val) != params_.end();
	}
};

// TODO add compile-time checks for operand (derived from 'operand')
template <typename _Type, typename _Op>
struct is
{
	is(_Type v, _Op op)
		: v_{v}
		, op_{std::move(op)} {};

	_Type v_;
	_Op op_;

	[[nodiscard]] constexpr operator bool()
	{
		return op_.check(v_);
	}
};

} // namespace termlib
