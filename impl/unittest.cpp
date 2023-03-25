#ifndef __CPPUNITTEST_IMPL__
#define __CPPUNITTEST_IMPL__

#include <sstream>
#include <sys/time.h>
#include <unittest>
#include <typeutil>

namespace cppunittest {

stream_writer :: stream_writer(std::ostream& os)
    : _os(os.rdbuf())
  {}

void stream_writer :: describe(
    const std::string& desc,
    const std::string& file) {
  _os << "[[ "
      << file << ": " << desc
      << " ]]" << std::endl;
}

void stream_writer :: result(
    const std::string& desc,
    const std::string& status,
    const std::string& errmsg,
    long duration) {

  _os << "[" << status << "] "
      << "[" << desc << "] "
      << errmsg << (errmsg.length() > 0? " " : "")
      << "[" << duration << " ns]"
      << std::endl;
}

void stream_writer :: summary(
    const std::string& desc,
    int success, int failure, int error) {
  _os << "[" << desc << "]"
      << " passed=" << success << ","
      << " failure=" << failure << ","
      << " error=" << error
      << std::endl;
}

unittest :: unittest(const std::string& desc,
    const std::string& filename,
    result_writer* wr) :
    _desc(desc),
    _wr(wr),
    _success(0),
    _failure(0),
    _error(0) {

  _wr -> describe(desc, filename);
}

unittest& unittest :: setup(std::function<void()> func) {
  func();

  return *this;
}

long _now_() {
  timespec time;
  clock_gettime(CLOCK_REALTIME, &time);
  return time.tv_sec * 1000000000 + time.tv_nsec;
}

unittest& unittest :: test(const std::string& desc,
    std::function<void()> func) {

  std::string msg;
  std::string status = "pass";
  long duration = 0;
  long starttime = _now_();
  try {
    func();
    duration = _now_() - starttime;
    _success ++;
  } catch (const expecterror& e) {
    duration = _now_() - starttime;
    status = "failure";
    std::stringstream ss;
    ss << typeutil :: classname(e) << ": " << e.what();
    msg = ss.str();
    _failure ++;
  } catch (const std::exception& e) {
    duration = _now_() - starttime;
    status = "error";
    msg = e.what();
    _error ++;
  } catch (...) {
    duration = _now_() - starttime;
    status = "error";
    msg = "unknown error";
    _error ++;
  }

  _wr -> result(desc, status, msg, duration);

  return *this;
}

unittest& unittest :: teardown(std::function<void()> func) {
  func();
  return *this;
}

void unittest :: summarize() {
   _wr -> summary(_desc, _success, _failure, _error);
}

int unittest :: success() const { return _success; }

int unittest :: failure() const { return _failure; }

int unittest :: error() const { return _error; }

const std::string& unittest :: desc() const
  { return _desc; }

stream_writer unittest :: console = stream_writer();
};

#endif //__CPPUNITTEST_IMPL__

