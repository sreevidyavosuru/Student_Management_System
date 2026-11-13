#include "Database.h"

#include <filesystem>
#include <memory>
#include <stdexcept>
#include <string>

namespace {
class Statement {
public:
    Statement(sqlite3* db, const std::string& sql) : statement_(nullptr) {
        const int result = sqlite3_prepare_v2(db, sql.c_str(), -1,
                                              &statement_, nullptr);
        if (result != SQLITE_OK) {
            throw std::runtime_error(
                std::string("SQL prepare failed: ") + sqlite3_errmsg(db));
        }
    }

    ~Statement() {
        if (statement_ != nullptr) {
            sqlite3_finalize(statement_);
        }
    }

    Statement(const Statement&) = delete;
    Statement& operator=(const Statement&) = delete;

    sqlite3_stmt* get() const noexcept { return statement_; }

private:
    sqlite3_stmt* statement_;
};

void bindText(sqlite3* db, sqlite3_stmt* statement, int index,
              const std::string& value) {
    const int result = sqlite3_bind_text(statement, index, value.c_str(),
                                         -1, SQLITE_TRANSIENT);
    if (result != SQLITE_OK) {
        throw std::runtime_error(
            std::string("SQL bind failed: ") + sqlite3_errmsg(db));
    }
}

void checkStep(sqlite3* db, int result, const std::string& operation) {
    if (result != SQLITE_DONE && result != SQLITE_ROW) {
        throw std::runtime_error(operation + ": " + sqlite3_errmsg(db));
    }
}

Student readStudent(sqlite3_stmt* statement) {
    const int id = sqlite3_column_int(statement, 0);
    const auto* nameText = sqlite3_column_text(statement, 1);
    const int age = sqlite3_column_int(statement, 2);
    const auto* courseText = sqlite3_column_text(statement, 3);
    const auto* emailText = sqlite3_column_text(statement, 4);

    const std::string name = nameText
        ? reinterpret_cast<const char*>(nameText) : "";
    const std::string course = courseText
        ? reinterpret_cast<const char*>(courseText) : "";
    const std::string email = emailText
        ? reinterpret_cast<const char*>(emailText) : "";

    return Student(id, name, age, course, email);
}
} // namespace

Database::Database(const std::string& databasePath)
    : db_(nullptr), databasePath_(databasePath) {
    if (databasePath_.empty()) {
        throw std::invalid_argument("Database path cannot be empty.");
    }

    const std::filesystem::path path(databasePath_);
    if (path.has_parent_path()) {
        std::filesystem::create_directories(path.parent_path());
    }

    const int result = sqlite3_open(databasePath_.c_str(), &db_);
    if (result != SQLITE_OK) {
        const std::string message = db_
            ? sqlite3_errmsg(db_) : "Unable to allocate SQLite connection.";
        if (db_ != nullptr) {
            sqlite3_close(db_);
            db_ = nullptr;
        }
        throw std::runtime_error("Cannot open database: " + message);
    }

    sqlite3_busy_timeout(db_, 3000);
}

Database::~Database() {
    if (db_ != nullptr) {
        sqlite3_close(db_);
    }
}

void Database::checkDatabase() const {
    if (db_ == nullptr) {
        throw std::runtime_error("Database connection is not available.");
    }
}

void Database::execute(const std::string& sql) const {
    checkDatabase();
    char* errorMessage = nullptr;
    const int result = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr,
                                    &errorMessage);
    if (result != SQLITE_OK) {
        const std::string message = errorMessage
            ? errorMessage : sqlite3_errmsg(db_);
        sqlite3_free(errorMessage);
        throw std::runtime_error("SQL execution failed: " + message);
    }
}

