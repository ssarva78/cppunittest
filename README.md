# cppunittest

A simple utility to define and run unit test cases for C++. It makes the definition of test cases and assertions very intuitive and readable that it naturally promotes test driven development.

Unlike typical JUnits for Java, this utility doesn't provide mechanism to mock classes and methods. Instead, it provides a way to define unit test cases that are simple to read and execute.

## Installation
Pull the code from github and run `make install` to install the libraries.
The header definition files are installed under `/usr/local/include/cppunittest`
and the shared object is installed as `/usr/local/lib/libcppunittest.so`

Run `make docs` to generate class documentation for cppunittest.

`make uninstall` to remove cppunittest from the system.

## Usage
### cppunittest :: expect
Assertion utility to assert the actual value with the expected value. It supports only following types of values
* Primitive arithmetic types - bool, char, short, int, float, double and their long and unsigned equivalents where applicable
* std::string and its derived classes
* std::type_info - to assert type of a variable
* std::function<void()> - to assert exception from the function


Assertions method | use
------- | --------------------------
__is()__ | asserts that the actual and the expected values/types are same
__isnot()__ | asserts that the actual and the expected values/types are not same
__istrue()__ | asserts that the actual value is true
__isfalse()__ | asserts that the actual value is false
__less_than()__ | asserts that the actual is less than the expected
__not_less_than()__ | asserts that the actual is greater than or equal to expected
__greater_than()__ | asserts that the actual is greater than the expected
__not_greater_than()__ | asserts that the actual is less than or equal to expected
__has()__ | asserts that the actual string has expected string as a substring
__throws()__ | asserts an exception is thrown from a function

These methods also have overloaded operator equivalents
operator | method equivalent
--------|-----------
__==__ | is
__!=__ | isnot
__<__ | less_than
__<=__ | not_greater_than
__>__ | greater_than
__>=__ | not_less_than

Assertion method throws __cppunittest :: expecterror__ when assertion fails. `cppunittest :: excepterror` is derived from `std :: exception`.

```C++

// To assert integer values
int x = 5;
expect<int>(x) == 5; // same as expect<int>(x).is(5);
expect<int>(x).is(7); // throws expecterror exception

expect<std::string>("helloworld").has("hello");

// To assert types of a variable
class C {};
C c;
expect<std::type_info>(typeid(c)).is(typeid(C));

```

__expectthrow__ is a convenience typedef equivalent to expect<std::function<void()>>

```C++
// asserts exception type
expectthrow(__testfunc__(
  throw std::bad_cast();
)).throws(typeid(std::bad_cast));
 
// asserts exception type and exception message
class myerror : public std::exception {...}
expectthrow(__testfunc__(
  throw myerror("this is my error");
)).throws(typeid(myerror), "this is my error");
```

### cppunittest :: unittest

Test methods, setup and teardown methods are defined through `cppunittest :: unittest` class.

Methods | Usage
--------|-----------
__unittest(std::string, std::string, result_writer)__ | Constructor. The first argument is a textual description of the collection of test methods.<br>The second argument is the filename where the testcases are defined.<br>Third argument is optional and specifies where the test executions are logged. If not provided, the executions are logged to stdout. Passing instance of default_xml_writer will produce unittest report file in the format that sonarqube recognizes.
__unittest& setup (std::function<void()>)__ | Takes function as the argument that has logic to perform all initialization steps required to run test cases. This method can be called any number of times in between testcases
__unittest& test (std::function<void()>)__ | Takes function as the argument that has steps to run the test case and its assertions.
__unittest& teardown (std::function<void()>)__ | Takes function as the argument that does clean up and tear down of any data setup in `setup` method.
__void summarize()__ | Finalizes the logging of testcase execution status
__int success()__ | Returns the count of passed test methods
__int failure()__ | Returns the count of failed test methods
__int error()__ | Returns the count of tests that resulted in unexpected exceptions
__std::string desc()__ | Returns the description of the test collection that was set in the constructor

* `setup` and `teardown` are not mandatory

A complete example of a test case collection

```C++
class fizzbuzzerror : public std::exception {
public:
  const char* what() const throw() { return "value should be greater than 0"; }
};

std::string fizzorbuzz(int n) {

  if (n <= 0) throw fizzbuzzerror();

  return n%15 == 0? "fizzbuzz"
    : n%5 == 0? "buzz"
    : n%3 == 0? "fizz"
    : std::to_string(n);
}

int testfizzbuzz() {
  int i = 0;

  unittest tst("FizzBuzz Test", __FILE__);

  tst
  .setup(__testfunc__(
    i = 6;
  ))
  .test("Divisibility by 3", __testfunc__(
    expect<std::string>(fizzorbuzz(i)).is("fizz");
  ))
  .test("Divisibility by 5", __testfunc__(
    expect<std::string>(fizzorbuzz(5)).is("buzz");
  ))
  .test("Others", __testfunc__(
    expect<std::string>(fizzorbuzz(1)).is("1");
  ))
  .teardown([&]() -> void {
    i = 0;
  })
  .summarize();
```

