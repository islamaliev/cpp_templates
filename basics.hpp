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

// ListSize

template<class>
struct ListSize;

template<template<class...> class List, class... Types>
struct ListSize<List<Types...>>
{
	static constexpr int value = sizeof...(Types);
};

static_assert(ListSize<TypeList<>>::value == 0, "");
static_assert(ListSize<TypeList<int>>::value == 1, "");
static_assert(ListSize<TypeList<int, float>>::value == 2, "");