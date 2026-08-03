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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef AST_H
#define AST_H

#include "md2html/base/common.h"
#include "md2html/base/result.h"
#include <sys/types.h>

typedef enum {
    M2H_ASTNODE_TYPE_NONE = 0,
    M2H_ASTNODE_TYPE_ROOT = 1,

    M2H_ASTNODE_TYPE_HEADING,
    M2H_ASTNODE_TYPE_PARAGRAPH,

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
    bool newline_tailed;
} M2H_ASTNodeDataText;

typedef struct {
    uint8_t level;
} M2H_ASTNodeDataHeading;

typedef struct M2H_ASTNode M2H_ASTNode;

struct M2H_ASTNode {
    M2H_ASTNodeType type;
    union {
        M2H_ASTNodeDataHeading heading;
        M2H_ASTNodeDataText text;
    };

    ssize_t prev_sibling;
    ssize_t next_sibling;
    ssize_t parent;
    // point to the last child
    ssize_t child;
};

/**
 * @note Use expressions like @c ast.data[index] to get a node
 */
typedef struct {
    M2H_ASTNode *data;
    ssize_t *next_free;
    bool *is_allocated;
    ssize_t first_free;
    size_t cap;
} M2H_AST;

#define M2H_DEFAULT_AST_SIZE 8
#define M2H_MAX_AST_CAP (SIZE_MAX / 2)

/**
 * @brief Construct an AST
 * @note The index @c 0 is reserved and the root index will never be it
 * @param self Out, the AST to construct
 * @param root Out, the root of AST, cannot be @c NULL
 * @return M2H_Result
 */
M2H_Result M2H_ast_ctor(M2H_OUT M2H_AST *self, M2H_OUT ssize_t *root);

/**
 * @brief Destruct an AST
 * @note 1. All of the member will be @c 0 or @c NULL (except @c first_free ,
 *          which will be @c -1 )
 *       2. The allocated nodes will be properly destructed
 * @param self Out, the AST to destruct
 * @return M2H_Result
 */
M2H_Result M2H_ast_dtor(M2H_OUT M2H_AST *self);

/**
 * @brief Insert an AST node after a given node
 * @note 1. If a node doesn't have a child/parent/sibling, the corresponding
 *          data will be -1
 *       2. The function follows the head insertion method, meaning that the
 *          direct child of a parent is the last inserted, which makes
 *          stack-based DFS traversal easier
 *       3. Parameter @p parent cannot be negative, @c 0 or any node index that
 *          haven't been allocated
 * @param insertee Out, the inserted node. Can be @c NULL if you don't need it
 * @param ast Out, the AST where the function inserts
 * @param node In & out, the given node
 * @param type In, the type of node to insert
 * @return M2H_Result
 */
M2H_Result M2H_insert_astnode(M2H_OUT ssize_t *insertee, M2H_OUT M2H_AST *ast,
                              M2H_INOUT ssize_t parent,
                              M2H_IN M2H_ASTNodeType type);

/**
 * @brief Delete an given ast node and all of its children
 * @note Parameter @p dest cannot be negative, @c 0 or any node index that
 *       haven't been allocated
 * @param ast Out, the AST where the function deletes
 * @param dest In & out, the node to delete
 * @return M2H_Result
 */
M2H_Result M2H_delete_astnode(M2H_OUT M2H_AST *ast, M2H_INOUT ssize_t dest);

/**
 * @brief Construct a text AST node data
 * @note The parameter @p text should only receive data that is from heap
 * @param self Out, the data to construct
 * @param text Move, the pointer to a string to be moved into the data
 * @param style In, the style of the text
 * @param newline_tailed In, whether the text tailed with newline
 * @return M2H_Result
 */
M2H_Result M2H_astnode_data_text_ctor(M2H_OUT M2H_ASTNodeDataText *self,
                                      M2H_MOVE char *text,
                                      M2H_IN M2H_TextStyle style,
                                      M2H_IN bool newline_tailed);

/**
 * @brief Destruct a text AST node data
 * @note The member @c content will be freed and turn to @c NULL
 * @param self Out, the data to destruct
 * @return M2H_Result
 */
M2H_Result M2H_astnode_data_text_dtor(M2H_OUT M2H_ASTNodeDataText *self);

/**
 * @brief Destruct a AST node
 * @param self Out, the node to destruct
 * @return M2H_Result
 */
M2H_Result M2H_astnode_dtor(M2H_OUT M2H_ASTNode *self);

#endif // AST_H