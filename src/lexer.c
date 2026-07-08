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
 * along with this program.  If not, see <https: //www.gnu.org/licenses/>.
 */

#include "lexer.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

M2H_Result M2H_token_ctor(M2H_OUT M2H_Token *self, M2H_IN M2H_TokenType type,
                          M2H_IN const char literal, M2H_IN const char *text) {

    self->type = type;

    switch (type) {
    case M2H_TOKENTYPE_TEXT: {
        self->text = (char *)malloc(strlen(text));
        if (self->text == NULL) {
            return M2H_RESULT_NOMEM;
        }
        break;
    }
    case M2H_TOKENTYPE_LITERAL: {
        self->literal = literal;
        break;
    }
    case M2H_TOKENTYPE_EOF: {
        break;
    }
    default: {
        return M2H_RESULT_UNKNOWN_TOKENTYPE;
    }
    }
    return M2H_RESULT_OK;
}

M2H_Result M2H_token_dtor(M2H_OUT M2H_Token *self) {
    if (self->type == M2H_TOKENTYPE_TEXT) {
        if (self->text == NULL) {
            return M2H_RESULT_NULL_DESTROY;
        }
        free(self->text);
        self->text = NULL;
    }

    return M2H_RESULT_OK;
}

M2H_Result M2H_lexer_ctor(M2H_OUT M2H_Lexer *self, M2H_IN const char *path) {
    self->input_file_path = (char *)malloc(strlen(path));
    if (self->input_file_path == NULL) {
        return M2H_RESULT_NOMEM;
    }

    self->fp = fopen(self->input_file_path, "r");
    if (self->fp == NULL) {
        return M2H_RESULT_ERRNO;
    }

    return M2H_RESULT_OK;
}

M2H_Result M2H_lexer_dtor(M2H_OUT M2H_Lexer *self) {
    if (self->input_file_path == NULL) {
        return M2H_RESULT_NULL_DESTROY;
    }
    free(self->input_file_path);
    self->input_file_path = NULL;

    if (self->fp == NULL || fclose(self->fp) == EOF) {
        return M2H_RESULT_CANNOT_CLOSE_FILE;
    }
    self->fp = NULL;
    return M2H_RESULT_OK;
}

M2H_Result M2H_next_token(M2H_OUT M2H_Token *token, M2H_IN M2H_Lexer *lexer) {
    int cur = fgetc(lexer->fp);
    
    if (cur == EOF) {
        if (feof(lexer->fp)) {
            token->type = M2H_TOKENTYPE_EOF;
        } else {
            return M2H_RESULT_ERRNO;
        }
    }

    

    return M2H_RESULT_OK;
}

#ifdef DEBUG

void M2H_token_tostr(M2H_IN M2H_Token *token) {
    switch (token->type) {
    case M2H_TOKENTYPE_TEXT: {
        printf("(TEXT, %s)\n", token->text);
        break;
    }
    case M2H_TOKENTYPE_LITERAL: {
        printf("(LITERAL, %c)", token->literal);
        break;
    }
    case M2H_TOKENTYPE_EOF: {
        printf("(EOF)");
        break;
    }
    default: {
        printf("(UNKNOWN TOKEN)");
        break;
    }
    }
}

#endif