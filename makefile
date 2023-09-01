CC := g++
INCLUDE := -I include
STD := c++20
CPPFLAGS := -Wall --std=$(STD) 
COVFLAGS := -fprofile-arcs -ftest-coverage
LIB_DIR := ./lib
LIB_NAME := $(LIB_DIR)/libcppunittest.so
TEST_DIR := test
REPORT_DIR := reports
LNK := -L$(LIB_DIR) -lcppunittest

ifeq ($(PREFIX),)
	PREFIX := /usr/local
endif

CPPCHECK := $(shell which cppcheck 2>/dev/null)
SYSTYPE := $(shell uname)

HEADER_INSTALL_DIR=$(PREFIX)/include/cppunittest
LIB_INSTALL_DIR=$(PREFIX)/lib

.PHONY: $(wildcard test/*.cpp)

all: clean verify

codecheck:
	@echo cppcheck=$(CPPCHECK)
ifeq ($(CPPCHECK),)
	@echo cppcheck not installed. skipping static code analysis...
else
	@echo static code analysis...
	@cppcheck --enable=all --language=c++ --error-exitcode=1 -I ./include --suppress=missingIncludeSystem  .
endif

build: 
	@echo build shared object...
	@mkdir -p $(LIB_DIR)
	@$(CC) $(CPPFLAGS) $(COVFLAGS) -fPIC -shared -o $(LIB_NAME) $(INCLUDE) impl/*.cpp

verify: $(wildcard test/*.cpp) codecheck coverage

test/testunittest.cpp: build
	@$(eval EXEC := $(basename $@))
	@echo $(EXEC)
	@$(CC) $(CPPFLAGS) $(COVFLAGS) $(INCLUDE) -o $(EXEC) $@ $(LNK)
	@LD_LIBRARY_PATH=$(LIB_DIR) $(EXEC)
	@echo

test/testdefaultxmlwriter.cpp: build
	@$(eval EXEC := $(basename $@))
	@echo $(EXEC)
	@$(CC) $(CPPFLAGS) $(COVFLAGS) $(INCLUDE) -o $(EXEC) $@ $(LNK)
	@LD_LIBRARY_PATH=$(LIB_DIR) $(EXEC)
	@echo

coverage:
	@echo systype=$(SYSTYPE)
ifeq ($(SYSTYPE), Darwin)
	@mv unittest.gcno impl/
	@mv unittest.gcda impl/
	@mv testlogger.gcno impl/
	@mv testlogger.gcda impl/
	@mv testdefaultxmlwriter.gcno test/
	@mv testdefaultxmlwriter.gcda test/
	@mv testunittest.gcno test/
	@mv testunittest.gcda test/
else
	@mv lib/libcppunittest.so-unittest.gcno impl/unittest.gcno
	@mv lib/libcppunittest.so-unittest.gcda impl/unittest.gcda
	@mv lib/libcppunittest.so-testlogger.gcno impl/testlogger.gcno
	@mv lib/libcppunittest.so-testlogger.gcda impl/testlogger.gcda
endif
	@gcov --relative-only --demangled-names impl/*.cpp test/*.cpp
	@mkdir -p $(REPORT_DIR)
	@mv *.gcov $(REPORT_DIR)/

cppunittest: verify clean
	@echo build shared object...
	@mkdir -p $(LIB_DIR)
	@$(CC) $(CPPFLAGS) -O3 -fPIC -shared -o $(LIB_DIR)/libcppunittest.so $(INCLUDE) impl/*.cpp

install: cppunittest
	@echo install cppunittest library and headers
	@mkdir -p $(HEADER_INSTALL_DIR)
	@cp -r include/* $(HEADER_INSTALL_DIR)
	@cp lib/libcppunittest.so $(LIB_INSTALL_DIR)

uninstall:
	@echo uninstall cppunittest
	@rm -rf $(PREFIX)/include/cppunittest
	@rm -f $(LIB_INSTALL_DIR)/libcppunittest.so

clean:
	@echo clean up
	@find $(TEST_DIR) -type f ! -name "*.cpp" -exec rm {} +
	@rm -f testxmlwriter.xml
	@rm -f $(LIB_NAME)
	@rm -f **/*.gcno **/*.gcda $(REPORT_DIR)/*.gcov

docs: clean
	@echo generate documentation
	@rm -rf docs
	@doxygen

