#ifndef STUDENT_H
#define STUDENT_H

#include <string>

class Student {
public:
    Student(int id, std::string name, int age, std::string course,
            std::string email = "");

    int getId() const noexcept;
    const std::string& getName() const noexcept;
    int getAge() const noexcept;
    const std::string& getCourse() const noexcept;
    const std::string& getEmail() const noexcept;

    void setName(const std::string& name);
    void setAge(int age);
    void setCourse(const std::string& course);
    void setEmail(const std::string& email);

    static bool isValidEmail(const std::string& email);

private:
    int id_;
    std::string name_;
    int age_;
    std::string course_;
    std::string email_;

    static std::string trim(const std::string& value);
    static void validateName(const std::string& name);
    static void validateAge(int age);
    static void validateCourse(const std::string& course);
    static void validateEmail(const std::string& email);
};

#endif
