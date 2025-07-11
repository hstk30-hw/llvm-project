// RUN: %clang_cc1 -std=c++20 -Wno-c++26-extensions -verify %s
// RUN: %clang_cc1 -std=c++2c -Wno-c++26-extensions -verify %s


static constexpr int PRIMARY = 0;
static constexpr int SPECIALIZATION_CONCEPT = 1;
static constexpr int SPECIALIZATION_REQUIRES = 2;

template <class T>
concept Concept = (sizeof(T) >= 2 * sizeof(int));

struct XY {
  int x;
  int y;
};

namespace members {

template <class T, class U> struct S {
  static constexpr int primary();
};

template <class T, class U> constexpr int S<T, U>::primary() {
  return PRIMARY;
};

template <Concept C, class U> struct S<C, U> {
  static constexpr int specialization();
};

template <class T, class U>
  requires(sizeof(T) == sizeof(int))
struct S<T, U> {
  static constexpr int specialization();
};

template <Concept C, class U> constexpr int S<C, U>::specialization() {
  return SPECIALIZATION_CONCEPT;
}

template <class T, class U>
  requires(sizeof(T) == sizeof(int))
constexpr int S<T, U>::specialization() {
  return SPECIALIZATION_REQUIRES;
}

static_assert(S<char, double>::primary() == PRIMARY);
static_assert(S<XY, double>::specialization() == SPECIALIZATION_CONCEPT);
static_assert(S<int, double>::specialization() == SPECIALIZATION_REQUIRES);

} // namespace members

namespace enumerations {

template <class T, class U> struct S {
  enum class E : int;
};

template <class T, class U> enum class S<T, U>::E { Value = PRIMARY };

template <Concept C, class U> struct S<C, U> {
  enum class E : int;
};

template <Concept C, class U>
enum class S<C, U>::E {
  Value = SPECIALIZATION_CONCEPT
};

template <class T, class U>
  requires(sizeof(T) == sizeof(int))
struct S<T, U> {
  enum class E : int;
};

template <class T, class U>
  requires(sizeof(T) == sizeof(int))
enum class S<T, U>::E {
  Value = SPECIALIZATION_REQUIRES
};

static_assert(static_cast<int>(S<char, double>::E::Value) == PRIMARY);
static_assert(static_cast<int>(S<XY, double>::E::Value) ==
              SPECIALIZATION_CONCEPT);
static_assert(static_cast<int>(S<int, double>::E::Value) ==
              SPECIALIZATION_REQUIRES);

} // namespace  enumerations

namespace multiple_template_parameter_lists {

template <class Outer>
struct S {
  template <class Inner>
  static constexpr int primary(Inner);
};

template <class Outer>
template <class Inner>
constexpr int S<Outer>::primary(Inner) {
  return PRIMARY;
};

template <Concept Outer>
struct S<Outer> {
  template <class Inner>
  static constexpr int specialization(Inner);
};

template <Concept Outer>
template <class Inner>
constexpr int S<Outer>::specialization(Inner) { return SPECIALIZATION_CONCEPT; }

template <class Outer>
  requires(sizeof(Outer) == sizeof(int))
struct S<Outer> {
  template <class Inner>
  static constexpr int specialization(Inner);
};

template <class Outer>
  requires(sizeof(Outer) == sizeof(int))
template <class Inner>
constexpr int S<Outer>::specialization(Inner) { return SPECIALIZATION_REQUIRES; }

static_assert(S<char>::primary("str") == PRIMARY);
static_assert(S<XY>::specialization("str") == SPECIALIZATION_CONCEPT);
static_assert(S<int>::specialization("str") == SPECIALIZATION_REQUIRES);

} // namespace multiple_template_parameter_lists

static constexpr int CONSTRAINED_METHOD_1 = 1;
static constexpr int CONSTRAINED_METHOD_2 = 2;

