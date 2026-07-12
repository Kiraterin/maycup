/**
 * @file ast.c
 * @brief Abstract syntax tree
 * @date 2026-07-12
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

#include "ast.h"

M2H_Result M2H_astnode_ctor(M2H_OUT M2H_ASTNode *self,
                            M2H_IN M2H_ASTNodeType type) {
    self->type = type;

    switch (self->type) {
    case M2H_ASTNODE_TYPE_HEADING:
    case M2H_ASTNODE_TYPE_PARAGRAPH:
        M2H_RELAY(
            M2H_vector_astnode_ptr_ctor(&self->children, M2H_DEFAULT_VEC_SIZE));
        break;
    default:
        self->children.ptr = NULL;
        self->children.cap = self->children.len = 0;
        break;
    }

    return M2H_RESULT_OK;
}

M2H_Result M2H_astnode_dtor(M2H_OUT M2H_ASTNode *self) {
    M2H_VectorASTNodePtr stack;
    M2H_RELAY(M2H_vector_astnode_ptr_pushback(&stack, self));

    while (stack.len > 0) {
        M2H_ASTNode *cur;
        M2H_RELAY(M2H_vector_astnode_ptr_top(&stack, &cur));
        M2H_RELAY(M2H_vector_astnode_ptr_popback(&stack));
        switch (cur->type) {
        case M2H_ASTNODE_TYPE_DOCUMENT:
        case M2H_ASTNODE_TYPE_HEADING:
        case M2H_ASTNODE_TYPE_PARAGRAPH: {
            while (cur->children.len > 0) {
                M2H_ASTNode *top;
                M2H_RELAY(M2H_vector_astnode_ptr_top(&cur->children, &top));
                M2H_RELAY(M2H_vector_astnode_ptr_pushback(&stack, top));
                M2H_RELAY(M2H_vector_astnode_ptr_popback(&cur->children));
            }
            M2H_RELAY(M2H_vector_astnode_ptr_dtor(&cur->children));
            break;
        }
        case M2H_ASTNODE_TYPE_TEXT:
            M2H_RELAY(M2H_astnode_text_dtor(&cur->text));
            break;
        default:
            break;
        }
    }

    self->type = M2H_ASTNODE_TYPE_NONE;
    return M2H_RESULT_OK;
}

M2H_Result M2H_astnode_text_ctor(M2H_OUT M2H_ASTNodeText *self,
                                 M2H_MOVE char *text,
                                 M2H_IN M2H_TextStyle style) {
    if (text == NULL) {
        return M2H_RESULT_ILLEGAL_ARGUMENT;
    }
    self->style = style;
    self->content = text;
    return M2H_RESULT_OK;
}
M2H_Result M2H_astnode_text_dtor(M2H_OUT M2H_ASTNodeText *self) {
    free(self->content);
    self->content = NULL;
    return M2H_RESULT_OK;
}