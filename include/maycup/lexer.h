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

#include "maycup/reader.h"
#include "maycup/result.h"

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
 * @note Function fails when the token type of @p dest is not
 *       @c MAYCUP_TOKENTYPE_NONE
 * @param dest Out, the destination. Cannot be @c NULL
 * @param src In, the token to duplicate. Cannot be @c NULL
 * @return MAYCUP_Result
 */
MAYCUP_Result maycup_token_duplicate(MAYCUP_Token *dest, MAYCUP_Token *src);

/**
 * @brief Destruct a token
 * @note 1. @c self->type will be changed to @c MAYCUP_TOKENTYPE_NONE
         2. @c self->text will be freed and changed to @c NULL if @c self->type
            is @c MAYCUP_TOKENTYPE_TEXT
         3. Double free a token is allowed
 * @param self Out, the token to destruct
 * @return MAYCUP_Result
 */
MAYCUP_Result maycup_token_dtor(MAYCUP_Token *self);

#define MAYCUP_VEC_T long
#define MAYCUP_VEC_DISPT Long
#include "maycup/vector.h"
#undef MAYCUP_VEC_DISPT
#undef MAYCUP_VEC_T

typedef struct {
    MAYCUP_Reader *reader;
    MAYCUP_VectorLong checkpoint;
} MAYCUP_Lexer;

/**
 * @brief Construct a lexer
 * @param self Out, the lexer to construct
 * @param reader In & out, the reader which lexer will use. Cannot be @c NULL
 * @return MAYCUP_Result
 */
MAYCUP_Result maycup_lexer_ctor(MAYCUP_Lexer *self, MAYCUP_Reader *reader);

/**
 * @brief Destruct a lexer
 * @note The reader won't be destructed, but the pointer value in the object
 *       will be changed to @c NULL
 * @param self Out, the lexer to destruct
 * @return MAYCUP_Result
 */
MAYCUP_Result maycup_lexer_dtor(MAYCUP_Lexer *self);

/**
 * @brief Construct and return the next token of the given lexer context
 * @note 1. This function should be the only place where tokens are constructed
 *          and it must return a constructed token with some content
 *       2. Argument @c token->type can only be @c MAYCUP_TOKENTYPE_NONE to
 *          ensure the token has been correctly destructed before construction
 * @param token Out, dest token. Cannot be @c NULL
 * @param lexer In, the context. Should be constructed by function
 *              @c maycup_lexer_ctor
 * @return MAYCUP_Result
 */
MAYCUP_Result maycup_next_token(MAYCUP_Token *token, MAYCUP_Lexer *lexer);

/**
 * @brief Set a checkpoint in the file
 * @param self Out, the lexer
 * @return MAYCUP_Result
 */
MAYCUP_Result maycup_lexer_checkpoint(MAYCUP_Lexer *self);

/**
 * @brief Return to the checkpoint but won't pop it out
 * @note The checkpoint stack cannot be empty
 * @param self In & out, the lexer
 * @return MAYCUP_Result
 */
MAYCUP_Result maycup_lexer_restore(MAYCUP_Lexer *self);

/**
 * @brief Drop the top checkpoint
 * @note The checkpoint stack cannot be empty
 * @param self Out, the lexer
 * @return MAYCUP_Result
 */
MAYCUP_Result maycup_lexer_drop_checkpoint(MAYCUP_Lexer *self);

#endif // LEXER_H