#include "Student.h"

#include <algorithm>
#include <cctype>
#include <regex>
#include <stdexcept>
#include <utility>

Student::Student(int id, std::string name, int age, std::string course,
                 std::string email)
    : id_(id), name_(trim(name)), age_(age), course_(trim(course)),
      email_(trim(email)) {
    if (id_ <= 0) {
        throw std::invalid_argument("Student ID must be positive.");
    }
    validateName(name_);
    validateAge(age_);
    validateCourse(course_);
    validateEmail(email_);
}

int Student::getId() const noexcept { return id_; }
const std::string& Student::getName() const noexcept { return name_; }
int Student::getAge() const noexcept { return age_; }
const std::string& Student::getCourse() const noexcept { return course_; }
const std::string& Student::getEmail() const noexcept { return email_; }

void Student::setName(const std::string& name) {
    const std::string cleaned = trim(name);
    validateName(cleaned);
    name_ = cleaned;
}

void Student::setAge(int age) {
    validateAge(age);
    age_ = age;
}

void Student::setCourse(const std::string& course) {
    const std::string cleaned = trim(course);
    validateCourse(cleaned);
    course_ = cleaned;
}

void Student::setEmail(const std::string& email) {
    const std::string cleaned = trim(email);
    validateEmail(cleaned);
    email_ = cleaned;
}

std::string Student::trim(const std::string& value) {
    const auto first = std::find_if_not(value.begin(), value.end(),
        [](unsigned char ch) { return std::isspace(ch); });
    const auto last = std::find_if_not(value.rbegin(), value.rend(),
        [](unsigned char ch) { return std::isspace(ch); }).base();

    if (first >= last) {
        return "";
    }
    return std::string(first, last);
}

void Student::validateName(const std::string& name) {
    if (name.empty()) {
        throw std::invalid_argument("Student name cannot be empty.");
    }
}

void Student::validateAge(int age) {
    if (age < 1 || age > 120) {
        throw std::invalid_argument("Age must be between 1 and 120.");
    }
}

void Student::validateCourse(const std::string& course) {
    if (course.empty()) {
        throw std::invalid_argument("Course cannot be empty.");
    }
}

bool Student::isValidEmail(const std::string& email) {
    if (email.empty()) {
        return true; // Email is optional.
    }
    static const std::regex pattern(
        R"(^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$)");
    return std::regex_match(email, pattern);
}

void Student::validateEmail(const std::string& email) {
    if (!isValidEmail(email)) {
        throw std::invalid_argument("Email address is not valid.");
    }
}
