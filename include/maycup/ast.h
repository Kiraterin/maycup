/**
 * @file ast.h
 * @brief Abstract Syntax tree
 * @date 2026-07-12
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

#ifndef AST_H
#define AST_H

#include "maycup/result.h"
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

typedef enum {
    MAYCUP_ASTNODE_TYPE_NONE = 0,
    MAYCUP_ASTNODE_TYPE_ROOT = 1,

    MAYCUP_ASTNODE_TYPE_HEADING,
    MAYCUP_ASTNODE_TYPE_PARAGRAPH,

    MAYCUP_ASTNODE_TYPE_TEXT,
} MAYCUP_ASTNodeType;

typedef enum {
    MAYCUP_TEXTSTYLE_PLAIN,
    MAYCUP_TEXTSTYLE_BOLD,
    MAYCUP_TEXTSTYLE_ITALIC,
    MAYCUP_TEXTSTYLE_BOLDITALIC,
    MAYCUP_TEXTSTYLE_CODE,
} MAYCUP_TextStyle;

typedef struct {
    MAYCUP_TextStyle style;
    char *content;
    bool newline_tailed;
} MAYCUP_ASTNodeDataText;

typedef struct {
    uint8_t level;
} MAYCUP_ASTNodeDataHeading;

typedef struct MAYCUP_ASTNode MAYCUP_ASTNode;

struct MAYCUP_ASTNode {
    MAYCUP_ASTNodeType type;
    union {
        MAYCUP_ASTNodeDataHeading heading;
        MAYCUP_ASTNodeDataText text;
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
    MAYCUP_ASTNode *data;
    ssize_t *next_free;
    bool *is_allocated;
    ssize_t first_free;
    size_t cap;
} MAYCUP_AST;

#define MAYCUP_DEFAULT_AST_SIZE 8
#define MAYCUP_MAX_AST_CAP (SIZE_MAX / 2)

/**
 * @brief Construct an AST
 * @note The index @c 0 is reserved and the root index will never be it
 * @param self Out, the AST to construct
 * @param root Out, the root of AST. Cannot be @c NULL
 * @return MAYCUP_Result
 */
MAYCUP_Result maycup_ast_ctor(MAYCUP_AST *self, ssize_t *root);

/**
 * @brief Destruct an AST
 * @note 1. All of the member will be @c 0 or @c NULL (except @c first_free ,
 *          which will be @c -1 )
 *       2. The allocated nodes will be properly destructed
 * @param self Out, the AST to destruct
 * @return MAYCUP_Result
 */
MAYCUP_Result maycup_ast_dtor(MAYCUP_AST *self);

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
 * @return MAYCUP_Result
 */
MAYCUP_Result maycup_insert_astnode(ssize_t *insertee, MAYCUP_AST *ast,
                                    ssize_t parent, MAYCUP_ASTNodeType type);

/**
 * @brief Delete an given ast node and all of its children
 * @note Parameter @p dest cannot be negative, @c 0 or any node index that
 *       haven't been allocated
 * @param ast Out, the AST where the function deletes
 * @param dest In & out, the node to delete
 * @return MAYCUP_Result
 */
MAYCUP_Result maycup_delete_astnode(MAYCUP_AST *ast, ssize_t dest);

/**
 * @brief Construct a text AST node data
 * @note The parameter @p text should only receive data that is from heap
 * @param self Out, the data to construct
 * @param text Move, the pointer to a string to be moved into the data
 * @param style In, the style of the text
 * @param newline_tailed In, whether the text tailed with newline
 * @return MAYCUP_Result
 */
MAYCUP_Result maycup_astnode_data_text_ctor(MAYCUP_ASTNodeDataText *self,
                                            char *text, MAYCUP_TextStyle style,
                                            bool newline_tailed);

/**
 * @brief Destruct a text AST node data
 * @note The member @c content will be freed and turn to @c NULL
 * @param self Out, the data to destruct
 * @return MAYCUP_Result
 */
MAYCUP_Result maycup_astnode_data_text_dtor(MAYCUP_ASTNodeDataText *self);

/**
 * @brief Destruct a AST node
 * @param self Out, the node to destruct
 * @return MAYCUP_Result
 */
MAYCUP_Result maycup_astnode_dtor(MAYCUP_ASTNode *self);

#endif // AST_H