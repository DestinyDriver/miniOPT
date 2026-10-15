#ifndef MINIO_TEST_UTILS_H
#define MINIO_TEST_UTILS_H

#include <iostream>
#include <string>
#include <cstdlib>

inline int& getFailedTestsCount() {
    static int failed = 0;
    return failed;
}

inline int& getTotalTestsCount() {
    static int total = 0;
    return total;
}

#define TEST_CASE(name) \
    void name(); \
    struct name##_runner { \
        name##_runner() { \
            std::cout << "[ RUN      ] " << #name << std::endl; \
            getTotalTestsCount()++; \
            try { \
                name(); \
                std::cout << "[       OK ] " << #name << std::endl; \
            } catch (const std::exception& e) { \
                std::cerr << "[  FAILED  ] " << #name << ": " << e.what() << std::endl; \
                getFailedTestsCount()++; \
            } catch (...) { \
                std::cerr << "[  FAILED  ] " << #name << ": Unknown exception" << std::endl; \
                getFailedTestsCount()++; \
            } \
        } \
    } name##_runner_instance; \
    void name()

#define ASSERT_TRUE(cond) \
    do { \
        if (!(cond)) { \
            std::cerr << "Assertion failed: " << #cond << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
            throw std::runtime_error("Assertion failed: " #cond); \
        } \
    } while (0)

#define ASSERT_EQ(a, b) \
    do { \
        if (!((a) == (b))) { \
            std::cerr << "Assertion failed: (" << #a << " == " << #b << ") [" << (a) << " vs " << (b) << "] at " \
                      << __FILE__ << ":" << __LINE__ << std::endl; \
            throw std::runtime_error("Assertion failed: " #a " == " #b); \
        } \
    } while (0)

#define ASSERT_THROWS(expr, ExceptionType) \
    do { \
        bool threw = false; \
        try { \
            expr; \
        } catch (const ExceptionType&) { \
            threw = true; \
        } catch (...) { \
            std::cerr << "Wrong exception type thrown by " << #expr << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
            throw std::runtime_error("Wrong exception type thrown"); \
        } \
        if (!threw) { \
            std::cerr << "Expected exception " << #ExceptionType << " from " << #expr << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
            throw std::runtime_error("Expected exception not thrown: " #ExceptionType); \
        } \
    } while (0)

inline int printTestSummary(const std::string& suiteName) {
    std::cout << "========================================\n";
    std::cout << " Test Suite: " << suiteName << "\n";
    std::cout << " Total:  " << getTotalTestsCount() << "\n";
    std::cout << " Passed: " << (getTotalTestsCount() - getFailedTestsCount()) << "\n";
    std::cout << " Failed: " << getFailedTestsCount() << "\n";
    std::cout << "========================================\n";
    return getFailedTestsCount() == 0 ? 0 : 1;
}

#endif // MINIO_TEST_UTILS_H
