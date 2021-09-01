#include "basics.hpp"

// Transform

template<class, template<class> class>
struct TransformT;

template<class List, template<class> class Func>
using Transform = typename TransformT<List, Func>::Type;

template<template<class...> class List, template<class> class Func, class... Ts>
struct TransformT<List<Ts...>, Func> : PushFrontT<
											Transform<PopFront<List<Ts...>>, Func>, 
											typename Func<Front<List<Ts...>>>::Type>
{
};

template<template<class...> class List, template<class> class Func>
struct TransformT<List<>, Func>
{
	using Type = List<>;
};

template<class T>
struct AddPointer
{
	using Type = T*;
};

static_assert(std::is_same_v<Transform<TypeList<>, AddPointer>, TypeList<>>, "");
static_assert(std::is_same_v<Transform<TypeList<int, double>, AddPointer>, TypeList<int*, double*>>, "");

// LargerType

template<class T1, class T2>
struct LargerTypeT 
{
	using Type = IfThenElse<(sizeof(T2) > sizeof(T1)), T2, T1>;
};

template<class T1, class T2>
using LargerType = typename LargerTypeT<T1, T2>::Type;

static_assert(std::is_same_v<LargerType<char, short>, short>, "");
static_assert(std::is_same_v<LargerType<short, double>, double>, "");

// LargerValue

template<class, class>
struct LargerValueT;

template<class T, T Value1, T Value2>
struct LargerValueT<Value<T, Value1>, Value<T, Value2>>
{
	using Type = Value<T, (Value1 < Value2 ? Value2 : Value1)>;
};

// Accumulate

template<class, template<class, class> class, class>
struct AccumulateT;

template<template<class...> class List, template<class, class> class Func, class Init>
struct AccumulateT<List<>, Func, Init>
{
	using Type = Init;
};

template<template<class...> class List, template<class, class> class Func, class Init, class... Ts>
struct AccumulateT<List<Ts...>, Func, Init> : AccumulateT<PopFront<List<Ts...>>, Func, typename Func<Init, Front<List<Ts...>>>::Type>
{};

template<class List, template<class, class> class Func, class Init>
using Accumulate = typename AccumulateT<List, Func, Init>::Type;

static_assert(std::is_same_v<Accumulate<TypeList<char, short>, LargerTypeT, char>, short>, "");
static_assert(std::is_same_v<Accumulate<TypeList<int, long long, short>, LargerTypeT, char>, long long>, "");
static_assert(std::is_same_v<Accumulate<TypeList<Value<int, 1>, Value<int, 2>, Value<int, 3>>, LargerValueT, Value<int, 0>>, Value<int, 3>>, "");
static_assert(std::is_same_v<Accumulate<ValueList<int, 1, 2, 3>, LargerValueT, Value<int, 0>>, Value<int, 3>>, "");

// NthElement

template<class, int>
struct NthElementT;

template<template<class...> class List, class Head, class... Tail>
struct NthElementT<List<Head, Tail...>, 0> 
{
	using Type = Head;
};

template<template<class...> class List, int Num, class Head, class... Tail>
struct NthElementT<List<Head, Tail...>, Num> : NthElementT<List<Tail...>, Num - 1> 
{};

template<class List, int Num>
using NthElement = typename NthElementT<List, Num>::Type;

static_assert(std::is_same_v<NthElement<ValueList<int, 0, 1, 2, 3>, 0>, Value<int, 0>>, "");
static_assert(std::is_same_v<NthElement<ValueList<int, 0, 1, 2, 3>, 1>, Value<int, 1>>, "");
static_assert(std::is_same_v<NthElement<ValueList<int, 0, 1, 2, 3>, 3>, Value<int, 3>>, "");
static_assert(std::is_same_v<NthElement<TypeList<int, bool, double, short>, 2>, double>, "");

// LessValue

template<class, class>
struct LessValue;

template<class T, T Value1, T Value2>
struct LessValue<Value<T, Value1>, Value<T, Value2>>
{
	static constexpr bool value = Value1 < Value2;
};

static_assert(LessValue<Value<int, 2>, Value<int, 3>>::value, "");
static_assert(!LessValue<Value<int, 3>, Value<int, 3>>::value, "");
static_assert(!LessValue<Value<int, 4>, Value<int, 3>>::value, "");

// GreaterValue

template<class, class>
struct GreaterValue;

template<class T, T Value1, T Value2>
struct GreaterValue<Value<T, Value1>, Value<T, Value2>>
{
	static constexpr bool value = Value1 > Value2;
};

static_assert(!GreaterValue<Value<int, 2>, Value<int, 3>>::value, "");
static_assert(!GreaterValue<Value<int, 3>, Value<int, 3>>::value, "");
static_assert(GreaterValue<Value<int, 4>, Value<int, 3>>::value, "");

// LowerBound

template<class, class, template<class, class> class CompFunc = GreaterValue>
struct LowerBound;

template<class, class, int, int, template<class, class> class>
struct LowerBoundRec;

template<template<class...> class List, class Sought, int Ind, template<class, class> class CompFunc, class... Ts>
struct LowerBoundRec<List<Ts...>, Sought, Ind, Ind, CompFunc>
{
	static constexpr int value = Ind;
};

template<template<class...> class List, class Sought, int Lower, int Upper, template<class, class> class CompFunc, class... Ts>
struct LowerBoundRec<List<Ts...>, Sought, Lower, Upper, CompFunc>
{
	static constexpr int Mid = Lower + (Upper - Lower) / 2;
	static constexpr int value = IfThenElse<
		CompFunc<Sought, NthElement<List<Ts...>, Mid>>::value,
		LowerBoundRec<List<Ts...>, Sought, Mid + 1, Upper, CompFunc>,
		LowerBoundRec<List<Ts...>, Sought, Lower, Mid, CompFunc>
	>::value;
};

template<template<class...> class List, class Sought, template<class, class> class CompFunc, class... Ts>
struct LowerBound<List<Ts...>, Sought, CompFunc>
{
	static constexpr int value = LowerBoundRec<List<Ts...>, Sought, 0, sizeof...(Ts), CompFunc>::value;
};

static_assert(LowerBound<ValueList<int, 1, 2, 3, 4, 5>, Value<int, 0>, GreaterValue>::value == 0, "");
static_assert(LowerBound<ValueList<int, 1, 2, 3, 4, 5>, Value<int, 0>>::value == 0, "");
static_assert(LowerBound<ValueList<int, 1, 2, 3, 4, 5>, Value<int, 3>>::value == 2, "");
static_assert(LowerBound<ValueList<int, 1, 2, 4, 5>, Value<int, 3>, GreaterValue>::value == 2, "");
static_assert(LowerBound<ValueList<int, 1, 2, 4, 5>, Value<int, 6>, GreaterValue>::value == 4, "");

// InsersionSort

template<class>
struct InsersionSort;


//static_assert(std::is_same_v<InsersionSort<ValueList<int, 4, 1, 3, 2>>, ValueList<int, 1, 2, 3, 4>>, "");