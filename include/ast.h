/**
 * @file ast.h
 * @brief Abstract Syntax tree
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

#ifndef AST_H
#define AST_H

#include "common.h"
#include "result.h"

typedef enum {
    M2H_ASTNODE_TYPE_NONE = 0,
    M2H_ASTNODE_TYPE_DOCUMENT = 1,
    
    M2H_ASTNODE_TYPE_HEADING,
    M2H_ASTNODE_TYPE_PARAGRAPH,

    M2H_ASTNODE_TYPE_BLANK,
    M2H_ASTNODE_TYPE_TEXT,
} M2H_ASTNodeType;

typedef enum {
    M2H_TEXTSTYLE_PLAIN,
    M2H_TEXTSTYLE_BOLD,
    M2H_TEXTSTYLE_ITALIC,
    M2H_TEXTSTYLE_BOLDITALIC,
    M2H_TEXTSTYLE_CODE,
} M2H_TextStyle;

typedef struct {
    M2H_TextStyle style;
    char *content;
} M2H_ASTNodeText;

typedef struct {
    uint8_t level;
} M2H_ASTNodeHeading;

typedef struct M2H_ASTNode M2H_ASTNode;

typedef M2H_ASTNode *astnode_ptr;
#define M2H_VEC_T astnode_ptr
#define M2H_VEC_DISPT ASTNodePtr
#include "vector.h"
#undef M2H_VEC_DISPT
#undef M2H_VEC_T

struct M2H_ASTNode {
    M2H_ASTNodeType type;
    union {
        M2H_ASTNodeHeading heading;
        M2H_ASTNodeText text;
    };
    M2H_VectorASTNodePtr children;
};

/**
 * @brief Construct an AST node
 * @param self Out, the node to construct
 * @param type In, the type of the node
 * @return M2H_Result
 */
M2H_Result M2H_astnode_ctor(M2H_OUT M2H_ASTNode *self,
                            M2H_IN M2H_ASTNodeType type);

/**
 * @brief Destroy an AST node
 * @param self Out, the node to destroy
 * @return M2H_Result
 */
M2H_Result M2H_astnode_dtor(M2H_OUT M2H_ASTNode *self);

/**
 * @brief Construct a text AST node
 * @param self Out, the node to construct
 * @param text Move, the pointer to a string to be moved into the node
 * @param style In, the style of the text
 * @return M2H_Result
 */
M2H_Result M2H_astnode_text_ctor(M2H_OUT M2H_ASTNodeText *self,
                                 M2H_MOVE char *text,
                                 M2H_IN M2H_TextStyle style);

/**
 * @brief Destroy a text AST node
 * @param self Out, the node to destroy
 * @return M2H_Result
 */
M2H_Result M2H_astnode_text_dtor(M2H_OUT M2H_ASTNodeText *self);

#endif // AST_H