void Database::initialize() {
    checkDatabase();
    execute("PRAGMA foreign_keys = ON;");
    execute(
        "CREATE TABLE IF NOT EXISTS students ("
        "student_id INTEGER PRIMARY KEY CHECK(student_id > 0),"
        "name TEXT NOT NULL CHECK(length(trim(name)) > 0),"
        "age INTEGER NOT NULL CHECK(age BETWEEN 1 AND 120),"
        "course TEXT NOT NULL CHECK(length(trim(course)) > 0),"
        "email TEXT NOT NULL DEFAULT ''"
        ");");
}

void Database::insertStudent(const Student& student) {
    checkDatabase();
    Statement statement(db_,
        "INSERT INTO students (student_id, name, age, course, email) "
        "VALUES (?, ?, ?, ?, ?);");

    sqlite3_stmt* query = statement.get();
    if (sqlite3_bind_int(query, 1, student.getId()) != SQLITE_OK ||
        sqlite3_bind_int(query, 3, student.getAge()) != SQLITE_OK) {
        throw std::runtime_error(std::string("SQL bind failed: ") +
                                 sqlite3_errmsg(db_));
    }
    bindText(db_, query, 2, student.getName());
    bindText(db_, query, 4, student.getCourse());
    bindText(db_, query, 5, student.getEmail());
    checkStep(db_, sqlite3_step(query), "Insert student failed");
}

std::vector<Student> Database::getAllStudents() const {
    checkDatabase();
    Statement statement(db_,
        "SELECT student_id, name, age, course, email "
        "FROM students ORDER BY student_id;");
    std::vector<Student> students;
    int result = SQLITE_OK;

    while ((result = sqlite3_step(statement.get())) == SQLITE_ROW) {
        students.push_back(readStudent(statement.get()));
    }
    if (result != SQLITE_DONE) {
        throw std::runtime_error(std::string("Read students failed: ") +
                                 sqlite3_errmsg(db_));
    }
    return students;
}

std::optional<Student> Database::getStudentById(int id) const {
    checkDatabase();
    if (id <= 0) {
        throw std::invalid_argument("Student ID must be positive.");
    }

    Statement statement(db_,
        "SELECT student_id, name, age, course, email "
        "FROM students WHERE student_id = ?;");
    if (sqlite3_bind_int(statement.get(), 1, id) != SQLITE_OK) {
        throw std::runtime_error(std::string("SQL bind failed: ") +
                                 sqlite3_errmsg(db_));
    }

    const int result = sqlite3_step(statement.get());
    if (result == SQLITE_DONE) {
        return std::nullopt;
    }
    if (result != SQLITE_ROW) {
        throw std::runtime_error(std::string("Search failed: ") +
                                 sqlite3_errmsg(db_));
    }
    return readStudent(statement.get());
}

bool Database::updateStudent(const Student& student) {
    checkDatabase();
    Statement statement(db_,
        "UPDATE students SET name = ?, age = ?, course = ?, email = ? "
        "WHERE student_id = ?;");

    sqlite3_stmt* query = statement.get();
    bindText(db_, query, 1, student.getName());
    if (sqlite3_bind_int(query, 2, student.getAge()) != SQLITE_OK ||
        sqlite3_bind_int(query, 5, student.getId()) != SQLITE_OK) {
        throw std::runtime_error(std::string("SQL bind failed: ") +
                                 sqlite3_errmsg(db_));
    }
    bindText(db_, query, 3, student.getCourse());
    bindText(db_, query, 4, student.getEmail());
    checkStep(db_, sqlite3_step(query), "Update student failed");
    return sqlite3_changes(db_) > 0;
}

bool Database::deleteStudent(int id) {
    checkDatabase();
    if (id <= 0) {
        throw std::invalid_argument("Student ID must be positive.");
    }

    Statement statement(db_, "DELETE FROM students WHERE student_id = ?;");
    if (sqlite3_bind_int(statement.get(), 1, id) != SQLITE_OK) {
        throw std::runtime_error(std::string("SQL bind failed: ") +
                                 sqlite3_errmsg(db_));
    }
    checkStep(db_, sqlite3_step(statement.get()), "Delete student failed");
    return sqlite3_changes(db_) > 0;
}
