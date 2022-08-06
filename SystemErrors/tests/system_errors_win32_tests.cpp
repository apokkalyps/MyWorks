//
// Created by MartinF on 31/07/2022.
//

#if MF_WINDOWS
#    include "MF/LightWindows.hpp"
#    include "MF/SystemErrors.hpp"
#    include "tests_data.hpp"

using MF::SystemErrors::Win32;
using ErrorCode_t = Win32::ErrorCode_t;

static ErrorCode_t doSomethingThatSetsLastError() {
    const auto result = GetProcessId(nullptr);
    EXPECT_EQ(result, DWORD(0));

    return ERROR_INVALID_HANDLE;
}

static void doSomethingThatDoesNotSetsLastError() {
    const std::string str = std::to_string(42);
    int convertedValue = atoi(str.c_str());
}

TEST(Win32_ThrowCurrentSystemErrorIf, it_throws_if_true) {
    const auto expectedError = doSomethingThatSetsLastError();

    try {
        Win32::throwCurrentSystemErrorIf(true);
        FAIL() << "Expected an exception to be thrown.";
    } catch (const std::system_error& systemError) {
        EXPECT_EQ(systemError.code().value(), expectedError);
        EXPECT_GT(std::strlen(systemError.what()), 1);
    } catch (...) {
        FAIL() << "Unexpected or unknown error.";
    }
}

TEST(Win32_ThrowCurrentSystemErrorIf, it_does_not_throw_if_false) {
    const auto expectedError = doSomethingThatSetsLastError();

    EXPECT_NO_THROW(Win32::throwCurrentSystemErrorIf(false));
}

TEST(Win32_GetCurrentErrorCode, it_returns_the_current_error_code) {
    const auto expectedError = doSomethingThatSetsLastError();

    const auto currentError = Win32::getCurrentErrorCode();
    EXPECT_EQ(currentError, expectedError);
}

TEST(Win32_GetCurrentErrorCode, it_returns_no_error) {
    constexpr int myValue = 4242;
    Win32::setCurrentErrorCode(myValue);
    doSomethingThatDoesNotSetsLastError();

    const auto currentError = Win32::getCurrentErrorCode();
    EXPECT_EQ(currentError, myValue);
}
#endif
