#include <tuple>
#include <type_traits>

// TypeList

template<class...>
struct TypeList {};

// ValueList

template<class T, T Val>
struct Value
{
	static constexpr T value = Val;
};

template<class T, T... Values>
using ValueList = TypeList<Value<int, Values>...>;

// ToTypeList

template<class>
struct ToTypeListT;

template<template<class...> class List, class... Ts>
struct ToTypeListT<List<Ts...>>
{
	using Type = TypeList<Ts...>;
};

template<class List>
using ToTypeList = typename ToTypeListT<List>::Type;

static_assert(std::is_same_v<ToTypeList<TypeList<int>>, TypeList<int>>, "");
static_assert(std::is_same_v<ToTypeList<TypeList<int, double>>, TypeList<int, double>>, "");
static_assert(std::is_same_v<ToTypeList<std::tuple<int>>, TypeList<int>>, "");
static_assert(std::is_same_v<ToTypeList<std::tuple<int, double>>, TypeList<int, double>>, "");

// FromTypeList

template<class, template<class...> class>
struct FromTypeListT;

template<template<class...> class To, class... Ts1>
struct FromTypeListT<TypeList<Ts1...>, To>
{
	using Type = To<Ts1...>;
};

template<class List, template<class...> class To>
using FromTypeList = typename FromTypeListT<List, To>::Type;

static_assert(std::is_same_v<FromTypeList<TypeList<>, std::tuple>, std::tuple<>>, "");
static_assert(std::is_same_v<FromTypeList<TypeList<int>, std::tuple>, std::tuple<int>>, "");
static_assert(std::is_same_v<FromTypeList<TypeList<int>, TypeList>, TypeList<int>>, "");
static_assert(std::is_same_v<FromTypeList<TypeList<int, double>, std::tuple>, std::tuple<int, double>>, "");
static_assert(std::is_same_v<FromTypeList<TypeList<int, double>, TypeList>, TypeList<int, double>>, "");

// Front

template<class...>
struct FrontT;

template<template<class...> class List, class Head, class... Tail>
struct FrontT<List<Head, Tail...>>
{
	using Type = Head;
};

template<class List>
using Front = typename FrontT<List>::Type;

static_assert(std::is_same_v<Front<TypeList<int, bool, double>>, int>, "");
static_assert(Front<ValueList<int, 1, 2, 3>>::value == 1, "");

// PopFront

template<class>
struct PopFrontT;

template<template<class...> class List, class Head, class... Tail>
struct PopFrontT<List<Head, Tail...>>
{
	using Type = List<Tail...>;
};

template<class List>
using PopFront = typename PopFrontT<List>::Type;

static_assert(std::is_same_v<PopFront<TypeList<int, bool, double>>, TypeList<bool, double>>, "");
static_assert(std::is_same_v<PopFront<TypeList<int>>, TypeList<>>, "");
static_assert(std::is_same_v<PopFront<ValueList<int, 1, 2, 3>>, ValueList<int, 2, 3>>, "");

// PushBack

template<class, class>
struct PushBackT;

template<template<class...> class List, class New, class... Ts>
struct PushBackT<List<Ts...>, New>
{
	using Type = List<Ts..., New>;
};

template<template<class...> class List, class New>
struct PushBackT<List<>, New>
{
	using Type = List<New>;
};

template<class List, class New>
using PushBack = typename PushBackT<List, New>::Type;

static_assert(std::is_same_v<PushBack<TypeList<int, bool>, double>, TypeList<int, bool, double>>, "");
static_assert(std::is_same_v<PushBack<TypeList<>, double>, TypeList<double>>, "");
static_assert(std::is_same_v<PushBack<std::tuple<int, bool>, double>, std::tuple<int, bool, double>>, "");
static_assert(std::is_same_v<PushBack<ValueList<int, 1, 2>, Value<int, 3>>, ValueList<int, 1, 2, 3>>, "");

// PushFront

template<class, class>
struct PushFrontT;

template<template<class...> class List, class New, class... Ts>
struct PushFrontT<List<Ts...>, New>
{
	using Type = List<New, Ts...>;
};

template<template<class...> class List, class New>
struct PushFrontT<List<>, New>
{
	using Type = List<New>;
};

template<class List, class New>
using PushFront = typename PushFrontT<List, New>::Type;

