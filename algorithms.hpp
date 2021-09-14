#pragma once

#include "basics.hpp"

// Transform

template<class, template<class> class>
struct TransformT; 

template<template<class...> class List, template<class> class Func, class... Ts>
struct TransformT<List<Ts...>, Func> {
	using Type = List<typename Func<Ts>::Type...>;
};

template<class List, template<class> class Func>
using Transform = typename TransformT<List, Func>::Type;

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

// ConcatLists

template<class...>
struct ConcatListsT;

template<template<class...> class List1, template<class...> class List2, class... Types1, class...Types2>
struct ConcatListsT<List1<Types1...>, List2<Types2...>>
{
	using Type = List1<Types1..., Types2...>;
};

template<class FirstList, class... RemainingLists>
struct ConcatListsT<FirstList, RemainingLists...>
{
	using Type = typename ConcatListsT<FirstList, typename ConcatListsT<RemainingLists...>::Type>::Type;
};

template<class... Lists>
using ConcatLists = typename ConcatListsT<Lists...>::Type;

static_assert(std::is_same_v<ConcatLists<TypeList<bool, int>, TypeList<float, char>>, TypeList<bool, int, float, char>>, "");
static_assert(std::is_same_v<ConcatLists<TypeList<>, TypeList<float, char>>, TypeList<float, char>>, "");
static_assert(std::is_same_v<ConcatLists<TypeList<bool, int>, TypeList<>>, TypeList<bool, int>>, "");
static_assert(std::is_same_v<ConcatLists<TypeList<bool>, TypeList<int>, TypeList<float, char>>, TypeList<bool, int, float, char>>, "");

// JoinLists

template<class...>
struct JoinListsT;

template<class Delim, template<class...> class List1, template<class...> class List2, class... Types1, class... Types2>
struct JoinListsT<Delim, List1<Types1...>, List2<Types2...>>
{
	using Type = ConcatLists<List1<Types1..., Delim>, List2<Types2...>>;
};

template<class Delim, class FirstList, class... RemainingLists>
struct JoinListsT<Delim, FirstList, RemainingLists...>
{
	using Type = typename JoinListsT<Delim, FirstList, typename JoinListsT<Delim, RemainingLists...>::Type>::Type;
};

template<class Delim, class... Lists>
using JoinLists = typename JoinListsT<Delim, Lists...>::Type;

static_assert(std::is_same_v<JoinLists<char, TypeList<>, TypeList<float>>, TypeList<char, float>>, "");
static_assert(std::is_same_v<JoinLists<char, TypeList<int, bool>, TypeList<>>, TypeList<int, bool, char>>, "");
static_assert(std::is_same_v<JoinLists<char, TypeList<int, bool>, TypeList<float>>, TypeList<int, bool, char, float>>, "");
static_assert(std::is_same_v<JoinLists<char, TypeList<int, bool>, TypeList<float>, TypeList<double>>, TypeList<int, bool, char, float, char, double>>, "");
static_assert(std::is_same_v<JoinLists<Value<int, 0>, ValueList<int, 1>, ValueList<int, 2, 3>, ValueList<int, 4>>, ValueList<int, 1, 0, 2, 3, 0, 4>>, "");

// ListHead

template<int Size, int Index, class List, class ResultList = EmptyList<List>>
struct ListHeadT
	: ListHeadT<Size, Index + 1, PopFront<List>, PushBack<ResultList, Front<List>>>
{};

template<int Size, class List, class ResultList>
struct ListHeadT<Size, Size, List, ResultList> 
{
	using Type = ResultList;
};

template<int Size, class List>
using ListHead = typename ListHeadT<Size, 0, List>::Type;

static_assert(std::is_same_v<ListHead<0, TypeList<float, int, char>>, TypeList<>>, "");
static_assert(std::is_same_v<ListHead<1, TypeList<float, int, char>>, TypeList<float>>, "");
static_assert(std::is_same_v<ListHead<2, TypeList<float, int, char>>, TypeList<float, int>>, "");
static_assert(std::is_same_v<ListHead<3, TypeList<float, int, char>>, TypeList<float, int, char>>, "");
static_assert(std::is_same_v<ListHead<2, ValueList<int, 1, 2, 3>>, ValueList<int, 1, 2>>, "");
static_assert(std::is_same_v<ListHead<2, std::tuple<int, float, char>>, std::tuple<int, float>>, "");

