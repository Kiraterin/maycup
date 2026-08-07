/**
 * @file lexer.c
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

#include "maycup/core/lexer.h"
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>


#define MAYCUP_VEC_T char
#define MAYCUP_VEC_DISPT Char
#include "maycup/base/vector.h"
#undef MAYCUP_VEC_DISPT
#undef MAYCUP_VEC_T

// mock def
#include "mock_funcs.h"

MAYCUP_Result MAYCUP_token_duplicate(MAYCUP_OUT MAYCUP_Token *dest, MAYCUP_IN MAYCUP_Token *src) {
    if (dest->type != MAYCUP_TOKENTYPE_NONE) {
        return MAYCUP_RESULT_ILLEGAL_ARGUMENT;
    }
    *dest = *src;
    if (src->type == MAYCUP_TOKENTYPE_TEXT) {
        dest->text = (char *)malloc((strlen(src->text) + 1) * sizeof(char));
        if (dest->text == NULL) {
            return MAYCUP_RESULT_MALLOC_FAIL;
        }
        strcpy(dest->text, src->text);
    }
    return MAYCUP_RESULT_OK;
}

MAYCUP_Result MAYCUP_token_dtor(MAYCUP_OUT MAYCUP_Token *self) {
    if (self->type == MAYCUP_TOKENTYPE_TEXT) {
        free(self->text);
        self->text = NULL;
    }
    self->type = MAYCUP_TOKENTYPE_NONE;
    return MAYCUP_RESULT_OK;
}

MAYCUP_Result MAYCUP_lexer_ctor(MAYCUP_OUT MAYCUP_Lexer *self,
                          MAYCUP_INOUT MAYCUP_Reader *reader) {
    self->reader = reader;
    MAYCUP_RELAY(MAYCUP_vector_long_ctor(&self->checkpoint, MAYCUP_DEFAULT_VEC_SIZE));
    return MAYCUP_RESULT_OK;
}

MAYCUP_Result MAYCUP_lexer_dtor(MAYCUP_OUT MAYCUP_Lexer *self) {
    MAYCUP_RELAY(MAYCUP_vector_long_dtor(&self->checkpoint));
    return MAYCUP_RESULT_OK;
}

static bool is_literal(int ch) {
    switch (ch) {
    case '*':
    case '\\':
    case '#':
    case '>':
    case '_':
    case '`':
    case '[':
    case ']':
    case '(':
    case ')':
    case '{':
    case '}':
    case '.':
    case '!':
    case '~':
    case '-':
    case '+':
    case '$':
    case ':':
    case '/':
        return true;
    default:
        return false;
    }
}

MAYCUP_Result MAYCUP_next_token(MAYCUP_OUT MAYCUP_Token *token, MAYCUP_IN MAYCUP_Lexer *lexer) {
    if (token->type != MAYCUP_TOKENTYPE_NONE) {
        return MAYCUP_RESULT_ILLEGAL_ARGUMENT;
    }

    MAYCUP_VectorChar buf;

    // Use the first char to determine token type
    int cur;
    MAYCUP_RELAY(MAYCUP_reader_get_char(lexer->reader, &cur));
    if (cur == EOF) {
        token->type = MAYCUP_TOKENTYPE_EOF;
        return MAYCUP_RESULT_OK;
    } else if (is_literal(cur)) {
        token->type = MAYCUP_TOKENTYPE_LITERAL;
        token->literal = cur;
        return MAYCUP_RESULT_OK;
    }

    // If the type is text or blank, needs to process
    if (cur == '\n') {
        token->type = MAYCUP_TOKENTYPE_NEWLINE;
    } else {
        token->type = MAYCUP_TOKENTYPE_TEXT;
        MAYCUP_RELAY(MAYCUP_vector_char_ctor(&buf, MAYCUP_DEFAULT_VEC_SIZE));
    }

    switch (token->type) {
    case MAYCUP_TOKENTYPE_NEWLINE: {
        long backward_pos;
        do {
            MAYCUP_RELAY(MAYCUP_reader_tell(lexer->reader, &backward_pos));
            MAYCUP_RELAY(MAYCUP_reader_get_char(lexer->reader, &cur));
        } while (isspace(cur) && cur != '\n');
        if (cur != EOF) {
            MAYCUP_RELAY(MAYCUP_reader_seek(lexer->reader, backward_pos));
        }
        break;
    }
    case MAYCUP_TOKENTYPE_TEXT: {
        long backward_pos;
        do {
            MAYCUP_RELAY(MAYCUP_reader_tell(lexer->reader, &backward_pos));
            MAYCUP_RELAY(MAYCUP_vector_char_pushback(&buf, (char)cur));
            MAYCUP_RELAY(MAYCUP_reader_get_char(lexer->reader, &cur));
        } while (cur != '\n' && cur != EOF && !is_literal(cur));
        if (cur != EOF) {
            MAYCUP_RELAY(MAYCUP_reader_seek(lexer->reader, backward_pos));
        }
        MAYCUP_RELAY(MAYCUP_vector_char_pushback(&buf, '\0'));
        token->text = buf.ptr;
        buf.ptr = NULL;
    }
    default:
        break;
    }

    return MAYCUP_RESULT_OK;
}

MAYCUP_Result MAYCUP_lexer_checkpoint(MAYCUP_OUT MAYCUP_Lexer *self) {
    long current;
    MAYCUP_RELAY(MAYCUP_reader_tell(self->reader, &current));
    if (current == -1L) {
        return MAYCUP_RESULT_ERRNO;
    }
    MAYCUP_RELAY(MAYCUP_vector_long_pushback(&self->checkpoint, current));
    return MAYCUP_RESULT_OK;
}

MAYCUP_Result MAYCUP_lexer_restore(MAYCUP_INOUT MAYCUP_Lexer *self) {
    long top;
    MAYCUP_RELAY(MAYCUP_vector_long_top(&self->checkpoint, &top));
    MAYCUP_RELAY(MAYCUP_reader_seek(self->reader, top));
    return MAYCUP_RESULT_OK;
}

MAYCUP_Result MAYCUP_lexer_drop_checkpoint(MAYCUP_OUT MAYCUP_Lexer *self) {
    MAYCUP_RELAY(MAYCUP_vector_long_popback(&self->checkpoint));
    return MAYCUP_RESULT_OK;
}
