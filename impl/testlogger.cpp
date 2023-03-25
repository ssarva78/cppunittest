#ifndef __CPPUNITTEST_TESTLOGGER_IMPL__
#define __CPPUNITTEST_TESTLOGGER_IMPL__

#include <testlogger>
#include <fstream>

namespace cppunittest {

structured_writer :: structured_writer() :
    _enable_console(true),
    _success(0), _failure(0), _error(0)
  {}

void structured_writer :: describe(
    const std::string& desc,
    const std::string& file) {
  if (_enable_console)
    stream_writer().describe(desc, file);
  _desc = desc;
  _file = file;
};

void structured_writer :: result(const std::string& desc,
    const std::string& status,
    const std::string& errmsg,
    long duration) {
  if (_enable_console)
    stream_writer().result(desc, status, errmsg, duration);
  _testcases.push_back({desc, status, errmsg, duration});
}

void structured_writer :: write(const std::string& fname) {
  std::fstream file(fname, std::ios::out | std::ios::trunc);
  write(file);
  file.close();
}

void structured_writer :: summary(
    const std::string& desc,
    int success, int failure, int error) {
  _success = success;
  _failure = failure;
  _error = error;
  if (_enable_console)
    stream_writer().summary(desc, success, failure, error);
}

bool& structured_writer :: console()
  { return _enable_console; }

default_xml_writer :: default_xml_writer() {}

std::string __escape_xml__(
    std::string s, char espch, std::string espseq) {
  std::string str(s);
  std::string::size_type pos = 0;
  while ((pos = str.find(espch, pos)) != std::string::npos) {
    str.replace(pos, 1, espseq);
    pos += espseq.length();
  }

  return str;
}

std::string __escape_xml__(std::string s) {
  std::string str(s);
  str = __escape_xml__(str, '&', "&amp;");
  str = __escape_xml__(str, '"', "&quot;");
  str = __escape_xml__(str, '\'', "&apos;");
  str = __escape_xml__(str, '<', "&lt;");
  str = __escape_xml__(str, '>', "&gt;");
  return str;
}

void __add_tc_xml__(std::ostream& os,
    const testcase& tc) {
  os << "    <testCase name=\"" << __escape_xml__(tc._desc)
      << "\" duration=\"" << (tc._duration / 1000000);
  if (tc._status == "failure"
          || tc._status == "error") {
    os << "\">" << std::endl;
    os << "      <" << tc._status
        << " message=\""
        << __escape_xml__(tc._errmsg) << "\"/>"
        << std::endl
        << "    </testCase>";
  } else {
    os << "\"/>";
  }
  os << std::endl;
}

void default_xml_writer :: write(std::ostream& os) {
  os << "<testExecutions version=\"1\">" << std::endl;
  os << "  <file path=\"" << _file << "\">" << std::endl;
  for (testcase tc : _testcases) {
    __add_tc_xml__(os, tc);
  }
  os << "  </file>" << std::endl;
  os << "</testExecutions>" << std::endl;
}

};

#endif //__CPPUNITTEST_TESTLOGGER_IMPL__

