#include "Common/Error.h"
#include <cerrno>
#include <gtest/gtest.h>

using namespace common;

static auto produce_int_or_fail(bool fail) -> ErrorOr<int> {
    if (fail) {
        return {Error::from_errno(EINVAL, "foo()")};
    }
    return 10;
}

static auto succeed_without_return_value_or_fail(bool fail) -> ErrorOr<void> {
    if (fail) {
        return {Error::from_errno(ENOMEM, "bar()")};
    }
    return {};
}

static auto call_produce_int_or_fail(bool fail) -> ErrorOr<int> {
    auto result = TRY(produce_int_or_fail(fail));
    return result + 2;
}

TEST(Error, ErrorBasics) {
    {
        auto should_be_value = produce_int_or_fail(false);
        EXPECT_EQ(should_be_value.is_value(), true);
        EXPECT_EQ(should_be_value.is_error(), false);
        EXPECT_EQ(should_be_value.value(), 10);
    }

    {
        auto should_be_error = produce_int_or_fail(true);
        EXPECT_EQ(should_be_error.is_value(), false);
        EXPECT_EQ(should_be_error.is_error(), true);
        EXPECT_EQ(should_be_error.error().error_domain(), ErrorDomain::CORE);
        EXPECT_EQ(should_be_error.error().error_message(), "foo() failed with EINVAL(22) Invalid argument");
    }

    {
        auto should_be_void = succeed_without_return_value_or_fail(false);
        EXPECT_EQ(should_be_void.is_value(), true);
        EXPECT_EQ(should_be_void.is_error(), false);
        EXPECT_EQ(should_be_void.value(), std::monostate{});
    }

    {
        auto should_be_error = succeed_without_return_value_or_fail(true);
        EXPECT_EQ(should_be_error.is_value(), false);
        EXPECT_EQ(should_be_error.is_error(), true);
        EXPECT_EQ(should_be_error.error().error_domain(), ErrorDomain::CORE);
        EXPECT_EQ(should_be_error.error().error_message(), "bar() failed with ENOMEM(12) Cannot allocate memory");
    }
}

TEST(Error, ErrorWithTry) {
    {
        auto should_be_value = call_produce_int_or_fail(false);
        EXPECT_EQ(should_be_value.is_value(), true);
        EXPECT_EQ(should_be_value.value(), 12);
    }

    {
        auto should_be_error = call_produce_int_or_fail(true);
        EXPECT_EQ(should_be_error.is_value(), false);
        EXPECT_EQ(should_be_error.is_error(), true);
        EXPECT_EQ(should_be_error.error().error_message(), "foo() failed with EINVAL(22) Invalid argument");
    }
}

TEST(Error, ErrorToException) {
    {
        EXPECT_THROW(
            {
                try {
                    [[maybe_unused]] auto not_reached = TRY_OR_THROW(produce_int_or_fail(true));
                } catch (const Exception& e) {
                    EXPECT_STREQ("foo() failed with EINVAL(22) Invalid argument [TestError.cpp:78]", e.what());
                    throw;
                }
            },
            Exception);
    }
}
