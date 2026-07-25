/**
 * @file lexer.c
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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "md2html/core/lexer.h"
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define M2H_VEC_T char
#define M2H_VEC_DISPT Char
#include "md2html/base/vector.h"
#undef M2H_VEC_DISPT
#undef M2H_VEC_T

M2H_Result M2H_token_duplicate(M2H_OUT M2H_Token *dest, M2H_IN M2H_Token *src) {
    if (dest->type != M2H_TOKENTYPE_NONE) {
        return M2H_RESULT_ILLEGAL_ARGUMENT;
    }
    *dest = *src;
    if (src->type == M2H_TOKENTYPE_TEXT) {
        dest->text = (char *)malloc((strlen(src->text) + 1) * sizeof(char));
        if (dest->text == NULL) {
            return M2H_RESULT_MALLOC_FAIL;
        }
        strcpy(dest->text, src->text);
    }
    return M2H_RESULT_OK;
}

M2H_Result M2H_token_dtor(M2H_OUT M2H_Token *self) {
    if (self->type == M2H_TOKENTYPE_TEXT) {
        free(self->text);
        self->text = NULL;
    }
    self->type = M2H_TOKENTYPE_NONE;
    return M2H_RESULT_OK;
}

M2H_Result M2H_lexer_ctor(M2H_OUT M2H_Lexer *self,
                          M2H_INOUT M2H_Reader *reader) {
    self->reader = reader;
    M2H_RELAY(M2H_vector_long_ctor(&self->checkpoint, M2H_DEFAULT_VEC_SIZE));
    return M2H_RESULT_OK;
}

M2H_Result M2H_lexer_dtor(M2H_OUT M2H_Lexer *self) {
    M2H_vector_long_dtor(&self->checkpoint);
    return M2H_RESULT_OK;
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

M2H_Result M2H_next_token(M2H_OUT M2H_Token *token, M2H_IN M2H_Lexer *lexer) {
    if (token->type != M2H_TOKENTYPE_NONE) {
        return M2H_RESULT_ILLEGAL_ARGUMENT;
    }

    M2H_VectorChar buf;

    // Use the first char to determine token type
    int cur;
    M2H_RELAY(lexer->reader->get_char(lexer->reader, &cur));
    if (cur == EOF) {
        token->type = M2H_TOKENTYPE_EOF;
        return M2H_RESULT_OK;
    } else if (is_literal(cur)) {
        token->type = M2H_TOKENTYPE_LITERAL;
        token->literal = cur;
        return M2H_RESULT_OK;
    }

    // If the type is text or blank, needs to process
    if (cur == '\n') {
        token->type = M2H_TOKENTYPE_NEWLINE;
    } else {
        token->type = M2H_TOKENTYPE_TEXT;
        M2H_RELAY(M2H_vector_char_ctor(&buf, M2H_DEFAULT_VEC_SIZE));
    }

    switch (token->type) {
    case M2H_TOKENTYPE_NEWLINE: {
        long backward_pos;
        do {
            M2H_RELAY(lexer->reader->tell(lexer->reader, &backward_pos));
            M2H_RELAY(lexer->reader->get_char(lexer->reader, &cur));
        } while (isspace(cur) && cur != '\n');
        if (cur != EOF) {
            M2H_RELAY(lexer->reader->seek(lexer->reader, backward_pos));
        }
        break;
    }
    case M2H_TOKENTYPE_TEXT: {
        long backward_pos;
        do {
            M2H_RELAY(lexer->reader->tell(lexer->reader, &backward_pos));
            M2H_RELAY(M2H_vector_char_pushback(&buf, (char)cur));
            M2H_RELAY(lexer->reader->get_char(lexer->reader, &cur));
        } while (cur != '\n' && cur != EOF && !is_literal(cur));
        if (cur != EOF) {
            M2H_RELAY(lexer->reader->seek(lexer->reader, backward_pos));
        }
        M2H_RELAY(M2H_vector_char_pushback(&buf, '\0'));
        token->text = buf.ptr;
        buf.ptr = NULL;
    }
    default:
        break;
    }

    return M2H_RESULT_OK;
}

M2H_Result M2H_lexer_checkpoint(M2H_OUT M2H_Lexer *self) {
    long current;
    M2H_RELAY(self->reader->tell(self->reader, &current));
    if (current == -1L) {
        return M2H_RESULT_ERRNO;
    }
    M2H_RELAY(M2H_vector_long_pushback(&self->checkpoint, current));
    return M2H_RESULT_OK;
}

M2H_Result M2H_lexer_restore(M2H_INOUT M2H_Lexer *self) {
    long top;
    M2H_RELAY(M2H_vector_long_top(&self->checkpoint, &top));
    M2H_RELAY(self->reader->seek(self->reader, top));
    return M2H_RESULT_OK;
}

M2H_Result M2H_lexer_drop_checkpoint(M2H_OUT M2H_Lexer *self) {
    M2H_RELAY(M2H_vector_long_popback(&self->checkpoint));
    return M2H_RESULT_OK;
}

#ifdef DEBUG

void M2H_print_token(M2H_IN M2H_Token *token) {
    switch (token->type) {
    case M2H_TOKENTYPE_TEXT: {
        printf("(TEXT, %s)\n", token->text);
        break;
    }
    case M2H_TOKENTYPE_LITERAL: {
        printf("(LITERAL, %c)\n", token->literal);
        break;
    }
    case M2H_TOKENTYPE_EOF: {
        printf("(EOF)\n");
        break;
    }
    case M2H_TOKENTYPE_NEWLINE: {
        printf("(NEWLINE)\n");
        break;
    }
    case M2H_TOKENTYPE_NONE: {
        printf("(NONE)\n");
        break;
    }
    default: {
        printf("(UNKNOWN TOKEN)\n");
        break;
    }
    }
}

#endif // DEBUG