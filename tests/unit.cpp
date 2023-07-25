#include <sigfn.hpp>

#include <csignal>
#include <iostream>
#include <sstream>
#include <unordered_map>

#define PASS 0
#define FAIL 1

#define NEGATIVE_SIGNUM -1
#define NULL_HANDLER NULL

#ifndef __FUNCTION_NAME__
#ifdef WIN32 // WINDOWS
#define __FUNCTION_NAME__ __FUNCTION__
#else //*NIX
#define __FUNCTION_NAME__ __func__
#endif
#endif

#define TEST_ASSERT(COND) test_assert((COND), __FILE__, __FUNCTION_NAME__, __LINE__, #COND)

static void test_assert(
    bool condition,
    const std::string &file,
    const std::string &function,
    int line,
    const std::string &description);

static void test_c_handle();
static void test_c_ignore();
static void test_c_reset();
static void test_cpp_handle();
static void test_cpp_ignore();
static void test_cpp_reset();
static void test_cpp_exception();

static void echo_signum(int signum, void *userdata);

int main(int argc, const char **argv)
{
    const std::unordered_map<std::string, std::function<void()>> unit_tests = {
        {"HANDLE_C", test_c_handle},
        {"IGNORE_C", test_c_ignore},
        {"RESET_C", test_c_reset},
        {"HANDLE_CPP", test_cpp_handle},
        {"IGNORE_CPP", test_cpp_ignore},
        {"RESET_CPP", test_cpp_reset},
        {"EXCEPTION_CPP", test_cpp_exception},
    };
    int result;
    result = PASS;
    if (argc > 1)
    {
        try
        {
            const std::function<void()> &unit_test = unit_tests.at(argv[1]);
            if (unit_test)
            {
                unit_test();
            }
            else
            {
                throw std::runtime_error(
                    "invalid test name \"" + std::string(argv[1]) + "\"");
            }
        }
        catch (const std::exception &exception)
        {
            std::cerr << exception.what() << std::endl;
            result = FAIL;
        }
    }
    return result;
}

void test_assert(
    bool condition,
    const std::string &file,
    const std::string &function,
    int line,
    const std::string &description)
{
    std::stringstream error_stream;
    if (!condition)
    {
        error_stream << file << ":" << function << ":" << line << ":"
                     << " failed to assert \"" << description << "\"";
        throw std::runtime_error(error_stream.str());
    }
}

void test_c_handle()
{
    int expected;
    int actual;
    int raised;

    expected = SIGFN_INVALID_SIGNUM;
    actual = sigfn_handle(-1, echo_signum, NULL);
    TEST_ASSERT(expected == actual);

    expected = SIGFN_INVALID_HANDLER;
    actual = sigfn_handle(SIGINT, NULL, NULL);
    TEST_ASSERT(expected == actual);

    expected = SIGFN_SUCCESS;
    actual = sigfn_handle(SIGINT, echo_signum, &raised);
    TEST_ASSERT(expected == actual);

    raised = -SIGINT;
    raise(SIGINT);
    TEST_ASSERT(raised == SIGINT);
}

void test_c_ignore()
{
    int expected;
    int actual;

    expected = SIGFN_INVALID_SIGNUM;
    actual = sigfn_ignore(-1);
    TEST_ASSERT(expected == actual);

    expected = SIGFN_SUCCESS;
    actual = sigfn_ignore(SIGINT);
    TEST_ASSERT(expected == actual);
}

void test_c_reset()
{
    int expected;
    int actual;

    expected = SIGFN_INVALID_SIGNUM;
    actual = sigfn_reset(-1);
    TEST_ASSERT(expected == actual);

    expected = SIGFN_SUCCESS;
    actual = sigfn_reset(SIGINT);
    TEST_ASSERT(expected == actual);
}

void test_cpp_handle()
{
    int flag;
    sigfn::handler_function copied_handler;
    sigfn::handler_function moved_handler;

    flag = 0;
    copied_handler = [&](int signum)
    {
        flag = 1;
    };

    moved_handler = [&](int signum)
    {
        flag = 2;
    };

    try
    {
        sigfn::handle(-1, copied_handler);
    }
    catch (const std::exception &e)
    {
        if (e.what() != nullptr)
        {
            flag = -1;
        }
    }
    TEST_ASSERT(flag == -1);

    sigfn::handle(SIGINT, copied_handler);
    raise(SIGINT);
    TEST_ASSERT(flag == 1);

    sigfn::handle(SIGINT, std::move(moved_handler));
    raise(SIGINT);
    TEST_ASSERT(flag == 2);
}

void test_cpp_ignore()
{
    const std::function<void(int, bool)> try_catch_assert(
        [](
            int signum,
            bool expect_error)
        {
            bool has_error;

            has_error = false;
            try
            {
                sigfn::ignore(signum);
            }
            catch (const std::exception &exception)
            {
                if (exception.what() != nullptr)
                {
                    has_error = true;
                }
            }
            TEST_ASSERT(expect_error == has_error);
        });
    try_catch_assert(-1, true);
    try_catch_assert(SIGINT, false);
}

void test_cpp_reset()
{
    const std::function<void(int, bool)> try_catch_assert(
        [](
            int signum,
            bool expect_error)
        {
            bool has_error;

            has_error = false;
            try
            {
                sigfn::reset(signum);
            }
            catch (const std::exception &exception)
            {
                if (exception.what() != nullptr)
                {
                    has_error = true;
                }
            }
            TEST_ASSERT(expect_error == has_error);
        });
    try_catch_assert(-1, true);
    try_catch_assert(SIGINT, false);
}

void test_cpp_exception()
{
    const std::function<void(int, const std::string &)> try_catch_assert(
        [](
            int status,
            const std::string &expected_message)
        {
            std::string message("replaced by error messgae");
            try
            {
                throw sigfn::exception(status);
            }
            catch (const std::exception &exception)
            {
                message = exception.what();
            }
            TEST_ASSERT(expected_message == message);
        });
    try_catch_assert(SIGFN_SUCCESS, "");
    try_catch_assert(SIGFN_INVALID_SIGNUM, sigfn::INVALID_SIGNUM);
    try_catch_assert(SIGFN_INVALID_HANDLER, sigfn::INVALID_HANDLER);
}

void echo_signum(int signum, void *userdata)
{
    *(int *)userdata = signum;
}