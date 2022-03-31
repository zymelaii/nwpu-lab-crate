#include <iostream>

namespace metafloat
{

//! object series
template <int... Elements> struct series {
	static constexpr auto N = sizeof...(Elements);
};

//! function make_series
template <size_t N, size_t left, int default_value, int... Elements> struct make_series_t {
	using type = typename make_series_t<N, N - sizeof...(Elements) - 1, default_value, Elements..., default_value>::type;
};
template <size_t N, int default_value, int... Elements> struct make_series_t<N, 0, default_value, Elements...> {
	using type = series<Elements...>;
};
template <size_t N, int default_value, int... Elements>
using make_series = typename make_series_t<N, N, default_value, Elements...>::type;

//! function append
template <class Series, int element> struct append_t;
template <int... Elements, int element> struct append_t<series<Elements...>, element> {
	using type = series<Elements..., element>;
};
template <class Series, int element>
using append = typename append_t<Series, element>::type;

//! function reverse
template <class Series> struct reverse_t;
template <int front, int... Elements> struct reverse_t<series<front, Elements...>> {
	using type = append<typename reverse_t<series<Elements...>>::type, front>;
};
template <> struct reverse_t<series<>> {
	using type = series<>;
};
template <class Series>
using reverse = typename reverse_t<Series>::type;

//! function sseries
template <class Series> struct sseries_t;
template <int... Elements> struct sseries_t<series<Elements...>> {
	static constexpr char value[series<Elements...>::N + 1] = { ('0' + Elements)..., 0 };
};
template <class Series>
constexpr auto sseries = sseries_t<Series>::value;

//! function pop_front
template <class Series> struct pop_front;
template <int front, int... Elements> struct pop_front<series<front, Elements...>> {
	static constexpr auto value = front;
	using type = series<Elements...>;
};

//! function add
template <class RSeries1, class RSeries2, class RResult, int Carrier> struct add_t {
	using lhs = pop_front<RSeries1>;
	using rhs = pop_front<RSeries2>;
	static constexpr auto ans = Carrier + lhs::value + rhs::value;
	using type = typename add_t<typename lhs::type, typename rhs::type, append<RResult, ans % 10>, ans / 10>::type;
};
template <class RResult, int Carrier> struct add_t<series<>, series<>, RResult, Carrier> {
	using type = RResult;
};
template <class Series1, class Series2>
using add = reverse<typename add_t<reverse<Series1>, reverse<Series2>, series<>, 0>::type>;

//! function mul
template <class RSeries, int Multiplier, class RResult, int Carrier> struct mul_t {
	using lhs = pop_front<RSeries>;
	static constexpr auto ans = Carrier + lhs::value * Multiplier;
	using type = typename mul_t<typename lhs::type, Multiplier, append<RResult, ans % 10>, ans / 10>::type;
};
template <int Multiplier, class RResult, int Carrier> struct mul_t<series<>, Multiplier, RResult, Carrier> {
	using type = RResult;
};
template <class Series, int Multiplier>
using mul = reverse<typename mul_t<reverse<Series>, Multiplier, series<>, 0>::type>;

//! function div
template <class Series, int Divisor, class Result, int Remainder> struct div_t {
	using lhs = pop_front<Series>;
	static constexpr auto ans = Remainder * 10 + lhs::value;
	using type = typename div_t<typename lhs::type, Divisor, append<Result, ans / Divisor>, ans % Divisor>::type;
};
template <int Divisor, class Result, int Remainder> struct div_t<series<>, Divisor, Result, Remainder> {
	using type = Result;
};
template <class Series, int Divisor>
using div = typename div_t<Series, Divisor, series<>, 0>::type;

};

namespace mf = metafloat;

template <class PI, class Delta, int N> struct program_t {
	using delta = mf::div<mf::mul<Delta, N>, 2 * N + 1>;
	using pi = mf::add<PI, delta>;
	using type = typename program_t<pi, delta, N + 1>::type;
};
template <class PI, int N> struct program_t<PI, mf::make_series<PI::N, 0>, N> {
	using type = PI;
};
template <size_t N>
using program = typename program_t<mf::make_series<N, 3, 9>, mf::make_series<N, 6, 2>, 3>::type;

int main(int argc, char const *argv[])
{
	printf("3.%.16s", mf::sseries<program<20>>);
	return 0;
}
