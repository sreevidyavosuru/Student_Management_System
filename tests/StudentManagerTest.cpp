#include "StudentManager.h"
#include <gtest/gtest.h>
#include <chrono>
#include <filesystem>
#include <memory>
#include <string>

class StudentManagerTest : public ::testing::Test {
protected:
    std::string path;
    std::unique_ptr<Database> database;
    std::unique_ptr<StudentManager> manager;

    void SetUp() override {
        const auto stamp = std::chrono::steady_clock::now()
                               .time_since_epoch().count();
        path = (std::filesystem::temp_directory_path() /
                ("sms_manager_test_" + std::to_string(stamp) + ".db")).string();
        database = std::make_unique<Database>(path);
        database->initialize();
        manager = std::make_unique<StudentManager>(*database);
    }

    void TearDown() override {
        manager.reset();
        database.reset();
        std::error_code error;
        std::filesystem::remove(path, error);
        std::filesystem::remove(path + "-journal", error);
        std::filesystem::remove(path + "-wal", error);
        std::filesystem::remove(path + "-shm", error);
    }
};

TEST_F(StudentManagerTest, AddsAndFindsStudent) {
    manager->addStudent(Student(1, "Asha", 20, "CSE"));
    const auto student = manager->findStudentById(1);
    ASSERT_TRUE(student.has_value());
    EXPECT_EQ(student->getName(), "Asha");
}

TEST_F(StudentManagerTest, RejectsDuplicateId) {
    manager->addStudent(Student(1, "Asha", 20, "CSE"));
    EXPECT_THROW(manager->addStudent(Student(1, "Nikhil", 21, "ECE")),
                 std::runtime_error);
}

TEST_F(StudentManagerTest, FindsNoStudentForUnknownId) {
    EXPECT_FALSE(manager->findStudentById(404).has_value());
}

TEST_F(StudentManagerTest, UpdatesStudentDetails) {
    manager->addStudent(Student(1, "Asha", 20, "CSE"));
    EXPECT_TRUE(manager->updateStudent(Student(1, "Asha R", 21, "IT")));
    const auto student = manager->findStudentById(1);
    ASSERT_TRUE(student.has_value());
    EXPECT_EQ(student->getName(), "Asha R");
    EXPECT_EQ(student->getCourse(), "IT");
}

TEST_F(StudentManagerTest, DeletesStudent) {
    manager->addStudent(Student(1, "Asha", 20, "CSE"));
    EXPECT_TRUE(manager->removeStudent(1));
    EXPECT_FALSE(manager->findStudentById(1).has_value());
}

TEST_F(StudentManagerTest, ListsAllStudents) {
    manager->addStudent(Student(1, "Asha", 20, "CSE"));
    manager->addStudent(Student(2, "Nikhil", 21, "ECE"));
    EXPECT_EQ(manager->getAllStudents().size(), 2U);
}