namespace constrained_members {

template <int>
struct S {
  template <Concept C>
  static constexpr int constrained_method();
};

template <>
template <Concept C>
constexpr int S<1>::constrained_method() { return CONSTRAINED_METHOD_1; }

template <>
template <Concept C>
constexpr int S<2>::constrained_method() { return CONSTRAINED_METHOD_2; }

static_assert(S<1>::constrained_method<XY>() == CONSTRAINED_METHOD_1);
static_assert(S<2>::constrained_method<XY>() == CONSTRAINED_METHOD_2);


template <class T1, class T2>
concept ConceptT1T2 = true;

template<typename T3>
struct S12 {
  template<ConceptT1T2<T3> T4>
  static constexpr int constrained_method();
};

template<>
template<ConceptT1T2<int> T5>
constexpr int S12<int>::constrained_method() { return CONSTRAINED_METHOD_1; }

template<>
template<ConceptT1T2<double> T5>
constexpr int S12<double>::constrained_method() { return CONSTRAINED_METHOD_2; }

static_assert(S12<int>::constrained_method<XY>() == CONSTRAINED_METHOD_1);
static_assert(S12<double>::constrained_method<XY>() == CONSTRAINED_METHOD_2);

} // namespace constrained members

namespace constrained_members_of_nested_types {

template <int>
struct S {
  struct Inner0 {
    struct Inner1 {
      template <Concept C>
      static constexpr int constrained_method();
    };
  };
};

template <>
template <Concept C>
constexpr int S<1>::Inner0::Inner1::constrained_method() { return CONSTRAINED_METHOD_1; }

template <>
template <Concept C>
constexpr int S<2>::Inner0::Inner1::constrained_method() { return CONSTRAINED_METHOD_2; }

static_assert(S<1>::Inner0::Inner1::constrained_method<XY>() == CONSTRAINED_METHOD_1);
static_assert(S<2>::Inner0::Inner1::constrained_method<XY>() == CONSTRAINED_METHOD_2);


template <class T1, class T2>
concept ConceptT1T2 = true;

template<typename T3>
struct S12 {
  struct Inner0 {
    struct Inner1 {
      template<ConceptT1T2<T3> T4>
      static constexpr int constrained_method();
    };
  };
};

template<>
template<ConceptT1T2<int> T5>
constexpr int S12<int>::Inner0::Inner1::constrained_method() { return CONSTRAINED_METHOD_1; }

template<>
template<ConceptT1T2<double> T5>
constexpr int S12<double>::Inner0::Inner1::constrained_method() { return CONSTRAINED_METHOD_2; }

static_assert(S12<int>::Inner0::Inner1::constrained_method<XY>() == CONSTRAINED_METHOD_1);
static_assert(S12<double>::Inner0::Inner1::constrained_method<XY>() == CONSTRAINED_METHOD_2);

} // namespace constrained_members_of_nested_types

namespace constrained_member_sfinae {

template<int N> struct S {
  template<class T>
  static constexpr int constrained_method() requires (sizeof(int[N * 1073741824 + 4]) == 16) { // expected-warning {{variable length arrays in C++ are a Clang extension}} \
                                                                                                  expected-note {{value 4294967296 is outside the range of representable values of type 'int'}} \
                                                                                                  expected-note {{while calculating associated constraint of template 'constrained_method' here}}
    return CONSTRAINED_METHOD_1;
  }

  template<class T>
  static constexpr int constrained_method() requires (sizeof(int[N]) == 16);
};

template<>
template<typename T>
constexpr int S<4>::constrained_method() requires (sizeof(int[4]) == 16) {
  return CONSTRAINED_METHOD_2;
}

// Verify that there is no amiguity in this case.
static_assert(S<4>::constrained_method<double>() == CONSTRAINED_METHOD_2);

} // namespace constrained_member_sfinae

namespace requires_expression_references_members {

void accept1(int x);
void accept2(XY xy);

template <class T> struct S {
  T Field = T();

  constexpr int constrained_method()
      requires requires { accept1(Field); };

  constexpr int constrained_method()
      requires requires { accept2(Field); };
};

template <class T>
constexpr int S<T>::constrained_method()
  requires requires { accept1(Field); } {
  return CONSTRAINED_METHOD_1;
}

template <class T>
constexpr int S<T>::constrained_method()
  requires requires { accept2(Field); } {
  return CONSTRAINED_METHOD_2;
}

static_assert(S<int>().constrained_method() == CONSTRAINED_METHOD_1);
static_assert(S<XY>().constrained_method() == CONSTRAINED_METHOD_2);

} // namespace requires_expression_references_members

namespace GH60231 {

template<typename T0> concept C = true;

template <typename T1>
struct S {
  template <typename F1> requires C<S<T1>>
  void foo1(F1 f);

