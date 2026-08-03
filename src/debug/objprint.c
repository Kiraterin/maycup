/**
 * @file objprint.c
 * @brief Print human-readable structures
 * @date 2026-08-04
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

#ifdef DEBUG

#include "md2html/core/ast.h"
#include "md2html/core/lexer.h"
#include <stdio.h>

typedef ssize_t idx;
#define M2H_VEC_T idx
#define M2H_VEC_DISPT Idx
#include "md2html/base/vector.h"
#undef M2H_VEC_DISPT
#undef M2H_VEC_T

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

void M2H_print_ast(M2H_IN M2H_AST *ast, M2H_IN ssize_t root) {
    M2H_VectorIdx stack;
    M2H_VectorIdx level;
    M2H_UNWRAP(M2H_vector_idx_ctor(&stack, M2H_DEFAULT_VEC_SIZE));
    M2H_UNWRAP(M2H_vector_idx_ctor(&level, M2H_DEFAULT_VEC_SIZE));
    M2H_UNWRAP(M2H_vector_idx_pushback(&stack, root));
    M2H_UNWRAP(M2H_vector_idx_pushback(&level, 0));
    while (stack.len > 0) {
        ssize_t cur, lvl;
        M2H_UNWRAP(M2H_vector_idx_top(&stack, &cur));
        M2H_UNWRAP(M2H_vector_idx_popback(&stack));
        M2H_UNWRAP(M2H_vector_idx_top(&level, &lvl));
        M2H_UNWRAP(M2H_vector_idx_popback(&level));

        ssize_t getter = ast->data[cur].child;
        while (getter != -1) {
            M2H_UNWRAP(M2H_vector_idx_pushback(&stack, getter));
            M2H_UNWRAP(M2H_vector_idx_pushback(&level, lvl + 1));
            getter = ast->data[getter].prev_sibling;
        }

        for (int i = 1; i <= lvl; ++i) {
            printf("  ");
        }

        switch (ast->data[cur].type) {
        case M2H_ASTNODE_TYPE_ROOT:
            printf("ROOT");
            break;
        case M2H_ASTNODE_TYPE_NONE:
            printf("NONE");
            break;
        case M2H_ASTNODE_TYPE_PARAGRAPH:
            printf("PARAGRAPH");
            break;
        case M2H_ASTNODE_TYPE_HEADING:
            printf("HEADING level=%d", ast->data[cur].heading.level);
            break;
        case M2H_ASTNODE_TYPE_TEXT:
            printf("TEXT text=\"%s\", style=", ast->data[cur].text.content);
            switch (ast->data[cur].text.style) {
            case M2H_TEXTSTYLE_PLAIN:
                printf("plain");
                break;
            case M2H_TEXTSTYLE_BOLD:
                printf("bold");
                break;
            case M2H_TEXTSTYLE_ITALIC:
                printf("italic");
                break;
            case M2H_TEXTSTYLE_BOLDITALIC:
                printf("bold & italic");
                break;
            case M2H_TEXTSTYLE_CODE:
                printf("code");
                break;
            }
        }
        putchar('\n');
    }

    M2H_UNWRAP(M2H_vector_idx_dtor(&stack));
    M2H_UNWRAP(M2H_vector_idx_dtor(&level));
}

#endif // DEBUG