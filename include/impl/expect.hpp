#ifndef __CPPUNITTEST_EXPECT_IMPL__
#define __CPPUNITTEST_EXPECT_IMPL__

#include <sstream>
#include <sys/time.h>

#ifdef __GNUG__
#include<cxxabi.h>
#endif

namespace cppunittest {

template<typename T> inline std::string classname(const T& o) {
  const std::type_info& t = typeid(o);
  return classname(t);
}

inline std::string classname(const std::type_info& t) {

  #ifdef __GNUG__

    int status;
    char *nm = abi::__cxa_demangle(t.name(), NULL, NULL, &status);
    std::string c_nm(status == 0? nm : t.name());
    free(nm);

    return c_nm;

  #else

    return static_cast<std::string>(t.name());

  #endif
}

template <typename K> std::string _errmsg(
    const std::string& prefix,
    const std::string& operation,
    const K& actual, const K& expect, bool isnot = false) {
  std::stringstream ss;

  ss << std::boolalpha
      << prefix
      << (isnot? "expected is 'not " : "expected is '")
      << operation
      << expect
      << "', but actual is '"
      << actual
      << "'";

  return ss.str();
}

template <typename K> std::string _errmsg(
    const std::string& prefix,
    const K& actual, const K& expect, bool isnot = false) {
  if constexpr(std::is_same<std::type_info, K>::value) {
    return _errmsg<std::string>(prefix, "",
        classname(actual),
        classname(expect), isnot);
  } else {
    return _errmsg(prefix, "", actual, expect, isnot);
  }
}

template <typename T> inline expect<T> :: expect(
    const T& actual) :
    _actual(const_cast<T*>(&actual))
  {}

template <typename T> inline expect<T> :: expect(
    const T& actual, const std::string& message) :
    _actual(const_cast<T*>(&actual)), _msgprefix(message)
  {}

template <typename T> inline
bool _equals_(const T& actual, const T& expected) {
  if constexpr(std::is_same<std::function<void()>, T>::value) {
    static_assert(
        !std::is_same<std::function<void()>, T>::value,
        "Template type must not be function<void()>");
  }
  return actual == expected;
}

template <typename T> inline
void expect<T> :: is(const T& expected) const {
  if (!_equals_(*_actual, expected)) {
    throw expecterror(_errmsg(_msgprefix, *_actual, expected));
  }
}

template <typename T> inline
void expect<T> :: isnot(const T& expected) const {
  if (_equals_(*_actual, expected)) {
    throw expecterror(_errmsg(_msgprefix, *_actual, expected));
  }
}


template <typename T> inline void expect<T> :: istrue() const {
  static_assert(std::is_same<bool, T>::value,
    "Template type must be bool");

  if constexpr(std::is_same<bool, T>::value) {
    if (!*_actual)
      throw expecterror(
        _errmsg(_msgprefix, *_actual, true));
  }
}

template <typename T> inline void expect<T> :: isfalse() const {
  static_assert(std::is_same<bool, T>::value,
    "Template type must be bool");

  if constexpr(std::is_same<bool, T>::value) {
    if (*_actual)
      throw expecterror(
        _errmsg(_msgprefix, *_actual, false));
  }
}

template <typename T> inline
void expect<T> :: less_than(const T& expected) const {
  static_assert(std::is_arithmetic<T>::value,
    "Template type must be arithmetic");
  if constexpr(std::is_arithmetic<T>::value) {
    if (*_actual >= expected)
      throw expecterror(
        _errmsg(_msgprefix, "less than ",
        *_actual, expected));
  }
}

template <typename T> inline
void expect<T> :: not_less_than(const T& expected) const {
  static_assert(std::is_arithmetic<T>::value,
    "Template type must be arithmetic");
  if constexpr(std::is_arithmetic<T>::value) {
    if (*_actual < expected)
      throw expecterror(
        _errmsg(_msgprefix, "less than ",
        *_actual, expected, true));
  }
}

template <typename T> inline
void expect<T> :: greater_than(const T& expected) const {
  static_assert(std::is_arithmetic<T>::value,
    "Template type must be arithmetic");
  if constexpr(std::is_arithmetic<T>::value) {
    if (*_actual <= expected)
      throw expecterror(
        _errmsg(_msgprefix, "greater than ",
        *_actual, expected));
  }
}

template <typename T> inline
void expect<T> :: not_greater_than(const T& expected) const {
  static_assert(std::is_arithmetic<T>::value,
    "Template type must be arithmetic");
  if constexpr(std::is_arithmetic<T>::value) {
    if (*_actual > expected)
      throw expecterror(
        _errmsg(_msgprefix, "greater than ",
        *_actual, expected, true));
  }
}

template <typename T> inline
void expect<T> :: has(const T& expected) const {
  static_assert(std::is_base_of<std::string, T>::value,
    "Template type must be std::string");
  if constexpr(std::is_base_of<std::string, T>::value) {
    std::size_t pos = (*_actual).find(expected);
    if (pos == std::string::npos)
      throw expecterror(
        "actual string '" +
        (*_actual) +
        "' does not contain expected string '" +
        expected +
        "'");
  }
}

template <typename T> inline void expect<T> :: throws(
    const std::type_info& errtype,
    const std::string& errmsg) const {
  static_assert(std::is_same<std::function<void()>, T>::value,
      "Template type must be function<void()>");

  if constexpr(std::is_same<std::function<void()>, T>::value) {
    try {
      (*_actual)();
      throw expecterror(
          "expected exception, but no exception thrown");
    } catch(const expecterror& e) {
      throw;
    } catch(const std::exception& e) {
        expect<std::type_info>(typeid(e)).is(errtype);
        expect<std::string>(e.what()).is(errmsg);
    } catch(...) {
        throw expecterror("expected exception of type " +
            classname(errtype));
    }
  }

}

template <typename T> inline void expect<T> :: throws(
    const std::type_info& errtype) const {
  static_assert(std::is_same<std::function<void()>, T>::value,
      "Template type must be function<void()>");

  if constexpr(std::is_same<std::function<void()>, T>::value) {
    try {
      (*_actual)();
      throw expecterror(
          "expected exception, but no exception thrown");
    } catch(const expecterror& e) {
      throw;
    } catch(const std::exception& e) {
        expect<std::type_info>(typeid(e)).is(errtype);
    } catch(...) {
        throw expecterror("expected exception of type " +
            classname(errtype));
    }
  }

}

inline expecterror :: expecterror() {}
inline expecterror :: expecterror(const std::string& msg) :
    _msg(msg) {}

inline const char* expecterror :: what() const throw()
  { return _msg.c_str(); }

};

#endif //__CPPUNITTEST_EXPECT_IMPL__

