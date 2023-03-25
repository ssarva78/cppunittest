#include <unittest>
#include <testlogger>

using namespace cppunittest;

int testxmlwriter(bool console = true) {
  default_xml_writer wr;
  wr.console() = console;
  unittest tst("default xml writer test",
      __FILE__,
      &wr);

  tst.test("test happy path", __testfunc__(
      expect<std::string>("fizz").is("fizz");
  ))
  .test("test error", __testfunc__(
      expect<std::string>("fizz").is("buzz");
  ))
  .test("test happy path2", __testfunc__(
      expect<std::string>("buzz").is("buzz");
  ))
  .test("test unhandled error", __testfunc__(
    throw std::bad_cast();
  ))
  .teardown(__testfunc__())
  .summarize();

  wr.write();
  wr.write("testxmlwriter.xml");

  return tst.failure() + tst.error();
}

int main() {
  testxmlwriter();
  testxmlwriter(false);
  return 0;
}


