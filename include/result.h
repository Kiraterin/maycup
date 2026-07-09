/**
 * @file result.h
 * @brief Result type definition
 * @date 2026-07-08
 * @copyright GPLv3 License
 * @section LICENSE
 * md2html
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
 * along with this program.  If not, see <https: //www.gnu.org/licenses/>.
 */

#ifndef RESULT_H
#define RESULT_H

#include "common.h"

typedef enum {
    M2H_RESULT_OK = 0,
    M2H_RESULT_ERRNO = 1,
    M2H_RESULT_NOMEM,
    M2H_RESULT_NULL_DESTROY,
    M2H_RESULT_CANNOT_CLOSE_FILE,
    M2H_RESULT_UNKNOWN_TOKENTYPE,
    M2H_RESULT_ILLEGAL_ARGUMENT
} M2H_Result;

#define M2H_RELAY(expr)                                                        \
    do {                                                                       \
        M2H_Result res = expr;                                                 \
        if (res != M2H_RESULT_OK) {                                            \
            return res;                                                        \
        }                                                                      \
    } while (false)

#endif // RESULT_H