static_assert(std::is_same_v<PushFront<TypeList<int, bool>, double>, TypeList<double, int, bool>>, "");
static_assert(std::is_same_v<PushFront<TypeList<>, double>, TypeList<double>>, "");
static_assert(std::is_same_v<PushFront<std::tuple<int, bool>, double>, std::tuple<double, int, bool>>, "");
static_assert(std::is_same_v<PushFront<ValueList<int, 1, 2>, Value<int, 3>>, ValueList<int, 3, 1, 2>>, "");

// IsEmpty

template<class List>
struct IsEmpty;

template<template<class...> class List>
struct IsEmpty<List<>> {
	constexpr static bool value = true;
};

template<template<class...> class List, class... Ts>
struct IsEmpty<List<Ts...>> {
	constexpr static bool value = false;
};

static_assert(IsEmpty<TypeList<int>>::value == false, "");
static_assert(IsEmpty<TypeList<int, double>>::value == false, "");
static_assert(IsEmpty<TypeList<>>::value == true, "");
static_assert(IsEmpty<std::tuple<int>>::value == false, "");
static_assert(IsEmpty<std::tuple<int, double>>::value == false, "");
static_assert(IsEmpty<std::tuple<>>::value == true, "");
static_assert(IsEmpty<ValueList<int, 1>>::value == false, "");
static_assert(IsEmpty<ValueList<int>>::value == true, "");

// Reverse

template<class List>
struct ReverseT;

template<class List>
using Reverse = typename ReverseT<List>::Type;

template<template<class...> class List, class... Ts>
struct ReverseT<List<Ts...>> : PushBackT<Reverse<PopFront<List<Ts...>>>, Front<List<Ts...>>>
{};

template<template<class...> class List>
struct ReverseT<List<>> 
{
	using Type = List<>;
};

static_assert(std::is_same_v<Reverse<TypeList<>>, TypeList<>>, "");
static_assert(std::is_same_v<Reverse<TypeList<int>>, TypeList<int>>, "");
static_assert(std::is_same_v<Reverse<TypeList<int, double>>, TypeList<double, int>>, "");
static_assert(std::is_same_v<Reverse<std::tuple<int>>, std::tuple<int>>, "");
static_assert(std::is_same_v<Reverse<std::tuple<int, bool>>, std::tuple<bool, int>>, "");
static_assert(std::is_same_v<Reverse<std::tuple<int, bool, double>>, std::tuple<double, bool, int>>, "");

// PopBack

template<class>
struct PopBackT;

template<template<class...> class List, class... Ts>
struct PopBackT<List<Ts...>>
{
	using Type = Reverse<PopFront<Reverse<List<Ts...>>>>;
};

template<class List>
using PopBack = typename PopBackT<List>::Type;

static_assert(std::is_same_v<PopBack<TypeList<double>>, TypeList<>>, "");
static_assert(std::is_same_v<PopBack<TypeList<int, double>>, TypeList<int>>, "");
static_assert(std::is_same_v<PopBack<TypeList<bool, int, double>>, TypeList<bool, int>>, "");
static_assert(std::is_same_v<PopBack<ValueList<int, 1, 2, 3>>, ValueList<int, 1, 2>>, "");

// Back

template<class>
struct BackT;

template<template<class...> class List, class... Ts>
struct BackT<List<Ts...>>
{
	using Type = Front<Reverse<List<Ts...>>>;
};

template<class List>
using Back = typename BackT<List>::Type;

static_assert(std::is_same_v<Back<TypeList<double>>, double>, "");
static_assert(std::is_same_v<Back<TypeList<int, double>>, double>, "");
static_assert(std::is_same_v<Back<TypeList<bool, int, short>>, short>, "");
static_assert(std::is_same_v<Back<ValueList<int, 1, 2, 3>>, Value<int, 3>>, "");

// IfThenElse

template<bool, class, class>
struct IfThenElseT;

template<class Then, class Else>
struct IfThenElseT<true, Then, Else>
{
	using Type = Then;
};

template<class Then, class Else>
struct IfThenElseT<false, Then, Else>
{
	using Type = Else;
};

template<bool Cond, class Then, class Else>
using IfThenElse = typename IfThenElseT<Cond, Then, Else>::Type;

static_assert(std::is_same_v<IfThenElse<true, int, short>, int>, "");
static_assert(std::is_same_v<IfThenElse<false, int, short>, short>, "");

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
