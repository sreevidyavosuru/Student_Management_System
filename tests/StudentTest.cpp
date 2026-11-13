#include "Student.h"
#include <gtest/gtest.h>
#include <stdexcept>

TEST(StudentTest, CreatesStudentWithValidData) {
    const Student student(1, "Ananya Rao", 21, "CSE", "ananya@example.com");
    EXPECT_EQ(student.getId(), 1);
    EXPECT_EQ(student.getName(), "Ananya Rao");
    EXPECT_EQ(student.getAge(), 21);
    EXPECT_EQ(student.getCourse(), "CSE");
    EXPECT_EQ(student.getEmail(), "ananya@example.com");
}

TEST(StudentTest, TrimsNameAndCourse) {
    const Student student(2, "  Ravi  ", 20, "  ECE  ");
    EXPECT_EQ(student.getName(), "Ravi");
    EXPECT_EQ(student.getCourse(), "ECE");
}

TEST(StudentTest, RejectsNonPositiveId) {
    EXPECT_THROW(Student(0, "Ravi", 20, "CSE"), std::invalid_argument);
}

TEST(StudentTest, RejectsEmptyName) {
    EXPECT_THROW(Student(1, "   ", 20, "CSE"), std::invalid_argument);
}

TEST(StudentTest, RejectsAgeOutsideRange) {
    EXPECT_THROW(Student(1, "Ravi", 0, "CSE"), std::invalid_argument);
    EXPECT_THROW(Student(1, "Ravi", 121, "CSE"), std::invalid_argument);
}

TEST(StudentTest, RejectsEmptyCourse) {
    EXPECT_THROW(Student(1, "Ravi", 20, " "), std::invalid_argument);
}

TEST(StudentTest, EmailIsOptionalButValidatedWhenPresent) {
    EXPECT_NO_THROW(Student(1, "Ravi", 20, "CSE"));
    EXPECT_THROW(Student(1, "Ravi", 20, "CSE", "not-an-email"),
                 std::invalid_argument);
}

TEST(StudentTest, SettersValidateNewValues) {
    Student student(1, "Ravi", 20, "CSE");
    student.setName("Kiran");
    student.setAge(22);
    student.setCourse("ECE");
    student.setEmail("kiran@example.com");
    EXPECT_EQ(student.getName(), "Kiran");
    EXPECT_EQ(student.getAge(), 22);
    EXPECT_EQ(student.getCourse(), "ECE");
    EXPECT_EQ(student.getEmail(), "kiran@example.com");
    EXPECT_THROW(student.setAge(0), std::invalid_argument);
}
