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

#include "maycup/base/result.h"
#include <stdlib.h>
#include <string.h>
#ifdef DEBUG

#include "maycup/base/common.h"
#include "maycup/core/ast.h"
#include "maycup/core/lexer.h"
#include <stdio.h>

typedef ssize_t idx;
#define MAYCUP_VEC_T idx
#define MAYCUP_VEC_DISPT Idx
#include "maycup/base/vector.h"
#undef MAYCUP_VEC_DISPT
#undef MAYCUP_VEC_T

char *maycup_token_tostr(MAYCUP_IN MAYCUP_Token *token) {
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

void maycup_print_token(MAYCUP_IN MAYCUP_Token *token) {
    char *str = maycup_token_tostr(token);
    printf("%s\n", str);
    free(str);
}

void maycup_print_ast(MAYCUP_IN MAYCUP_AST *ast, MAYCUP_IN ssize_t root) {
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
            printf("  ");
        }

        switch (ast->data[cur].type) {
        case MAYCUP_ASTNODE_TYPE_ROOT:
            printf("ROOT");
            break;
        case MAYCUP_ASTNODE_TYPE_NONE:
            printf("NONE");
            break;
        case MAYCUP_ASTNODE_TYPE_PARAGRAPH:
            printf("PARAGRAPH");
            break;
        case MAYCUP_ASTNODE_TYPE_HEADING:
            printf("HEADING level=%d", ast->data[cur].heading.level);
            break;
        case MAYCUP_ASTNODE_TYPE_TEXT:
            printf("TEXT text=\"%s\", style=", ast->data[cur].text.content);
            switch (ast->data[cur].text.style) {
            case MAYCUP_TEXTSTYLE_PLAIN:
                printf("plain");
                break;
            case MAYCUP_TEXTSTYLE_BOLD:
                printf("bold");
                break;
            case MAYCUP_TEXTSTYLE_ITALIC:
                printf("italic");
                break;
            case MAYCUP_TEXTSTYLE_BOLDITALIC:
                printf("bold & italic");
                break;
            case MAYCUP_TEXTSTYLE_CODE:
                printf("code");
                break;
            }
        }
        putchar('\n');
    }

    MAYCUP_UNWRAP(maycup_vector_idx_dtor(&stack));
    MAYCUP_UNWRAP(maycup_vector_idx_dtor(&level));
}

#endif // DEBUG