  template <typename F2>
  void foo2(F2 f) requires C<S<T1>>;

  template <typename F3> requires C<F3>
  void foo3(F3 f);
};

template <typename T2>
template <typename F4> requires C<S<T2>>
void S<T2>::foo1(F4 f) {}

template <typename T3>
template <typename F5>
void S<T3>::foo2(F5 f) requires C<S<T3>> {}

template <typename T4>
template <typename F6> requires C<F6>
void S<T4>::foo3(F6 f) {}

} // namespace GH60231

namespace GH62003 {

template <typename T0> concept Concept = true;

template <class T1>
struct S1 {
  template <Concept C1>
  static constexpr int foo();
};
template <class T2>
template <Concept C2>
constexpr int S1<T2>::foo() { return 1; }

template <Concept C3>
struct S2 {
  template <class T3>
  static constexpr int foo();
};
template <Concept C4>
template <class T4>
constexpr int S2<C4>::foo() { return 2; }

template <Concept C5>
struct S3 {
  template <Concept C6>
  static constexpr int foo();
};
template <Concept C7>
template <Concept C8>
constexpr int S3<C7>::foo() { return 3; }

static_assert(S1<int>::foo<int>() == 1);
static_assert(S2<int>::foo<int>() == 2);
static_assert(S3<int>::foo<int>() == 3);

} // namespace GH62003

namespace MultilevelTemplateWithPartialSpecialization {
template <typename>
concept Concept = true;

namespace two_level {
template <typename T1, int>
struct W0 {
  template <typename T2>
  requires (Concept<T2>)
  void f(const T2 &);
};

template <typename T3>
struct W0<T3, 0> {
  template <typename T4>
  requires (Concept<T4>)
  void f(const T4 &);
};

template <typename T3>
template <typename T4>
requires (Concept<T4>)
inline void W0<T3, 0>::f(const T4 &) {}
} // namespace two_level

namespace three_level {
template <typename T1, int>
struct W0 {
  template <typename T2>
  struct W1 {
    template <typename T3>
    requires (Concept<T3>)
    void f(const T3 &);
  };
};

template <typename T4>
struct W0<T4, 0> {
  template <typename T5>
  struct W1 {
    template <typename T6>
    requires (Concept<T6>)
    void f(const T6 &);
  };
};

template <typename T7>
template <typename T8>
template <typename T9>
requires (Concept<T9>)
inline void W0<T7, 0>::W1<T8>::f(const T9 &) {}
} // namespace three_level

} // namespace MultilevelTemplateWithPartialSpecialization

namespace PR62697 {
template<typename>
concept c = true;

template<typename T>
struct s {
    void f() requires c<void(T)>;
};

template<typename T>
void s<T>::f() requires c<void(T)> { }
}

namespace GH62272 {
template<typename T> concept A = true;
template<typename T> struct X { A<T> auto f(); };
template<typename T> A<T> auto X<T>::f() {}
}

namespace GH65810 {
template<typename Param>
concept TrivialConcept =
requires(Param param) {
  (void)param;
};

template <typename T>
struct Base {
  class InnerClass;
};

template <typename T>
class Base<T>::InnerClass {
  template <typename Param>
    requires TrivialConcept<Param>
    int func(Param param) const;
};

template <typename T>
template <typename Param>
requires TrivialConcept<Param>
int Base<T>::InnerClass::func(Param param) const {
  return 0;
}

template<typename T>
struct Outermost {
  struct Middle {
    template<typename U>
    struct Innermost {
      template <typename Param>
        requires TrivialConcept<Param>
        int func(Param param) const;
    };
  };
};

template <typename T>
template <typename U>
template <typename Param>
requires TrivialConcept<Param>
int Outermost<T>::Middle::Innermost<U>::func(Param param) const {
  return 0;
}

} // namespace GH65810

namespace GH61763 {
template<typename T, typename U>
concept same_as = true;

template <class = void>
struct Foo {
      template <same_as<void> Param>
            friend struct Bar;
};

template struct Foo<>;

template <same_as<void> Param>
struct Bar {
};


template<typename T>
concept ok = true;

struct outer {
    template<typename T>
        requires ok<T>
          struct foo {};
};

template<typename U>
struct bar {
    template<typename T>
        requires ok<T>
          friend struct outer::foo;
};

bar<int> x;
} // namespace GH61763


