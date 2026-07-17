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
 * along with this program.  If not, see <https: //www.gnu.org/licenses/>.
 */

#include "md2html/renderer.h"
#include <stdio.h>
#include <string.h>

typedef ssize_t idx;
#define M2H_VEC_T idx
#define M2H_VEC_DISPT Idx
#include "md2html/vector.h"
#undef M2H_VEC_DISPT
#undef M2H_VEC_T

M2H_Result M2H_renderer_ctor(M2H_OUT M2H_Renderer *self,
                             M2H_IN const char *path) {
    size_t len = strlen(path);
    self->output_file_path = (char *)malloc((len + 1) * sizeof(char));
    if (self->output_file_path == NULL) {
        return M2H_RESULT_MALLOC_FAIL;
    }
    strcpy(self->output_file_path, path);

    self->fp = fopen(self->output_file_path, "w");
    if (self->fp == NULL) {
        return M2H_RESULT_ERRNO;
    }

    return M2H_RESULT_OK;
}
M2H_Result M2H_renderer_dtor(M2H_OUT M2H_Renderer *self) {
    free(self->output_file_path);
    self->output_file_path = NULL;

    if (self->fp == NULL || fclose(self->fp) == EOF) {
        return M2H_RESULT_ERRNO;
    }
    self->fp = NULL;
    return M2H_RESULT_OK;
}

M2H_Result M2H_render(M2H_INOUT M2H_Renderer *renderer,
                      M2H_IN M2H_Parser *parser) {
    fputs("<!DOCTYPE html>", renderer->fp);
    M2H_VectorIdx stack;
    M2H_RELAY(M2H_vector_idx_ctor(&stack, M2H_DEFAULT_VEC_SIZE));
    M2H_RELAY(M2H_vector_idx_pushback(&stack, parser->root_astnode));
    int iores = 0;
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
                iores = fputs("<body>", renderer->fp);
                break;
            case M2H_ASTNODE_TYPE_HEADING:
                iores = fprintf(renderer->fp, "<h%d>", cur->heading.level);
                break;
            case M2H_ASTNODE_TYPE_PARAGRAPH:
                iores = fputs("<p>", renderer->fp);
                break;
            case M2H_ASTNODE_TYPE_TEXT:
                iores = fputs(cur->text.content, renderer->fp);
                break;
            }
        } else if (curidx < 0) {
            switch (cur->type) {
            case M2H_ASTNODE_TYPE_NONE:
                return M2H_RESULT_ILLEGAL_ARGUMENT;
            case M2H_ASTNODE_TYPE_ROOT:
                fputs("</body>", renderer->fp);
                break;
            case M2H_ASTNODE_TYPE_HEADING:
                fprintf(renderer->fp, "</h%d>", cur->heading.level);
                break;
            case M2H_ASTNODE_TYPE_PARAGRAPH:
                fputs("</p>", renderer->fp);
                break;
            case M2H_ASTNODE_TYPE_TEXT:
                if (cur->text.newline_tailed) {
                    fputs("<br>", renderer->fp);
                }
                break;
            }
        }

        if (iores < 0) {
            M2H_RELAY(M2H_vector_idx_dtor(&stack));
            return M2H_RESULT_ERRNO;
        }
    }

    M2H_RELAY(M2H_vector_idx_dtor(&stack));
    return M2H_RESULT_OK;
}