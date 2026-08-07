/**
 * @file lexer.h
 * @brief Lexer in maycup
 * @date 2026-07-08
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

#ifndef LEXER_H
#define LEXER_H

#include "maycup/base/common.h"
#include "maycup/base/result.h"
#include "maycup/io/reader.h"
#include <stdio.h>

typedef enum {
    MAYCUP_TOKENTYPE_NONE = 0,
    MAYCUP_TOKENTYPE_EOF,
    MAYCUP_TOKENTYPE_LITERAL,
    MAYCUP_TOKENTYPE_TEXT,
    MAYCUP_TOKENTYPE_NEWLINE
} MAYCUP_TokenType;

typedef struct {
    MAYCUP_TokenType type;
    union {
        char *text;
        int literal;
    };
} MAYCUP_Token;

/**
 * @brief Duplicate a token
 * @param dest Out, the destination
 * @param src In, the token to duplicate
 * @return MAYCUP_Result
 */
MAYCUP_Result maycup_token_duplicate(MAYCUP_OUT MAYCUP_Token *dest,
                                     MAYCUP_IN MAYCUP_Token *src);

/**
 * @brief Destruct a token
 * @param self Out, the token to destruct
 * @return MAYCUP_Result
 */
MAYCUP_Result maycup_token_dtor(MAYCUP_OUT MAYCUP_Token *self);

#define MAYCUP_VEC_T long
#define MAYCUP_VEC_DISPT Long
#include "maycup/base/vector.h"
#undef MAYCUP_VEC_DISPT
#undef MAYCUP_VEC_T

typedef struct {
    MAYCUP_Reader *reader;
    MAYCUP_VectorLong checkpoint;
} MAYCUP_Lexer;

/**
 * @brief Construct a lexer
 * @param self Out, the lexer to construct
 * @param reader In & out, the reader which lexer will use
 * @return MAYCUP_Result
 */
MAYCUP_Result maycup_lexer_ctor(MAYCUP_OUT MAYCUP_Lexer *self,
                                MAYCUP_INOUT MAYCUP_Reader *reader);

/**
 * @brief Destruct a lexer
 * @param self Out, the lexer to destruct
 * @return MAYCUP_Result
 */
MAYCUP_Result maycup_lexer_dtor(MAYCUP_OUT MAYCUP_Lexer *self);

/**
 * @brief Construct and return the next token of the given lexer context
 * @param token Out, dest token
 * @param lexer In, the context
 * @return MAYCUP_Result
 */
MAYCUP_Result maycup_next_token(MAYCUP_OUT MAYCUP_Token *token,
                                MAYCUP_IN MAYCUP_Lexer *lexer);

/**
 * @brief Set a checkpoint in the file
 * @param self Out, the lexer
 * @return MAYCUP_Result
 */
MAYCUP_Result maycup_lexer_checkpoint(MAYCUP_OUT MAYCUP_Lexer *self);

/**
 * @brief Return to the checkpoint and won't pop out it
 * @param self In & out, the lexer
 * @return MAYCUP_Result
 */
MAYCUP_Result maycup_lexer_restore(MAYCUP_INOUT MAYCUP_Lexer *self);

/**
 * @brief Drop the top checkpoint
 * @param self Out, the lexer
 * @return MAYCUP_Result
 */
MAYCUP_Result maycup_lexer_drop_checkpoint(MAYCUP_OUT MAYCUP_Lexer *self);

#endif // LEXER_H