namespace GH74314 {
template <class T, class U> constexpr bool is_same_v = __is_same(T, U);
template <class T, class U> constexpr bool is_not_same_v = !__is_same(T, U);

template <class Result>
concept something_interesting = requires {
      true;
      requires is_same_v<int, Result>;
};

template <class T>
struct X { // #defined-here
      void foo() requires requires { requires is_not_same_v<T, int>; };
      void bar(decltype(requires { requires is_not_same_v<T, int>; }));
};

template <class T>
void X<T>::foo() requires requires { requires something_interesting<T>; } {}
// expected-error@-1{{definition of 'foo' does not match any declaration}}
// expected-note@#defined-here{{defined here}}
// expected-note@-8{{member declaration nearly matches}}

template <class T>
void X<T>::foo() requires requires { requires is_not_same_v<T, int>; } {} // ok

template <class T>
void X<T>::bar(decltype(requires { requires something_interesting<T>; })) {}
// expected-error@-1{{definition of 'bar' does not match any declaration}}
// expected-note@#defined-here{{defined here}}

template <class T>
void X<T>::bar(decltype(requires { requires is_not_same_v<T, int>; })) {}
} // namespace GH74314

namespace GH56482 {
template <typename SlotMap>
concept slot_map_has_reserve = true;

template <typename T> struct Slot_map {
  constexpr void reserve() const noexcept
    requires slot_map_has_reserve<Slot_map>;

  constexpr void reserve(int) const noexcept
    requires slot_map_has_reserve<Slot_map<T>>;
};

template <typename T>
constexpr void Slot_map<T>::reserve() const noexcept
  requires slot_map_has_reserve<Slot_map<T>>
{}

template <typename T>
constexpr void Slot_map<T>::reserve(int) const noexcept
  requires slot_map_has_reserve<Slot_map>
{}
} // namespace GH56482

namespace GH74447 {
template <typename T> struct S {
  template <typename... U, int V>
  void test(T target, U... value)
    requires requires {
      target;
      sizeof...(value) == 1;
      V == 2;
    };
};

template <typename T>
template <typename... U, int V>
void S<T>::test(T target, U... value)
  requires requires {
    target;
    sizeof...(value) == 1;
    V == 2;
  }
{}
} // namespace GH74447

namespace GH72557 {

template <typename...>
concept IsAnyOf = true;

template <class... DerTs> struct DerivedCollection {
  template <class DerT>
    requires IsAnyOf<DerTs...>
  unsigned long index();
};

template <class... DerTs>
template <class DerT>
  requires IsAnyOf<DerTs...>
unsigned long DerivedCollection<DerTs...>::index() {}

} // namespace GH72557

namespace GH101735 {

template <class, class>
concept True = true;

template <typename T>
class A {
  template <typename... Ts>
  void method(Ts&... ts)
    requires requires (T t) {
      { t.method(static_cast<Ts &&>(ts)...) } -> True<void>;
    };
};

template <typename T>
template <typename... Ts>
void A<T>::method(Ts&... ts)
  requires requires (T t) {
    { t.method(static_cast<Ts &&>(ts)...) } -> True<void>;
  } {}

}

namespace GH63782 {
// GH63782 was also fixed by PR #80594, so let's add a test for it.

template<bool... Vals>
constexpr bool All = (Vals && ...);

template<bool... Bs>
class Class {
  template<typename>
  requires All<Bs...>
  void Foo();
};

template<bool... Bs>
template<typename>
requires All<Bs...>
void Class<Bs...>::Foo() {
};

} // namespace GH63782

namespace eve {
// Reduced from the "eve" project

template <typename... Ts>
struct tuple {
  template <int I0> requires(I0 <= sizeof...(Ts))
  constexpr auto split();
};

template <typename... Ts>
template <int I0>
requires(I0 <= sizeof...(Ts))
constexpr auto tuple<Ts...>::split(){
  return 0;
}

int foo() {
  tuple<int, float> x;
  return x.split<0>();
}

} // namespace eve

