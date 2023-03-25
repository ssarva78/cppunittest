#ifndef __TYPEUTIL_IMPL__
#define __TYPEUTIL_IMPL__

#include <string>
#include <typeinfo>
#include <iostream>

#ifdef __GNUG__
#include<cxxabi.h>
#endif

namespace typeutil {

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

}; //namespace

#endif //__TYPEUTIL_IMPL__

