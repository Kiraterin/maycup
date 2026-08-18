/**
 * @file objprint.c
 * @brief Print human-readable structures
 * @date 2026-08-04
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

#include "maycup/reader.h"
#include "maycup/result.h"
#include "maycup/writer.h"
#include <stdlib.h>
#include <string.h>

#ifdef DEBUG

#include "maycup/ast.h"
#include "maycup/lexer.h"
#include <stdio.h>

typedef ssize_t idx;
#define MAYCUP_VEC_T idx
#define MAYCUP_VEC_DISPT Idx
#include "maycup/vector.h"
#undef MAYCUP_VEC_DISPT
#undef MAYCUP_VEC_T

char *maycup_token_tostr(MAYCUP_Token *token) {
    char *str;
    if (token->type == MAYCUP_TOKENTYPE_TEXT) {
        str = (char *)malloc((128 + strlen(token->text)) * sizeof(char));
    } else {
        str = (char *)malloc(128 * sizeof(char));
    }

    switch (token->type) {
    case MAYCUP_TOKENTYPE_TEXT: {
        sprintf(str, "(TEXT, %s)", token->text);
        break;
    }
    case MAYCUP_TOKENTYPE_LITERAL: {
        sprintf(str, "(LITERAL, %c)", token->literal);
        break;
    }
    case MAYCUP_TOKENTYPE_EOF: {
        sprintf(str, "(EOF)");
        break;
    }
    case MAYCUP_TOKENTYPE_NEWLINE: {
        sprintf(str, "(NEWLINE)");
        break;
    }
    case MAYCUP_TOKENTYPE_NONE: {
        sprintf(str, "(NONE)");
        break;
    }
    default: {
        sprintf(str, "(UNKNOWN TOKEN)");
        break;
    }
    }
    return str;
}

void maycup_print_token(MAYCUP_Token *token) {
    char *str = maycup_token_tostr(token);
    printf("%s\n", str);
    free(str);
}

char *maycup_ast_tostr(MAYCUP_AST *ast, ssize_t root) {
    MAYCUP_StringWriter sw;
    MAYCUP_UNWRAP(maycup_stringwriter_ctor_flexible(
        &sw, MAYCUP_DEFAULT_STRWRITER_FLEXBUF_SIZE));

    MAYCUP_VectorIdx stack;
    MAYCUP_VectorIdx level;
    MAYCUP_UNWRAP(maycup_vector_idx_ctor(&stack, MAYCUP_DEFAULT_VEC_SIZE));
    MAYCUP_UNWRAP(maycup_vector_idx_ctor(&level, MAYCUP_DEFAULT_VEC_SIZE));
    MAYCUP_UNWRAP(maycup_vector_idx_pushback(&stack, root));
    MAYCUP_UNWRAP(maycup_vector_idx_pushback(&level, 0));
    while (stack.len > 0) {
        ssize_t cur, lvl;
        MAYCUP_UNWRAP(maycup_vector_idx_top(&stack, &cur));
        MAYCUP_UNWRAP(maycup_vector_idx_popback(&stack));
        MAYCUP_UNWRAP(maycup_vector_idx_top(&level, &lvl));
        MAYCUP_UNWRAP(maycup_vector_idx_popback(&level));

        ssize_t getter = ast->data[cur].child;
        while (getter != -1) {
            MAYCUP_UNWRAP(maycup_vector_idx_pushback(&stack, getter));
            MAYCUP_UNWRAP(maycup_vector_idx_pushback(&level, lvl + 1));
            getter = ast->data[getter].prev_sibling;
        }

        for (int i = 1; i <= lvl; ++i) {
            MAYCUP_UNWRAP(maycup_writer_puts(&sw, "  "));
        }

        switch (ast->data[cur].type) {
        case MAYCUP_ASTNODE_TYPE_ROOT:
            MAYCUP_UNWRAP(maycup_writer_puts(&sw, "ROOT"));
            break;
        case MAYCUP_ASTNODE_TYPE_NONE:
            MAYCUP_UNWRAP(maycup_writer_puts(&sw, "NONE"));
            break;
        case MAYCUP_ASTNODE_TYPE_PARAGRAPH:
            MAYCUP_UNWRAP(maycup_writer_puts(&sw, "PARAGRAPH"));
            break;
        case MAYCUP_ASTNODE_TYPE_HEADING:
            MAYCUP_UNWRAP(maycup_writer_printf(&sw, "HEADING level=%d",
                                               ast->data[cur].heading.level));
            break;
        case MAYCUP_ASTNODE_TYPE_TEXT:
            MAYCUP_UNWRAP(maycup_writer_printf(
                &sw, "TEXT text=\"%s\", style=", ast->data[cur].text.content));
            switch (ast->data[cur].text.style) {
            case MAYCUP_TEXTSTYLE_PLAIN:
                MAYCUP_UNWRAP(maycup_writer_puts(&sw, "plain"));
                break;
            case MAYCUP_TEXTSTYLE_BOLD:
                MAYCUP_UNWRAP(maycup_writer_puts(&sw, "bold"));
                break;
            case MAYCUP_TEXTSTYLE_ITALIC:
                MAYCUP_UNWRAP(maycup_writer_puts(&sw, "italic"));
                break;
            case MAYCUP_TEXTSTYLE_BOLDITALIC:
                MAYCUP_UNWRAP(maycup_writer_puts(&sw, "bold & italic"));
                break;
            case MAYCUP_TEXTSTYLE_CODE:
                MAYCUP_UNWRAP(maycup_writer_puts(&sw, "code"));
                break;
            }
        }
        MAYCUP_UNWRAP(maycup_writer_puts(&sw, "\n"));
    }

    MAYCUP_UNWRAP(maycup_vector_idx_dtor(&stack));
    MAYCUP_UNWRAP(maycup_vector_idx_dtor(&level));

    char *res;
    MAYCUP_UNWRAP(maycup_stringwriter_dtor(&sw, &res));
    return res;
}

void maycup_print_ast(MAYCUP_AST *ast, ssize_t root) {
    char *res = maycup_ast_tostr(ast, root);
    printf("%s", res);
    free(res);
}

#endif // DEBUG