namespace GH93099 {

// Issues with sizeof...(expr)

template <typename T = int> struct C {
  template <int... N>
    requires(sizeof...(N) > 0)
  friend class NTTP;

  template <class... Tp>
    requires(sizeof...(Tp) > 0)
  friend class TP;

  template <template <typename> class... TTp>
    requires(sizeof...(TTp) > 0)
  friend class TTP;
};

template <int... N>
  requires(sizeof...(N) > 0)
class NTTP;

template <class... Tp>
  requires(sizeof...(Tp) > 0)
class TP;

template <template <typename> class... TTp>
  requires(sizeof...(TTp) > 0)
class TTP;

C v;

} // namespace GH93099

namespace GH115098 {

template <typename... Ts> struct c {
  template <typename T>
    requires(sizeof...(Ts) > 0)
  friend bool operator==(c, c);
};

template <typename... Ts> struct d {
  template <typename T>
    requires(sizeof...(Ts) > 0)
  friend bool operator==(d, d);
};

template struct c<int>;
template struct d<int, int>;

} // namespace GH115098

namespace GH123441 {

struct buf {
  constexpr buf(auto&&... initList) requires (sizeof...(initList) <= 8);
};

constexpr buf::buf(auto&&... initList) requires (sizeof...(initList) <= 8) {}

template <class>
struct buffer {
  constexpr buffer(auto&&... initList) requires (sizeof...(initList) <= 8);
};

template <class T>
constexpr buffer<T>::buffer(auto&&... initList) requires (sizeof...(initList) <= 8) {}

template <class...>
struct foo { // expected-note {{foo defined here}}
  constexpr foo(auto&&... initList)
    requires (sizeof...(initList) <= 8);
};

template <class... T>
constexpr foo<T...>::foo(auto&&... initList) // expected-error {{does not match any declaration}}
  requires (sizeof...(T) <= 8) {}

} // namespace GH123441

namespace GH114685 {

template <typename T> struct ptr {
  template <typename U>
  friend ptr<U> make_item(auto &&args)
    requires(sizeof(args) > 1);
};

template <typename U>
ptr<U> make_item(auto &&args)
  requires(sizeof(args) > 1) {}

ptr<char> p;

} // namespace GH114685

namespace GH123472 {

consteval bool fn() { return true; }

struct S {
  template <typename T>
  static consteval void mfn() requires (bool(&fn));
};

template <typename T>
consteval void S::mfn() requires (bool(&fn)) {}

}


namespace GH138255 {

template <typename... T>
concept C = true;

struct Func {
  template<typename... Ts>
  requires C<Ts...[0]>
  static auto buggy() -> void;

  template<typename... Ts>
  requires C<Ts...[0]>
  friend auto fr() -> void;

  template<typename... Ts>
  requires C<Ts...[0]>
  friend auto fr2() -> void{}; // expected-note{{previous definition is here}}
};

template<typename... Ts>
requires C<Ts...[0]>
auto Func::buggy() -> void {}

template<typename... Ts>
requires C<Ts...[0]>
auto fr() -> void {}

template<typename... Ts>
requires C<Ts...[0]>
auto fr2() -> void {} // expected-error{{redefinition of 'fr2'}}


template <typename... Ts>
requires C<Ts...[0]>
struct Class;

template <typename... Ts>
requires C<Ts...[0]>
struct Class;


template <typename...>
struct TplClass {
  template<typename... Ts>
  requires C<Ts...[0]>
  static auto buggy() -> void;
};

template<>
template<typename... Ts>
requires C<Ts...[0]>
auto TplClass<int>::buggy() -> void {}

}

namespace PackIndexExpr {
template <int... T>
concept C = true;

template <typename...> struct TplClass {
  template <int... Ts>
  requires C<Ts...[0]>
  static auto buggy() -> void;
};

template <>
template <int... Ts>
requires C<Ts...[0]>
auto TplClass<int>::buggy() -> void {}
}

namespace GH139476 {

namespace moo {
  template <typename T>
  constexpr bool baa = true;

  template <typename T> requires baa<T>
  void caw();
}

template <typename T> requires moo::baa<T>
void moo::caw() {}

}

namespace GH145521 {

template <typename X>
concept is_valid = true;

template<typename T>
class Nesting
{
public:
    template<typename Q> requires is_valid<Q>
    class Inner;

    template<typename Q> requires is_valid<Q>
    friend class Inner2;
};

template<typename T>
template<typename Q> requires is_valid<Q>
class Nesting<T>::Inner {};

template<typename Q> requires is_valid<Q>
class Inner2 {};

}
