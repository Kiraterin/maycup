/**
 * @file parser.c
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

#include "parser.h"

static M2H_Result advance(M2H_Parser *self, M2H_Lexer *lexer) {
    M2H_RELAY(M2H_next_token(&self->cur, lexer));
    return M2H_RESULT_OK;
}

static M2H_Result parse_blank(M2H_Parser *self, M2H_Lexer *lexer) {
    return M2H_RESULT_OK;
}

static M2H_Result parse_heading(M2H_Parser *self, M2H_Lexer *lexer) {
    return M2H_RESULT_OK;
}

static M2H_Result parse_paragraph(M2H_Parser *self, M2H_Lexer *lexer) {
    return M2H_RESULT_OK;
}

static M2H_Result parse_inline_text(M2H_Parser *self, M2H_Lexer *lexer) {
    return M2H_RESULT_OK;
}

static M2H_Result parse_block(M2H_Parser *self, M2H_Lexer *lexer) {
    while (self->cur.type != M2H_TOKENTYPE_EOF) {
        M2H_RELAY(advance(self, lexer));
        M2H_RELAY_HANDLE(parse_blank(self, lexer), M2H_RESULT_PARSER_DISMATCH,
                         continue);
        M2H_RELAY_HANDLE(parse_heading(self, lexer), M2H_RESULT_PARSER_DISMATCH,
                         continue);
        M2H_RELAY_HANDLE(parse_paragraph(self, lexer),
                         M2H_RESULT_PARSER_DISMATCH, continue);
    }
    return M2H_RESULT_OK;
}

M2H_Result M2H_parser_ctor(M2H_OUT M2H_Parser *self) {
    M2H_RELAY(M2H_astnode_ctor(&self->ast, M2H_ASTNODE_TYPE_DOCUMENT));
    return M2H_RESULT_OK;
}

M2H_Result M2H_parser_dtor(M2H_OUT M2H_Parser *self) {
    M2H_RELAY(M2H_astnode_dtor(&self->ast));
    return M2H_RESULT_OK;
}