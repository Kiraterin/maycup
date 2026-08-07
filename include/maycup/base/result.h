/**
 * @file result.h
 * @brief Result type declaration
 * @date 2026-07-08
 * @copyright GPLv3 License
 * @section LICENSE
 * maycup
 * Copyright (C) 2026 Kiraterin
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef RESULT_H
#define RESULT_H

#include "maycup/base/common.h"

typedef enum {
    MAYCUP_RESULT_OK = 0,
    MAYCUP_RESULT_ERRNO = 1,
    MAYCUP_RESULT_MALLOC_FAIL,
    MAYCUP_RESULT_UNKNOWN_TOKENTYPE,
    MAYCUP_RESULT_ILLEGAL_ARGUMENT,
    MAYCUP_RESULT_EMPTY_VECTOR,
    MAYCUP_RESULT_ARENA_ERROR,
    MAYCUP_RESULT_MAX_CAP_EXCEEDED,

    MAYCUP_RESULT_PARSE_MISMATCH
} MAYCUP_Result;

/**
 * @brief Print error message
 * @param _res_ In, the type of result
 */
void MAYCUP_error_printmsg(MAYCUP_IN MAYCUP_Result _res_);

/**
 * @brief Do something when @c expr is not @c MAYCUP_RESULT_OK ; The error type is
 *        @c _res_
 */
#define MAYCUP_HANDLE(expr, handler)                                              \
    do {                                                                       \
        MAYCUP_Result _res_ = (expr);                                               \
        if (_res_ != MAYCUP_RESULT_OK) {                                            \
            handler;                                                           \
        }                                                                      \
    } while (false)

/**
 * @brief Relay error to caller
 */
#define MAYCUP_RELAY(expr) MAYCUP_HANDLE(expr, return _res_)

/**
 * @brief Relay error or ok to caller, but if the _res_ is @c when , continue; if
 *        the _res_ is @c act_res , do action
 */
#define MAYCUP_RELAY_UNLESS_DO(expr, when, act_res, action)                       \
    if (true) {                                                                \
        MAYCUP_Result _res_ = (expr);                                               \
        if (_res_ == act_res) {                                                  \
            action;                                                            \
        }                                                                      \
        if (_res_ != (when)) {                                                   \
            return _res_;                                                        \
        }                                                                      \
    } else                                                                     \
        (void)0

/**
 * @brief Relay error to caller or do action if it's ok, but if the _res_ is
 *        @c err , continue
 */
#define MAYCUP_RELAY_UNLESSOK_DO(expr, err, ok)                                   \
    MAYCUP_RELAY_UNLESS_DO(expr, err, MAYCUP_RESULT_OK, ok)

/**
 * @brief Abort and print error message if the result of expr is not @c
 *        MAYCUP_RESULT_OK
 */
#define MAYCUP_UNWRAP(expr)                                                       \
    do {                                                                       \
        MAYCUP_Result _res_ = (expr);                                               \
        if (_res_ != MAYCUP_RESULT_OK) {                                            \
            MAYCUP_error_printmsg(_res_);                                           \
            abort();                                                           \
        }                                                                      \
    } while (false)

#endif // RESULT_H