// ListTail

template<int Size, class List>
using ListTail = Reverse<ListHead<Size, Reverse<List>>>;

static_assert(std::is_same_v<ListTail<0, TypeList<float, int, char>>, TypeList<>>, "");
static_assert(std::is_same_v<ListTail<1, TypeList<float, int, char>>, TypeList<char>>, "");
static_assert(std::is_same_v<ListTail<2, TypeList<float, int, char>>, TypeList<int, char>>, "");
static_assert(std::is_same_v<ListTail<3, TypeList<float, int, char>>, TypeList<float, int, char>>, "");
static_assert(std::is_same_v<ListTail<2, ValueList<int, 1, 2, 3>>, ValueList<int, 2, 3>>, "");
static_assert(std::is_same_v<ListTail<2, std::tuple<int, float, char>>, std::tuple<float, char>>, "");

// ListSlice

template<int, int, class>
struct ListSliceT;

template<int Begin, int End, class List>
using ListSlice = ListHead<End - Begin, ListTail<ListSize<List>::value - Begin, List>>;

static_assert(std::is_same_v<ListSlice<1, 4, TypeList<int, char, float, bool, double>>, TypeList<char, float, bool>>, "");
static_assert(std::is_same_v<ListSlice<0, 2, TypeList<int, char, float, bool, double>>, TypeList<int, char>>, "");
static_assert(std::is_same_v<ListSlice<2, 5, TypeList<int, char, float, bool, double>>, TypeList<float, bool, double>>, "");

// SortList

template<class List, int Ind, int End, template<class, class> class Comp = GreaterValue>
struct SortListT
{
private:
	using El = NthElement<List, Ind>;
	static constexpr int elPos = LowerBound<ListHead<Ind, List>, El, Comp>::value;
	using SortedPart = JoinLists<El, ListHead<elPos, List>, ListSlice<elPos, Ind, List>>;

public:
	using Type = typename SortListT<ConcatLists<SortedPart, ListTail<ListSize<List>::value - Ind - 1, List>>, Ind + 1, End, Comp>::Type;
};

template<class List, int Ind, template<class, class> class Comp>
struct SortListT<List, Ind, Ind, Comp>
{
	using Type = List;
};

template<class List, template<class, class> class Comp>
struct SortListT<List, 1, 0, Comp>
{
	using Type = EmptyList<List>;
};

template<class List>
using SortList = typename SortListT<List, 1, ListSize<List>::value>::Type;

template<class List, template<class, class> class Comp>
using SortListComp = typename SortListT<List, 1, ListSize<List>::value, Comp>::Type;

static_assert(std::is_same_v<SortList<ValueList<int>>, ValueList<int>>, "");
static_assert(std::is_same_v<SortList<ValueList<int, 3>>, ValueList<int, 3>>, "");
static_assert(std::is_same_v<SortList<ValueList<int, 4, 3>>, ValueList<int, 3, 4>>, "");
static_assert(std::is_same_v<SortList<ValueList<int, 1, 4, 3>>, ValueList<int, 1, 3, 4>>, "");
static_assert(std::is_same_v<SortList<ValueList<int, 4, 3, 1, 5, 2>>, ValueList<int, 1, 2, 3, 4, 5>>, "");
static_assert(std::is_same_v<SortListComp<ValueList<int, 4, 3, 1, 5, 2>, LessValue>, ValueList<int, 5, 4, 3, 2, 1>>, "");

// IsEven

struct IsEven {
	template<class T, T I>
	static constexpr bool apply(Value<T, I>) {
		return apply(I);
	}

	template<class T>
	static constexpr bool apply(T i) {
		return i % 2 == 0;
	}
};

