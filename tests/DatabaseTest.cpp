#include "Database.h"
#include <gtest/gtest.h>
#include <chrono>
#include <filesystem>
#include <stdexcept>
#include <string>

class DatabaseTest : public ::testing::Test {
protected:
    std::string path;

    void SetUp() override {
        const auto stamp = std::chrono::steady_clock::now()
                               .time_since_epoch().count();
        path = (std::filesystem::temp_directory_path() /
                ("sms_database_test_" + std::to_string(stamp) + ".db")).string();
    }

    void TearDown() override {
        std::error_code error;
        std::filesystem::remove(path, error);
        std::filesystem::remove(path + "-journal", error);
        std::filesystem::remove(path + "-wal", error);
        std::filesystem::remove(path + "-shm", error);
    }
};

TEST_F(DatabaseTest, InitializesDatabaseAndCreatesTable) {
    Database database(path);
    EXPECT_NO_THROW(database.initialize());
    EXPECT_TRUE(std::filesystem::exists(path));
}

TEST_F(DatabaseTest, InsertsAndRetrievesStudent) {
    Database database(path);
    database.initialize();
    const Student expected(10, "Meera", 21, "CSE", "meera@example.com");
    database.insertStudent(expected);

    const auto actual = database.getStudentById(10);
    ASSERT_TRUE(actual.has_value());
    EXPECT_EQ(actual->getName(), "Meera");
    EXPECT_EQ(actual->getCourse(), "CSE");
    EXPECT_EQ(database.getAllStudents().size(), 1U);
}

TEST_F(DatabaseTest, RejectsDuplicateStudentId) {
    Database database(path);
    database.initialize();
    database.insertStudent(Student(10, "Meera", 21, "CSE"));
    EXPECT_THROW(database.insertStudent(Student(10, "Arun", 22, "ECE")),
                 std::runtime_error);
}

TEST_F(DatabaseTest, ReturnsEmptyForMissingStudent) {
    Database database(path);
    database.initialize();
    EXPECT_FALSE(database.getStudentById(999).has_value());
}

TEST_F(DatabaseTest, UpdatesAndDeletesStudent) {
    Database database(path);
    database.initialize();
    database.insertStudent(Student(10, "Meera", 21, "CSE"));

    EXPECT_TRUE(database.updateStudent(Student(10, "Meera S", 22, "ECE")));
    const auto updated = database.getStudentById(10);
    ASSERT_TRUE(updated.has_value());
    EXPECT_EQ(updated->getName(), "Meera S");
    EXPECT_EQ(updated->getAge(), 22);

    EXPECT_TRUE(database.deleteStudent(10));
    EXPECT_FALSE(database.getStudentById(10).has_value());
    EXPECT_FALSE(database.deleteStudent(10));
}

TEST_F(DatabaseTest, RejectsInvalidSearchAndDeleteIds) {
    Database database(path);
    database.initialize();
    EXPECT_THROW(database.getStudentById(0), std::invalid_argument);
    EXPECT_THROW(database.deleteStudent(-1), std::invalid_argument);
}

TEST_F(DatabaseTest, RejectsInvalidDatabasePath) {
    EXPECT_THROW(Database(""), std::invalid_argument);
}
