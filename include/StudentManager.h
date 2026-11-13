#ifndef STUDENT_MANAGER_H
#define STUDENT_MANAGER_H

#include "Database.h"

#include <optional>
#include <vector>

class StudentManager {
public:
    explicit StudentManager(Database& database);

    void addStudent(const Student& student);
    std::vector<Student> getAllStudents() const;
    std::optional<Student> findStudentById(int id) const;
    bool updateStudent(const Student& student);
    bool removeStudent(int id);

private:
    Database& database_;
};

#endif
