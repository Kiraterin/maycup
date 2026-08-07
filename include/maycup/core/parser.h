/**
 * @file parser.h
 * @brief Parser in maycup
 * @date 2026-07-12
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

#ifndef PARSER_H
#define PARSER_H

#include "maycup/core/ast.h"
#include "maycup/core/lexer.h"

typedef struct {
    MAYCUP_Token cur_token;
    MAYCUP_AST ast;
    ssize_t root_astnode;
} MAYCUP_Parser;

/**
 * @brief Construct a parser
 * @param self Out, the parser to construct
 * @return MAYCUP_Result
 */
MAYCUP_Result maycup_parser_ctor(MAYCUP_OUT MAYCUP_Parser *self);

/**
 * @brief Destruct a parser
 * @param self Out, the parser to destruct
 * @return MAYCUP_Result
 */
MAYCUP_Result maycup_parser_dtor(MAYCUP_OUT MAYCUP_Parser *self);

MAYCUP_Result maycup_parse(MAYCUP_INOUT MAYCUP_Parser *parser,
                           MAYCUP_INOUT MAYCUP_Lexer *lexer);

#endif // PARSER_H