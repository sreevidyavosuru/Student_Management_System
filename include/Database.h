#ifndef DATABASE_H
#define DATABASE_H

#include "Student.h"

#include <optional>
#include <string>
#include <vector>
#include <sqlite3.h>

class Database {
public:
    explicit Database(const std::string& databasePath);
    ~Database();

    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;
    Database(Database&&) = delete;
    Database& operator=(Database&&) = delete;

    void initialize();
    void insertStudent(const Student& student);
    std::vector<Student> getAllStudents() const;
    std::optional<Student> getStudentById(int id) const;
    bool updateStudent(const Student& student);
    bool deleteStudent(int id);

private:
    sqlite3* db_;
    std::string databasePath_;

    void execute(const std::string& sql) const;
    void checkDatabase() const;
};

#endif
