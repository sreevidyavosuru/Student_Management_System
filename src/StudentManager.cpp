#include "StudentManager.h"

StudentManager::StudentManager(Database& database) : database_(database) {}

void StudentManager::addStudent(const Student& student) {
    database_.insertStudent(student);
}

std::vector<Student> StudentManager::getAllStudents() const {
    return database_.getAllStudents();
}

std::optional<Student> StudentManager::findStudentById(int id) const {
    return database_.getStudentById(id);
}

bool StudentManager::updateStudent(const Student& student) {
    return database_.updateStudent(student);
}

bool StudentManager::removeStudent(int id) {
    return database_.deleteStudent(id);
}
