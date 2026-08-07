/**
 * @file renderer.c
 * @brief Renderer in md2html
 * @date 2026-07-15
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

#include "md2html/core/renderer.h"
#include "md2html/io/writer.h"
#include <stdio.h>

typedef ssize_t idx;
#define M2H_VEC_T idx
#define M2H_VEC_DISPT Idx
#include "md2html/base/vector.h"
#undef M2H_VEC_DISPT
#undef M2H_VEC_T

// mock def
#include "mock_funcs.h"

M2H_Result M2H_renderer_ctor(M2H_OUT M2H_Renderer *self,
                             M2H_IN M2H_Writer *writer) {
    self->writer = writer;
    return M2H_RESULT_OK;
}
M2H_Result M2H_renderer_dtor(M2H_OUT M2H_Renderer *self) {
    (void)self;
    return M2H_RESULT_OK;
}

M2H_Result M2H_render(M2H_INOUT M2H_Renderer *renderer,
                      M2H_IN M2H_Parser *parser) {
    M2H_writer_puts(renderer->writer, "<!DOCTYPE html>");
    M2H_VectorIdx stack;
    M2H_RELAY(M2H_vector_idx_ctor(&stack, M2H_DEFAULT_VEC_SIZE));
    M2H_RELAY(M2H_vector_idx_pushback(&stack, parser->root_astnode));
    M2H_Result iores = 0;
    while (stack.len > 0) {
        ssize_t curidx;
        M2H_RELAY(M2H_vector_idx_top(&stack, &curidx));
        M2H_RELAY(M2H_vector_idx_popback(&stack));
        M2H_ASTNode *const cur =
            &parser->ast.data[(curidx > 0 ? curidx : -curidx)];

        if (curidx > 0) {
            // close mark
            M2H_RELAY(M2H_vector_idx_pushback(&stack, -curidx));
            ssize_t getter = cur->child;
            while (getter != -1) {
                M2H_RELAY(M2H_vector_idx_pushback(&stack, getter));
                getter = parser->ast.data[getter].prev_sibling;
            }
            switch (cur->type) {
            case M2H_ASTNODE_TYPE_NONE:
                return M2H_RESULT_ILLEGAL_ARGUMENT;
            case M2H_ASTNODE_TYPE_ROOT:
                iores = M2H_writer_puts(renderer->writer, "<body>");
                break;
            case M2H_ASTNODE_TYPE_HEADING:
                iores = M2H_writer_printf(renderer->writer, "<h%d>",
                                          cur->heading.level);
                break;
            case M2H_ASTNODE_TYPE_PARAGRAPH:
                iores = M2H_writer_puts(renderer->writer, "<p>");
                break;
            case M2H_ASTNODE_TYPE_TEXT:
                iores = M2H_writer_puts(renderer->writer, cur->text.content);
                break;
            }
        } else if (curidx < 0) {
            switch (cur->type) {
            case M2H_ASTNODE_TYPE_NONE:
                return M2H_RESULT_ILLEGAL_ARGUMENT;
            case M2H_ASTNODE_TYPE_ROOT:
                iores = M2H_writer_puts(renderer->writer, "</body>");
                break;
            case M2H_ASTNODE_TYPE_HEADING:
                iores = M2H_writer_printf(renderer->writer, "</h%d>",
                                          cur->heading.level);
                break;
            case M2H_ASTNODE_TYPE_PARAGRAPH:
                iores = M2H_writer_puts(renderer->writer, "</p>");
                break;
            case M2H_ASTNODE_TYPE_TEXT:
                if (cur->text.newline_tailed) {
                    iores = M2H_writer_puts(renderer->writer, "<br>");
                }
                break;
            }
        }

        if (iores != M2H_RESULT_OK) {
            M2H_RELAY(M2H_vector_idx_dtor(&stack));
            return iores;
        }
    }

    M2H_RELAY(M2H_vector_idx_dtor(&stack));
    return M2H_RESULT_OK;
}