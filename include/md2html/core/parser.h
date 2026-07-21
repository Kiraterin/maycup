/**
 * @file parser.h
 * @brief Parser in md2html
 * @date 2026-07-12
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

#ifndef PARSER_H
#define PARSER_H

#include "md2html/core/ast.h"
#include "md2html/core/lexer.h"

typedef struct {
    M2H_Token cur_token;
    M2H_AST ast;
    ssize_t root_astnode;
} M2H_Parser;

/**
 * @brief Construct a parser
 * @param self Out, the parser to construct
 * @return M2H_Result 
 */
M2H_Result M2H_parser_ctor(M2H_OUT M2H_Parser *self);

/**
 * @brief Destruct a parser
 * @param self Out, the parser to destruct
 * @return M2H_Result
 */
M2H_Result M2H_parser_dtor(M2H_OUT M2H_Parser *self);

M2H_Result M2H_parse(M2H_INOUT M2H_Parser *parser, M2H_INOUT M2H_Lexer *lexer);

#endif // PARSER_H