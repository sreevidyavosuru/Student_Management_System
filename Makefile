CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Iinclude
SQLITE_CFLAGS := $(shell pkg-config --cflags sqlite3 2>/dev/null)
SQLITE_LIBS := $(shell pkg-config --libs sqlite3 2>/dev/null || echo -lsqlite3)
GTEST_CFLAGS ?=
GTEST_LIBS ?= -lgtest -lgtest_main -pthread

SRC := src/Student.cpp src/Database.cpp src/StudentManager.cpp
APP_SRC := $(SRC) src/main.cpp
APP := build/student_management
TEST_SRC := tests/StudentTest.cpp tests/DatabaseTest.cpp tests/StudentManagerTest.cpp
TEST_BIN := build/student_tests

.PHONY: all run test cppcheck valgrind coverage clean help

all: $(APP)

build:
	mkdir -p build

$(APP): $(APP_SRC) | build
	$(CXX) $(CXXFLAGS) $(SQLITE_CFLAGS) $(APP_SRC) -o $@ $(SQLITE_LIBS)

$(TEST_BIN): $(SRC) $(TEST_SRC) | build
	$(CXX) $(CXXFLAGS) $(SQLITE_CFLAGS) $(GTEST_CFLAGS) $(SRC) $(TEST_SRC) \
		-o $@ $(SQLITE_LIBS) $(GTEST_LIBS)

run: $(APP)
	./$(APP)

test: $(TEST_BIN)
	./$(TEST_BIN)

cppcheck:
	cppcheck --enable=warning,performance,portability,style \
		--std=c++17 --language=c++ --error-exitcode=1 \
		-I include src tests

valgrind: $(TEST_BIN)
	valgrind --leak-check=full --show-leak-kinds=definite,indirect \
		--errors-for-leak-kinds=definite,indirect --error-exitcode=1 ./$(TEST_BIN)

coverage: clean | build
	$(CXX) $(CXXFLAGS) $(SQLITE_CFLAGS) --coverage -O0 -g \
		$(SRC) $(TEST_SRC) -o $(TEST_BIN) $(SQLITE_LIBS) $(GTEST_LIBS)
	./$(TEST_BIN)
	@if command -v gcovr >/dev/null 2>&1; then \
		gcovr --root . --filter 'src/' --print-summary \
			--html-details build/coverage.html; \
		echo "HTML report: build/coverage.html"; \
	else \
		echo "gcovr not found; generating gcov text reports."; \
		find . -name '*.gcno' -exec gcov -b -c {} +; \
	fi

clean:
	rm -rf build *.gcda *.gcno src/*.gcda src/*.gcno tests/*.gcda tests/*.gcno

help:
	@echo "Targets: all, run, test, cppcheck, valgrind, coverage, clean"
