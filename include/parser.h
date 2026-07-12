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
 * along with this program.  If not, see <https: //www.gnu.org/licenses/>.
 */

#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"

typedef struct {
    M2H_Token cur;
    M2H_ASTNode ast;
} M2H_Parser;

M2H_Result M2H_parser_ctor(M2H_OUT M2H_Parser *self);
M2H_Result M2H_parser_dtor(M2H_OUT M2H_Parser *self);

M2H_Result M2H_parse(M2H_INOUT M2H_Parser *parser, M2H_INOUT M2H_Lexer *lexer);

#endif // PARSER_H