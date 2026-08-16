/**
 * @file objprint.h
 * @brief Print human-readable structures
 * @date 2026-08-04
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

#ifndef OBJPRINT_H
#define OBJPRINT_H
#ifdef DEBUG

#include "maycup/base/common.h"
#include "maycup/core/ast.h"
#include "maycup/core/lexer.h"

/**
 * @brief Convert a token to string
 * @param token In, the dest token
 * @return char* remember to free it 
 */
char *maycup_token_tostr(MAYCUP_IN MAYCUP_Token *token);

/**
 * @brief Print a token
 * @param token In, the dest token
 */
void maycup_print_token(MAYCUP_IN MAYCUP_Token *token);

/**
 * @brief Print an AST
 * @param ast In, the ast to print
 * @param root In, the entry point
 */
void maycup_print_ast(MAYCUP_IN MAYCUP_AST *ast, MAYCUP_IN ssize_t root);

#endif // DEBUG
#endif // OBJPRINT_H