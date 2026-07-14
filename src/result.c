/**
 * @file result.c
 * @brief Result type definition
 * @date 2026-07-09
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

#include "md2html/result.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

void M2H_error_printmsg(M2H_IN M2H_Result res) {
    switch (res) {
    case M2H_RESULT_OK:
        return;
    case M2H_RESULT_ERRNO:
        fprintf(stderr, "Error: %s (%d)\n", strerror(errno), errno);
        break;
    default: {
        fprintf(stderr, "Error: ");
        switch (res) {
        case M2H_RESULT_ILLEGAL_ARGUMENT:
            puts("illegal argument");
            break;
        case M2H_RESULT_MALLOC_FAIL:
            puts("failed when allocating memory");
            break;
        case M2H_RESULT_EMPTY_VECTOR:
            puts("vector operated is empty");
            break;
        case M2H_RESULT_UNKNOWN_TOKENTYPE:
            puts("unknown token type");
            break;
        case M2H_RESULT_ARENA_ERROR:
            puts("arena error");
            break;
        case M2H_RESULT_PARSE_MISMATCH:
            puts("parsing mismatched");
        default:
            break;
        }
        putchar('\n');
    }
    }
}