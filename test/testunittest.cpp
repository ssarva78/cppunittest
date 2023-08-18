#include <unittest>

using namespace cppunittest;

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
    i = 5;
  ))
  .test("Divisibility by 3", __testfunc__(
    expect<int>(i).is(5);
    expect<std::string>(fizzorbuzz(3), "for 3, ").is("fizzr");
    expect<std::string>(fizzorbuzz(6)).is("fizz");
    expect<std::string>(fizzorbuzz(9)).is("fizz");
  ))
  .test("Divisibility by 5", __testfunc__(
    expect<std::string>(fizzorbuzz(5)).is("buzz");
    expect<std::string>(fizzorbuzz(10)).is("buzz");
    expect<std::string>(fizzorbuzz(20)).is("buzz");
  ))
  .test("Divisibility by 3 and 5", __testfunc__(
    expect<std::string>(fizzorbuzz(15)).is("fizzbuzz");
    expect<std::string>(fizzorbuzz(30)).is("fizzbuzz");
    expect<std::string>(fizzorbuzz(45)).is("fizzbuzz");
  ))
  .test("Others", __testfunc__(
    expect<std::string>(fizzorbuzz(1)).is("1");
    expect<std::string>(fizzorbuzz(2)).is("2");
    expect<std::string>(fizzorbuzz(7)).is("7");
  ))
  .setup(__testfunc__(
    i = 10;
  ))
  .test("check after new setup", __testfunc__(
    expect<int>(i).is(5);
  ))
  .test("check is not", __testfunc__(
    expect<int>(5).isnot(6);
    expect<int>(5).isnot(5); // throw expect error
  ))
  .test("check is true", __testfunc__(
    expect<bool>(i == 10).istrue();
    expect<bool>(i != 10).istrue(); // throw expect error
  ))
  .test("check is false", __testfunc__(
    expect<bool>(i == 6).isfalse();
    expect<bool>(i == 10).isfalse(); // throw expect error
  ))
  .test("check numerical less than", __testfunc__(
    expect<int>(5).less_than(10);
    expect<int>(5).less_than(3); // throw expect error
  ))
  .test("check numerical not less than", __testfunc__(
    expect<int>(5).not_less_than(3);
    expect<int>(5).not_less_than(5);
    expect<int>(5).not_less_than(10); // throw expect error
  ))
  .test("check numerical greater than", __testfunc__(
    expect<int>(5).greater_than(1);
    expect<int>(5).greater_than(10); // throw expect error
  ))
  .test("check numerical not greater than", __testfunc__(
    expect<int>(5).not_greater_than(10);
    expect<int>(5).not_greater_than(5);
    expect<int>(5).not_greater_than(1); // throw expect error
  ))
  .test("check substring", __testfunc__(
    expect<std::string>("some text").has("me te");
    expect<std::string>("some text").has("text1"); // throw expect error
  ))
  .test("expect error 1", __testfunc__(
    expectthrow(__testfunc__(fizzorbuzz(-2);)).throws(
        typeid(fizzbuzzerror),
        "value should be greater than 0");
    expectthrow(__testfunc__(fizzorbuzz(-2);)).throws(
        typeid(fizzbuzzerror));
  ))
  .test("expect error 2", __testfunc__(
    expectthrow(__testfunc__(fizzorbuzz(-2);)).throws(
        typeid(fizzbuzzerror),
        "value should be greater than 1"); // expecting test case failure
  ))
  .test("expect error 3", __testfunc__(
    expectthrow(__testfunc__(fizzorbuzz(-2);)).throws(
        typeid(std::bad_cast)); // expecting test case failure
  ))
  .test("expect error 4", __testfunc__(
    expectthrow(__testfunc__(fizzorbuzz(2);)).throws(
        typeid(fizzbuzzerror)); // test case failure as no exception is thrown
  ))
  .test("expect error 5", __testfunc__(
    expectthrow(__testfunc__(fizzorbuzz(2);)).throws(
        typeid(fizzbuzzerror),
        "message");             // test case failure as no exception is thrown
  ))
  .test("expect error 6", __testfunc__(
    expectthrow(__testfunc__(throw 5;)).throws(
        typeid(fizzbuzzerror)); // test case failure as no exception is thrown
  ))
  .test("expect error 7", __testfunc__(
    expectthrow(__testfunc__(throw 5;)).throws(
        typeid(fizzbuzzerror),
        "message");             // test case failure as no exception is thrown
  ))
  .test("unhandled error", __testfunc__(
    throw std::bad_cast();
  ))
  .test("unknown error", __testfunc__(
    throw 5;
  ))
  .teardown([&]() -> void {
    i = 0;
  })
  .summarize();

  std::cout << tst.desc()
    << ": passed=" << tst.success()
    << ", failed=" << tst.failure()
    << ", error=" << tst.error()
    << std::endl;

  return tst.failure() + tst.error();
}

void teststream() {
  std::stringstream ss;
  stream_writer wr(ss);

  unittest tst("Stream Writer Test", __FILE__, &wr);
  tst.test("test stream writer", __testfunc__());
  tst.summarize();

  std::cout << ss.str() << std::endl;
}

int main() {
  testfizzbuzz();
  std::cout << std::endl << std::endl;
  teststream();
  return 0;
}

