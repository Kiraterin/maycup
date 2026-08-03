/**
 * @file objprint.h
 * @brief Print human-readable structures
 * @date 2026-08-04
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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef OBJPRINT_H
#define OBJPRINT_H
#ifdef DEBUG

#include "md2html/base/common.h"
#include "md2html/core/lexer.h"
#include "md2html/core/ast.h"

/**
 * @brief Print a token
 * @param token In, the dest token
 */
void M2H_print_token(M2H_IN M2H_Token *token);

/**
 * @brief Print an AST
 * @param ast In, the ast to print
 * @param root In, the entry point
 */
void M2H_print_ast(M2H_IN M2H_AST *ast, M2H_IN ssize_t root);

#endif // DEBUG
#endif // OBJPRINT_H