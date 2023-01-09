#pragma once

#include "Assertions.h"

// TRY macro is meant for streamlining code that deals with ErrorOr<> return
// types.
//
// Its usage takes the form
//
//   auto my_func() -> ErrorOr<> {
//     ...
//     auto result = TRY(call_returning_erroror("foo"));
//     ...
//   }
//
// When the expression passed to TRY-macro returns an ErrorOr<> type
// containing an error, the execution of the current method is interrupted
// by returning the error up the call chain.
//
// The macro relies on non-standard GNU C++ extension called
// 'Statement and Declaration Expressions' or just 'Statement Expression':
// https://gcc.gnu.org/onlinedocs/gcc/Statement-Exprs.html
//
#define TRY(expression)                                                                                                \
    ({                                                                                                                 \
        auto _temporary_result = (expression);                                                                         \
        if (_temporary_result.is_error()) {                                                                            \
            return _temporary_result.release_error();                                                                  \
        }                                                                                                              \
        _temporary_result.release_value();                                                                             \
    })

// TRY_OR_THROW macro is similar to TRY but instead of returning the error it
// will raise the error as an exception.
#define TRY_OR_THROW(expression)                                                                                       \
    ({                                                                                                                 \
        auto _temporary_result = (expression);                                                                         \
        if (_temporary_result.is_error()) {                                                                            \
            _temporary_result.error().raise(__FILE__, __LINE__, __PRETTY_FUNCTION__);                                  \
        }                                                                                                              \
        _temporary_result.release_value();                                                                             \
    })

// MUST macro is similar to TRY but instead of returning the error it will
// cause program panic
#define MUST(expression)                                                                                               \
    ({                                                                                                                 \
        auto _temporary_result = (expression);                                                                         \
        VERIFY(!_temporary_result.is_error());                                                                         \
        _temporary_result.release_value();                                                                             \
    })

#define RAISE(error) error.raise(__FILE__, __LINE__, __PRETTY_FUNCTION__)