static_assert(IsEven::apply(1) == false, "");
static_assert(IsEven::apply(2) == true, "");
static_assert(IsEven::apply(Value<int, 1>{}) == false, "");
static_assert(IsEven::apply(Value<int, 2>{}) == true, "");

// Filter

template<class List, class FilterFunc, class Result = EmptyList<List>>
struct FilterT {
	using Type = typename FilterT<PopFront<List>, FilterFunc, 
		IfThenElse<
			FilterFunc::apply(Front<List>{}),
			PushBack<Result, Front<List>>,
			Result
		>
	>::Type;
};

template<template<class...> class List, class Func, class Result>
struct FilterT<List<>, Func, Result> {
	using Type = Result;
};

template<class List, class FilterFunc>
using Filter = typename FilterT<List, FilterFunc>::Type;

static_assert(std::is_same_v<Filter<ValueList<int, 1, 3, 4, 5, 6, 2>, IsEven>, ValueList<int, 4, 6, 2>>, "");

// Not

template<class Func>
struct Not {
	template<class T>
	static constexpr bool apply(T v) {
		return !Func::apply(v);
	}
};

static_assert(Not<IsEven>::apply(Value<int, 1>{}) == true, "");
static_assert(Not<IsEven>::apply(Value<int, 2>{}) == false, "");

// RemoveIf

template<class List, class Func>
using RemoveIf = Filter<List, Not<Func>>;

static_assert(std::is_same_v<RemoveIf<ValueList<int, 1, 3, 4, 5, 6, 2>, IsEven>, ValueList<int, 1, 3, 5>>, "");

// LessEq

template<class>
struct LessEq; 

template<class T, int I>
struct LessEq<Value<T, I>> {
	template<class T2, T2 I>
	static constexpr bool apply(Value<T2, I>) {
		return apply(I);
	}

	template<class T2>
	static constexpr bool apply(T2 i) {
		return i <= I;
	}
};

static_assert(LessEq<Value<int, 2>>::apply(Value<int, 2>{}), "");
static_assert(LessEq<Value<int, 2>>::apply(1), "");
static_assert(LessEq<Value<int, 2>>::apply(2), "");
static_assert(!LessEq<Value<int, 2>>::apply(3), "");

// Greater

template<class>
struct Greater; 

template<class T, int I>
struct Greater <Value<T, I>> {
	template<class T2, T2 I>
	static constexpr bool apply(Value<T2, I>) {
		return apply(I);
	}

	template<class T2>
	static constexpr bool apply(T2 i) {
		return i > I;
	}
};

static_assert(!Greater<Value<int, 2>>::apply(Value<int, 2>{}), "");
static_assert(!Greater<Value<int, 2>>::apply(1), "");
static_assert(!Greater<Value<int, 2>>::apply(2), "");
static_assert(Greater<Value<int, 2>>::apply(3), "");

// QuickSort

template<class List>
struct QuickSortT {
private:
	using Pivot = Front<List>;
	using LowerPart = Filter<PopFront<List>, LessEq<Pivot>>;
	using UpperPart = Filter<PopFront<List>, Greater<Pivot>>;

public:
	using Type = JoinLists<
		Pivot, 
		typename QuickSortT<LowerPart>::Type, 
		typename QuickSortT<UpperPart>::Type
	>;
};

template<template<class...> class List>
struct QuickSortT<List<>> {
	using Type = List<>;
};

template<class List>
using QuickSort = typename QuickSortT<List>::Type;

static_assert(std::is_same_v<QuickSort<ValueList<int>>, ValueList<int>>, "");
static_assert(std::is_same_v<QuickSort<ValueList<int, 3>>, ValueList<int, 3>>, "");
static_assert(std::is_same_v<QuickSort<ValueList<int, 4, 3>>, ValueList<int, 3, 4>>, "");
static_assert(std::is_same_v<QuickSort<ValueList<int, 1, 4, 3>>, ValueList<int, 1, 3, 4>>, "");
static_assert(std::is_same_v<QuickSort<ValueList<int, 4, 3, -1, 5, 2, -2>>, ValueList<int, -2, -1, 2, 3, 4, 5>>, "");
