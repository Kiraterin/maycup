/**
 * @file renderer.c
 * @brief Renderer in maycup
 * @date 2026-07-15
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

#include "maycup/core/renderer.h"
#include "maycup/base/result.h"
#include "maycup/io/writer.h"

typedef ssize_t idx;
#define MAYCUP_VEC_T idx
#define MAYCUP_VEC_DISPT Idx
#include "maycup/base/vector.h"
#undef MAYCUP_VEC_DISPT
#undef MAYCUP_VEC_T

// mock def
#include "mock_funcs.h"

MAYCUP_Result maycup_renderer_ctor(MAYCUP_OUT MAYCUP_Renderer *self,
                                   MAYCUP_IN MAYCUP_Writer *writer) {
    if (self == NULL || writer == NULL) {
        return MAYCUP_RESULT_ILLEGAL_ARGUMENT;
    }
    self->writer = writer;
    return MAYCUP_RESULT_OK;
}

MAYCUP_Result maycup_renderer_dtor(MAYCUP_OUT MAYCUP_Renderer *self) {
    if (self == NULL || self->writer == NULL) {
        return MAYCUP_RESULT_ILLEGAL_ARGUMENT;
    }
    self->writer = NULL;
    return MAYCUP_RESULT_OK;
}

MAYCUP_Result maycup_render(MAYCUP_INOUT MAYCUP_Renderer *renderer,
                            MAYCUP_IN MAYCUP_Parser *parser) {
    if (renderer == NULL || parser == NULL) {
        return MAYCUP_RESULT_ILLEGAL_ARGUMENT;
    }
    maycup_writer_puts(renderer->writer, "<!DOCTYPE html>");
    MAYCUP_VectorIdx stack;
    MAYCUP_RELAY(maycup_vector_idx_ctor(&stack, MAYCUP_DEFAULT_VEC_SIZE));
    MAYCUP_RELAY(maycup_vector_idx_pushback(&stack, parser->root_astnode));
    MAYCUP_Result iores = 0;
    while (stack.len > 0) {
        ssize_t curidx;
        MAYCUP_RELAY(maycup_vector_idx_top(&stack, &curidx));
        MAYCUP_RELAY(maycup_vector_idx_popback(&stack));
        MAYCUP_ASTNode *const cur =
            &parser->ast.data[(curidx > 0 ? curidx : -curidx)];

        if (curidx > 0) {
            // close mark
            MAYCUP_RELAY(maycup_vector_idx_pushback(&stack, -curidx));
            ssize_t getter = cur->child;
            while (getter != -1) {
                MAYCUP_RELAY(maycup_vector_idx_pushback(&stack, getter));
                getter = parser->ast.data[getter].prev_sibling;
            }
            switch (cur->type) {
            case MAYCUP_ASTNODE_TYPE_NONE:
                return MAYCUP_RESULT_ILLEGAL_ARGUMENT;
            case MAYCUP_ASTNODE_TYPE_ROOT:
                iores = maycup_writer_puts(renderer->writer, "<body>");
                break;
            case MAYCUP_ASTNODE_TYPE_HEADING:
                iores = maycup_writer_printf(renderer->writer, "<h%d>",
                                             cur->heading.level);
                break;
            case MAYCUP_ASTNODE_TYPE_PARAGRAPH:
                iores = maycup_writer_puts(renderer->writer, "<p>");
                break;
            case MAYCUP_ASTNODE_TYPE_TEXT:
                iores = maycup_writer_puts(renderer->writer, cur->text.content);
                break;
            }
        } else if (curidx < 0) {
            switch (cur->type) {
            case MAYCUP_ASTNODE_TYPE_NONE:
                return MAYCUP_RESULT_ILLEGAL_ARGUMENT;
            case MAYCUP_ASTNODE_TYPE_ROOT:
                iores = maycup_writer_puts(renderer->writer, "</body>");
                break;
            case MAYCUP_ASTNODE_TYPE_HEADING:
                iores = maycup_writer_printf(renderer->writer, "</h%d>",
                                             cur->heading.level);
                break;
            case MAYCUP_ASTNODE_TYPE_PARAGRAPH:
                iores = maycup_writer_puts(renderer->writer, "</p>");
                break;
            case MAYCUP_ASTNODE_TYPE_TEXT:
                if (cur->text.newline_tailed) {
                    iores = maycup_writer_puts(renderer->writer, "<br>");
                }
                break;
            }
        }

        if (iores != MAYCUP_RESULT_OK) {
            MAYCUP_RELAY(maycup_vector_idx_dtor(&stack));
            return iores;
        }
    }

    MAYCUP_RELAY(maycup_vector_idx_dtor(&stack));
    return MAYCUP_RESULT_OK;
}