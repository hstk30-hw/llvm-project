// RUN: %clang_cc1 -std=c++1z %s -verify

namespace std {
  template<typename> struct tuple_size;
  template<int, typename> struct tuple_element;
}

struct Get {
  template<int> int get() { return 0; }
  operator bool() { return true; }
};

namespace std {
  template<> struct tuple_size<Get> { static constexpr int value = 1; };
  template<> struct tuple_element<0, Get> { using type = int; };
}

struct Na {
  bool flag;
  float data;
};

struct Rst {
  bool flag;
  float data;
  explicit operator bool() const {
    return flag;
  }
};

Rst f();
Na g();

namespace CondInIf {
int h() {
  if (auto [ok, d] = f()) // expected-warning {{structured binding declaration in a condition is a C++2c extension}}
    ;
  if (auto [ok, d] = g()) // expected-warning {{structured binding declaration in a condition is a C++2c extension}} expected-error {{value of type 'Na' is not contextually convertible to 'bool'}}
    ;
  if (auto [value] = Get()) // expected-warning {{structured binding declaration in a condition is a C++2c extension}}
    return value;
}
} // namespace CondInIf

namespace CondInWhile {
int h() {
  while (auto [ok, d] = f()) // expected-warning {{structured binding declaration in a condition is a C++2c extension}}
    ;
  while (auto [ok, d] = g()) // expected-warning {{structured binding declaration in a condition is a C++2c extension}} expected-error {{value of type 'Na' is not contextually convertible to 'bool'}}
    ;
  while (auto [value] = Get()) // expected-warning{{structured binding declaration in a condition is a C++2c extension}}
    return value;
}
} // namespace CondInWhile

namespace CondInFor {
int h() {
  for (; auto [ok, d] = f();) // expected-warning {{structured binding declaration in a condition is a C++2c extension}}
    ;
  for (; auto [ok, d] = g();) // expected-warning {{structured binding declaration in a condition is a C++2c extension}} expected-error {{value of type 'Na' is not contextually convertible to 'bool'}}
    ;
  for (; auto [value] = Get();) // expected-warning {{structured binding declaration in a condition is a C++2c extension}}
    return value;
}
} // namespace CondInFor

struct IntegerLike {
  bool flag;
  float data;
  operator int() const {
    return int(data);
  }
};

namespace CondInSwitch {
int h(IntegerLike x) {
  switch (auto [ok, d] = x) // expected-warning {{structured binding declaration in a condition is a C++2c extension}}
    ;
  switch (auto [ok, d] = g()) // expected-warning {{structured binding declaration in a condition is a C++2c extension}} expected-error {{statement requires expression of integer type ('Na' invalid)}}
    ;
  switch (auto [value] = Get()) {// expected-warning {{structured binding declaration in a condition is a C++2c extension}}
  // expected-warning@-1{{switch condition has boolean value}}
  case 1:
    return value;
  }
}
} // namespace CondInSwitch
