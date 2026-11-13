#include "Database.h"
#include "StudentManager.h"

#include <iostream>
#include <limits>
#include <optional>
#include <string>

namespace {
int readInteger(const std::string& prompt) {
    int value = 0;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        std::cout << "Invalid input. Please enter a whole number.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

std::string readLine(const std::string& prompt) {
    std::string value;
    std::cout << prompt;
    std::getline(std::cin, value);
    return value;
}

Student readStudentDetails(int id) {
    const std::string name = readLine("Enter student name: ");
    const int age = readInteger("Enter age: ");
    const std::string course = readLine("Enter course: ");
    const std::string email = readLine("Enter email (optional, press Enter to skip): ");
    return Student(id, name, age, course, email);
}

void printStudent(const Student& student) {
    std::cout << "\nID: " << student.getId()
              << "\nName: " << student.getName()
              << "\nAge: " << student.getAge()
              << "\nCourse: " << student.getCourse()
              << "\nEmail: "
              << (student.getEmail().empty() ? "Not provided" : student.getEmail())
              << "\n-----------------------------\n";
}

void showMenu() {
    std::cout << "\n===== STUDENT MANAGEMENT SYSTEM =====\n"
              << "1. Add Student\n"
              << "2. Display All Students\n"
              << "3. Search Student by ID\n"
              << "4. Update Student Details\n"
              << "5. Delete Student\n"
              << "6. Exit\n";
}
} // namespace

int main() {
    try {
        Database database("database/students.db");
        database.initialize();
        StudentManager manager(database);

        while (true) {
            showMenu();
            const int choice = readInteger("Enter your choice: ");

            try {
                if (choice == 1) {
                    const int id = readInteger("Enter student ID: ");
                    const Student student = readStudentDetails(id);
                    manager.addStudent(student);
                    std::cout << "Student added successfully.\n";
                } else if (choice == 2) {
                    const auto students = manager.getAllStudents();
                    if (students.empty()) {
                        std::cout << "No student records found.\n";
                    } else {
                        for (const auto& student : students) {
                            printStudent(student);
                        }
                    }
                } else if (choice == 3) {
                    const int id = readInteger("Enter student ID to search: ");
                    const auto student = manager.findStudentById(id);
                    if (student) {
                        printStudent(*student);
                    } else {
                        std::cout << "Student not found.\n";
                    }
                } else if (choice == 4) {
                    const int id = readInteger("Enter student ID to update: ");
                    if (!manager.findStudentById(id)) {
                        std::cout << "Student not found.\n";
                        continue;
                    }
                    const Student updated = readStudentDetails(id);
                    if (manager.updateStudent(updated)) {
                        std::cout << "Student updated successfully.\n";
                    } else {
                        std::cout << "No changes were made.\n";
                    }
                } else if (choice == 5) {
                    const int id = readInteger("Enter student ID to delete: ");
                    if (manager.removeStudent(id)) {
                        std::cout << "Student deleted successfully.\n";
                    } else {
                        std::cout << "Student not found.\n";
                    }
                } else if (choice == 6) {
                    std::cout << "Exiting Student Management System. Goodbye!\n";
                    break;
                } else {
                    std::cout << "Please choose a number between 1 and 6.\n";
                }
            } catch (const std::exception& error) {
                std::cout << "Operation failed: " << error.what() << '\n';
            }
        }
    } catch (const std::exception& error) {
        std::cerr << "Application startup failed: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
