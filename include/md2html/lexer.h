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

#include "md2html/common.h"
#include "md2html/result.h"
#include <stdio.h>

typedef enum {
    M2H_TOKENTYPE_NONE = 0,
    M2H_TOKENTYPE_EOF,
    M2H_TOKENTYPE_LITERAL,
    M2H_TOKENTYPE_TEXT,
    M2H_TOKENTYPE_NEWLINE
} M2H_TokenType;

typedef struct {
    M2H_TokenType type;
    union {
        char *text;
        int literal;
    };
} M2H_Token;

/**
 * @brief Duplicate a token
 * @param dest Out, the destination
 * @param src In, the token to duplicate
 * @return M2H_Result
 */
M2H_Result M2H_token_duplicate(M2H_OUT M2H_Token *dest, M2H_IN M2H_Token *src);

/**
 * @brief Destruct a token
 * @param self Out, the token to destruct
 * @return M2H_Result
 */
M2H_Result M2H_token_dtor(M2H_OUT M2H_Token *self);

#define M2H_VEC_T long
#define M2H_VEC_DISPT Long
#include "md2html/vector.h"
#undef M2H_VEC_DISPT
#undef M2H_VEC_T

typedef struct {
    char *input_file_path;
    FILE *fp;
    M2H_VectorLong checkpoint;
} M2H_Lexer;

/**
 * @brief Construct a lexer
 * @param self Out, the lexer to construct
 * @param path In, the path to the source Markdown file
 * @return M2H_Result
 */
M2H_Result M2H_lexer_ctor(M2H_OUT M2H_Lexer *self, M2H_IN const char *path);

/**
 * @brief Destruct a lexer
 * @param self Out, the lexer to destruct
 * @return M2H_Result
 */
M2H_Result M2H_lexer_dtor(M2H_OUT M2H_Lexer *self);

/**
 * @brief Construct and return the next token of the given lexer context
 * @param token Out, dest token
 * @param lexer In, the context
 * @return M2H_Result
 */
M2H_Result M2H_next_token(M2H_OUT M2H_Token *token, M2H_IN M2H_Lexer *lexer);

/**
 * @brief Set a checkpoint in the file
 * @param self Out, the lexer
 * @return M2H_Result
 */
M2H_Result M2H_lexer_checkpoint(M2H_OUT M2H_Lexer *self);

/**
 * @brief Return to the checkpoint and won't pop out it
 * @param self In & out, the lexer
 * @return M2H_Result
 */
M2H_Result M2H_lexer_restore(M2H_INOUT M2H_Lexer *self);

/**
 * @brief Drop the top checkpoint
 * @param self Out, the lexer
 * @return M2H_Result 
 */
M2H_Result M2H_lexer_drop_checkpoint(M2H_OUT M2H_Lexer *self);

#ifdef DEBUG

/**
 * @brief Print a token
 * @param token In, the dest token
 */
void M2H_print_token(M2H_IN M2H_Token *token);

#endif // DEBUG

#endif // LEXER_H