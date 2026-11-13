# Student Management System (C++17 + SQLite)

A modular, console-based Student Management System developed as a four-member team Proof of Concept. Student records are persisted in SQLite; the application does not store student data in text, CSV, or binary flat files.

## Features

- Add, list, search, update, and delete student records
- Positive, unique student IDs enforced by SQLite primary key and application validation
- Name and course validation; age restricted to 1–120
- Optional email with basic format validation
- Parameterized SQL statements
- RAII cleanup for SQLite connection and prepared statements
- Separate temporary databases for unit tests
- GoogleTest, Cppcheck, Valgrind, gcov and optional gcovr targets

## Architecture

```text
Console UI (src/main.cpp)
          |
          v
StudentManager (include/StudentManager.h)
          |
          v
Database (include/Database.h)
          |
          v
SQLite database (database/students.db)

Student (include/Student.h) provides validated student data
```

### Source layout

```text
StudentManagementSystem/
├── include/
│   ├── Student.h
│   ├── StudentManager.h
│   └── Database.h
├── src/
│   ├── Student.cpp
│   ├── StudentManager.cpp
│   ├── Database.cpp
│   └── main.cpp
├── tests/
│   ├── StudentTest.cpp
│   ├── StudentManagerTest.cpp
│   └── DatabaseTest.cpp
├── database/
│   └── .gitkeep
├── Makefile
├── README.md
├── .gitignore
└── LICENSE
```

## Technologies

- C++17
- GCC/G++
- SQLite 3 C API
- GoogleTest
- Cppcheck
- Valgrind
- gcov and optional gcovr
- GNU Make

## Prerequisites (Red Hat Linux)

Install or ask your system administrator to provide:
- GCC/G++ with C++17 support
- GNU Make
- SQLite runtime and development headers
- GoogleTest development library
- Cppcheck, Valgrind, gcov
- Optional: gcovr

Check availability:

```bash
g++ --version
make --version
pkg-config --modversion sqlite3
pkg-config --cflags --libs sqlite3
test -f /usr/include/sqlite3.h && echo "SQLite header found"
ldconfig -p 2>/dev/null | grep sqlite3
pkg-config --modversion gtest 2>/dev/null || echo "Check GoogleTest installation"
cppcheck --version
valgrind --version
gcov --version
gcovr --version
```

Package names and repository availability differ by Red Hat release and organizational image. On RHEL-family systems, use the approved repositories and your administrator's package process. Common package names include `gcc-c++`, `make`, `sqlite`, `sqlite-devel`, `cppcheck`, `valgrind`, and `gcovr`. GoogleTest may be supplied as a development package or built/installed from the distribution's approved source package. Do not assume `sudo` or internet access on a company server.

If GoogleTest is installed in a non-standard location, provide its include and library flags when invoking make, for example:

```bash
make test GTEST_CFLAGS="-I/approved/gtest/include" \
  GTEST_LIBS="-L/approved/gtest/lib -lgtest -lgtest_main -pthread"
```

If SQLite's `pkg-config` metadata is unavailable but its header and library are installed, the Makefile falls back to `-lsqlite3`. If SQLite is missing, install the approved SQLite development package before building.

## Build and run

From the project root:

```bash
make
make run
```

The first application start creates `database/students.db` and the `students` table automatically. The database file is runtime data and is ignored by Git.

Alternatively:

```bash
./build/student_management
```

## Unit tests

```bash
make test
```

Tests create unique database files under the operating system temporary directory and remove them during teardown. They do not open `database/students.db`.

To run one named test:

```bash
./build/student_tests --gtest_filter="StudentTest.*"
```

## Static analysis

```bash
make cppcheck
```

Cppcheck is configured for warning, performance, portability, and style checks. Review each warning's file and line number. Fix real defects first; for example, check array/index bounds, null pointers, unused variables, resource lifetime, and implicit conversions. Do not suppress a warning merely to obtain a clean report. If a warning is a verified false positive, document the reason and use a narrowly scoped suppression.

## Valgrind

```bash
make valgrind
```

The target runs the unit tests under Valgrind. A clean run should report no definite or indirect memory leaks and no memory errors. SQLite and GoogleTest may retain process-lifetime allocations reported as still reachable; inspect the full report rather than treating every reachable byte as a leak. The target returns a non-zero status for definite/indirect leaks or memory errors.

## Code coverage

```bash
make coverage
```

The target cleans previous build output, recompiles tests with GCC coverage instrumentation, and executes them. If `gcovr` is installed, it prints a summary and writes detailed HTML under `build/coverage.html`. Otherwise it attempts to generate `.gcov` text reports using `gcov`.

Coverage measures which instrumented lines/branches executed; it does not prove correctness. Use the report to find untested functions and branches, then add tests for both successful and failure paths. Run `make clean` before a normal non-coverage rebuild.

## Database communication flow

1. `main.cpp` creates `Database("database/students.db")`.
2. `Database` opens SQLite using `sqlite3_open`.
3. `initialize()` creates the table if it does not exist.
4. The console gathers user input and constructs a validated `Student`.
5. `StudentManager` delegates the requested CRUD operation to `Database`.
6. `Database` prepares parameterized SQL, binds values, executes it, and checks SQLite's result.
7. The prepared statement is finalized automatically; the connection is closed when the `Database` object leaves scope.

## Four-member team contribution plan

| Member | Ownership | Deliverables |
|---|---|---|
| 1 | Student model | `Student.h`, `Student.cpp`, `StudentTest.cpp` |
| 2 | Application operations | `StudentManager.h`, `StudentManager.cpp`, manager tests |
| 3 | Persistence | `Database.h`, `Database.cpp`, database tests |
| 4 | Integration and quality | `main.cpp`, Makefile, README, `.gitignore`, quality checks |

### Integration rules

- Agree on the public declarations in `include/` before parallel implementation.
- Each member works on a separate feature branch.
- Do not edit another member's implementation without coordinating.
- Member 1 owns the `Student` constructor and validation contract.
- Member 3 owns SQL schema, database path and SQLite resource handling.
- Member 2 calls the public `Database` API; it must not execute SQL directly.
- Member 4 integrates through the published headers and runs the complete test/build checks.
- Merge small pull requests and resolve conflicts before final integration.

## GitHub setup

Create an empty repository on GitHub first. Then, from this directory:

```bash
git init
git add .
git status
git commit -m "Initial Student Management System implementation"
git branch -M main
git remote add origin https://github.com/<YOUR-USERNAME>/<YOUR-REPOSITORY>.git
git push -u origin main
```

Replace the placeholders with your repository details. Never commit credentials, tokens, private server details, or the generated SQLite database. `.gitignore` excludes runtime database files and build artifacts.

For team work, add collaborators in GitHub repository settings and use feature branches plus pull requests.

## Future enhancements

- Sort and filter student lists
- Add pagination for large datasets
- Add stronger email validation and field length limits
- Add transaction support for multi-step operations
- Add database schema versioning/migrations
- Add role-based login and audit history
- Add export functionality only as an explicit user-requested feature (SQLite remains the system of record)
- Add CI workflow for build, tests, and static analysis

## Notes and limitations

- The email expression is a practical format check, not a complete implementation of every valid email standard.
- The current project is a local single-process POC. For concurrent multi-user deployment, review transaction strategy, permissions, backups, and database location.
- `std::filesystem` is used for automatic creation of the database directory and requires a C++17 compiler.
