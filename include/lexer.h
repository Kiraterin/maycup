/**
 * @file lexer.h
 * @brief Lexer in md2html
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

#ifndef LEXER_H
#define LEXER_H

#include "common.h"
#include "result.h"
#include <stdio.h>

typedef enum {
    M2H_TOKENTYPE_EOF = 0,
    M2H_TOKENTYPE_LITERAL,
    M2H_TOKENTYPE_TEXT,
} M2H_TokenType;

typedef struct {
    M2H_TokenType type;
    union {
        char *text;
        char literal;
    };
} M2H_Token;

/**
 * @brief Construct a token
 * @param self Out, the token to construct
 * @param type In, the type of token
 * @param literal In, necessary only when the type is @c M2H_TOKENTYPE_LITERAL
 * @param text In, necessary only when the type is @c M2H_TOKENTYPE_TEXT
 * @return M2H_Result
 */
M2H_Result M2H_token_ctor(M2H_OUT M2H_Token *self, M2H_IN M2H_TokenType type,
                          M2H_IN const char literal, M2H_IN const char *text);
M2H_Result M2H_token_dtor(M2H_OUT M2H_Token *self);

typedef struct {
    char *input_file_path;
    FILE *fp;
} M2H_Lexer;

/**
 * @brief Construct a lexer
 * @param self Out, the lexer to construct
 * @param path In, the path to the source Markdown file
 * @return M2H_Result
 */
M2H_Result M2H_lexer_ctor(M2H_OUT M2H_Lexer *self, M2H_IN const char *path);

/**
 * @brief Destroy a lexer
 * @param self Out, the lexer to destroy
 * @return M2H_Result
 */
M2H_Result M2H_lexer_dtor(M2H_OUT M2H_Lexer *self);

/**
 * @brief Return the next token of the given lexer context
 * @param token Out, mustn't be already constructed
 * @param lexer In, the context
 * @return M2H_Result
 */
M2H_Result M2H_next_token(M2H_OUT M2H_Token *token, M2H_IN M2H_Lexer *lexer);

#ifdef DEBUG

/**
 * @brief Print a token
 * @param token In, the dest token
 */
void M2H_print_token(M2H_IN M2H_Token *token);

#endif

#endif // LEXER_H