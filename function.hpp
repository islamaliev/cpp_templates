#include <functional>

namespace {
	struct TestCallable {
		void operator()(double) {};

		float memberFunc() { return 0.f; }
		long memberConstFunc() const { return 1; }
	};

	auto testLambda = [](int, float) { return true; };

	int testGlobalFunction(char) { return 0; }
}

// IsCallable

template<class, class = void>
struct IsCallableT : std::false_type {};

template<class T>
struct IsCallableT<T, std::void_t<decltype(&T::operator())>> : std::true_type {};

template<class T>
constexpr bool IsCallable = IsCallableT<T>::value;

static_assert(!IsCallable<int>, "");
static_assert(IsCallable<TestCallable>, "");
static_assert(IsCallable<std::function<int(int, double)>>, "");
static_assert(IsCallable<decltype(testLambda) > , "");

// Function

template<class R, class... Params>
struct FunctionInfo
{
	using Signature = R(Params...);
	using Ret = R;

	static constexpr int numParams = sizeof...(Params);

	template<int index>
	struct Param
	{
		using Type = NthElement<TypeList<Params...>, index>;
	};
};
template<class T, typename = void>
struct Function;

// member
template<class T, class Ret, class... Params>
struct Function<Ret(T::*)(Params...)> : FunctionInfo<Ret, Params...>
{
	static constexpr bool isMemberFunction = true;
};

template<class T, class Ret, class... Params>
struct Function<Ret(T::*)(Params...) const> : Function<Ret(T::*)(Params...)> {};

// function
template<class R, class... FuncParams>
struct Function<R(FuncParams...)> : FunctionInfo<R, FuncParams...>
{
	static constexpr bool isMemberFunction = false;
};

template<class R, class... FuncParams>
struct Function<R(*)(FuncParams...)> : Function<R(FuncParams...)> {};

template<class R, class... FuncParams>
struct Function<R(&)(FuncParams...)> : Function<R(FuncParams...)> {};

// lambda
template<typename T>
struct Function<T, decltype(void(&T::operator()))> : Function<decltype(&T::operator())> {
	static constexpr bool isMemberFunction = false;
};

static_assert(Function<decltype(&testGlobalFunction)>::numParams == 1, "");
static_assert(Function<decltype(&testGlobalFunction)>::isMemberFunction == false, "");
static_assert(Function<decltype(testLambda)>::isMemberFunction == false, "");
static_assert(Function<decltype(testLambda)>::numParams == 2, "");
static_assert(std::is_same_v<Function<decltype(testLambda)>::Ret, bool>, "");
static_assert(std::is_same_v<Function<decltype(testLambda)>::Param<0>::Type, int>, "");
static_assert(std::is_same_v<Function<decltype(testLambda)>::Param<1>::Type, float>, "");
static_assert(Function<TestCallable>::isMemberFunction == false, "");
static_assert(std::is_same_v<Function<TestCallable>::Param<0>::Type, double>, "");
static_assert(Function<decltype(&TestCallable::memberFunc)>::isMemberFunction == true, "");
static_assert(Function<decltype(&TestCallable::memberConstFunc)>::isMemberFunction == true, "");
