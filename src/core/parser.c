/**
 * @file parser.c
 * @brief Parser in maycup
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

#include "maycup/core/parser.h"
#include "maycup/base/result.h"
#include "maycup/core/lexer.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define MAYCUP_VEC_T char
#define MAYCUP_VEC_DISPT Char
#include "maycup/base/vector.h"
#undef MAYCUP_VEC_DISPT
#undef MAYCUP_VEC_T

// mock def
#include "mock_funcs.h"

static MAYCUP_Result advance(MAYCUP_Parser *parser, MAYCUP_Lexer *lexer) {
    MAYCUP_RELAY(maycup_token_dtor(&parser->cur_token));
    MAYCUP_RELAY(maycup_next_token(&parser->cur_token, lexer));
    return MAYCUP_RESULT_OK;
}

static MAYCUP_Result parse_inline_text(MAYCUP_Parser *parser,
                                       MAYCUP_Lexer *lexer, ssize_t parent,
                                       char *prefix) {
    if (parent == -1) {
        return MAYCUP_RESULT_ILLEGAL_ARGUMENT;
    }

    MAYCUP_VectorChar buffer;
    MAYCUP_RELAY(maycup_vector_char_ctor(&buffer, 16));
    char *buffer_end;
    size_t destlen;

    if (prefix != NULL) {
        // +1 for '\0'
        destlen = strlen(prefix) + 1;
        if (destlen >= buffer.cap) {
            MAYCUP_RELAY(maycup_vector_char_reserve(&buffer, destlen * 2));
        }
        buffer_end = strcpy(buffer.ptr, prefix) + strlen(buffer.ptr);
        // ignore the '\0'
        buffer.len = destlen - 1;
    }

    while (true) {
        buffer_end = buffer.ptr + buffer.len;
        switch (parser->cur_token.type) {
        case MAYCUP_TOKENTYPE_LITERAL: {
            MAYCUP_RELAY(maycup_vector_char_pushback(
                &buffer, (char)parser->cur_token.literal));
            break;
        }
        case MAYCUP_TOKENTYPE_TEXT: {
            // +1 for '\0'
            destlen = strlen(parser->cur_token.text) + 1 + buffer.len;
            if (destlen >= buffer.cap) {
                MAYCUP_RELAY(maycup_vector_char_reserve(&buffer, destlen * 2));
                buffer_end = buffer.ptr + buffer.len;
            }

            if (buffer_end == buffer.ptr) {
                // trim front spaces
                char *begin = parser->cur_token.text;
                while (isspace(*begin)) {
                    ++begin;
                }
                destlen = strlen(begin) + 1 + buffer.len;
                buffer_end = strcpy(buffer_end, begin) + strlen(buffer_end);
            } else {
                buffer_end = strcpy(buffer_end, parser->cur_token.text) +
                             strlen(buffer_end);
            }

            // ignore the '\0'
            buffer.len = destlen - 1;

            break;
        }
        default:
            goto loop_break;
        }
        MAYCUP_RELAY(advance(parser, lexer));
    }

loop_break: {
    MAYCUP_RELAY(maycup_vector_char_pushback(&buffer, '\0'));
    ssize_t insertee;
    MAYCUP_RELAY(maycup_insert_astnode(&insertee, &parser->ast, parent,
                                       MAYCUP_ASTNODE_TYPE_TEXT));
    MAYCUP_RELAY(maycup_astnode_data_text_ctor(&parser->ast.data[insertee].text,
                                               buffer.ptr,
                                               MAYCUP_TEXTSTYLE_PLAIN, true));
    return MAYCUP_RESULT_OK;
}
}

static MAYCUP_Result parse_heading_mark(MAYCUP_Parser *parser,
                                        MAYCUP_Lexer *lexer, uint8_t *level) {
    if (parser->cur_token.type != MAYCUP_TOKENTYPE_LITERAL ||
        parser->cur_token.literal != '#') {
        return MAYCUP_RESULT_PARSE_MISMATCH;
    }

    MAYCUP_RELAY(advance(parser, lexer));

    uint8_t _level = 1;
    while (parser->cur_token.type == MAYCUP_TOKENTYPE_LITERAL &&
           parser->cur_token.literal == '#') {
        ++_level;
        if (_level > 6) {
            return MAYCUP_RESULT_PARSE_MISMATCH;
        }
        MAYCUP_RELAY(advance(parser, lexer));
    }

    if (level != NULL) {
        *level = _level;
    }

    return MAYCUP_RESULT_OK;
}

static MAYCUP_Result parse_heading_mark_textbegin(MAYCUP_Parser *parser,
                                                  MAYCUP_Lexer *lexer,
                                                  uint8_t *level) {
    MAYCUP_RELAY(parse_heading_mark(parser, lexer, level));

    if (parser->cur_token.type != MAYCUP_TOKENTYPE_TEXT ||
        parser->cur_token.text[0] == ' ') {
        return MAYCUP_RESULT_PARSE_MISMATCH;
    }

    return MAYCUP_RESULT_OK;
}

static MAYCUP_Result parse_para_begin_char(MAYCUP_Parser *parser) {
    switch (parser->cur_token.type) {
    case MAYCUP_TOKENTYPE_TEXT:
    case MAYCUP_TOKENTYPE_LITERAL:
        return MAYCUP_RESULT_OK;
    default:
        return MAYCUP_RESULT_PARSE_MISMATCH;
    }
}

static MAYCUP_Result parse_blank(MAYCUP_Parser *parser, MAYCUP_Lexer *lexer) {
    if (parser->cur_token.type != MAYCUP_TOKENTYPE_NEWLINE) {
        return MAYCUP_RESULT_PARSE_MISMATCH;
    }
    while (parser->cur_token.type == MAYCUP_TOKENTYPE_NEWLINE) {
        MAYCUP_RELAY(advance(parser, lexer));
    }

    return MAYCUP_RESULT_OK;
}

static MAYCUP_Result parse_paragraph_section(MAYCUP_Parser *parser,
                                             MAYCUP_Lexer *lexer,
                                             ssize_t paranode) {
    if (parser->cur_token.type == MAYCUP_TOKENTYPE_NEWLINE) {
        return MAYCUP_RESULT_PARSE_MISMATCH;
    }

    char heading_marker[7] = {};
    uint8_t level = 0;
    {
        MAYCUP_Token tmp = (MAYCUP_Token){.type = MAYCUP_TOKENTYPE_NONE};

        MAYCUP_RELAY(maycup_lexer_checkpoint(lexer));
        MAYCUP_RELAY(maycup_token_duplicate(&tmp, &parser->cur_token));
        MAYCUP_RELAY_UNLESSOK_DO(
            parse_heading_mark_textbegin(parser, lexer, &level),
            MAYCUP_RESULT_PARSE_MISMATCH, goto drop_checkpoint);

        MAYCUP_RELAY(maycup_lexer_restore(lexer));
        MAYCUP_RELAY(maycup_token_dtor(&parser->cur_token));
        MAYCUP_RELAY(maycup_token_duplicate(&parser->cur_token, &tmp));
        MAYCUP_RELAY_UNLESSOK_DO(parse_para_begin_char(parser),
                                 MAYCUP_RESULT_PARSE_MISMATCH,
                                 goto drop_checkpoint);

        return MAYCUP_RESULT_PARSE_MISMATCH;

    drop_checkpoint:
        MAYCUP_RELAY(maycup_token_dtor(&tmp));
        maycup_lexer_drop_checkpoint(lexer);
    }

    if (level != 0) {
        for (int i = 0; i < level; ++i) {
            heading_marker[i] = '#';
        }
    }

    MAYCUP_RELAY(parse_inline_text(parser, lexer, paranode,
                                   level == 0 ? NULL : heading_marker));

    if (parser->cur_token.type != MAYCUP_TOKENTYPE_NEWLINE) {
        return MAYCUP_RESULT_PARSE_MISMATCH;
    }
    MAYCUP_RELAY(advance(parser, lexer));

    return MAYCUP_RESULT_OK;
}

static MAYCUP_Result parse_heading(MAYCUP_Parser *parser, MAYCUP_Lexer *lexer,
                                   ssize_t parent) {
    ssize_t heading_node;
    MAYCUP_RELAY(maycup_insert_astnode(&heading_node, &parser->ast, parent,
                                       MAYCUP_ASTNODE_TYPE_HEADING));

    uint8_t level;
    MAYCUP_RELAY_UNLESS_DO(parse_heading_mark(parser, lexer, &level),
                           MAYCUP_RESULT_OK, MAYCUP_RESULT_PARSE_MISMATCH,
                           goto mismatch);

    parser->ast.data[heading_node].heading.level = level;

    if (parser->cur_token.type != MAYCUP_TOKENTYPE_TEXT ||
        parser->cur_token.text[0] != ' ') {
        goto mismatch;
    }

    MAYCUP_RELAY_UNLESS_DO(parse_inline_text(parser, lexer, heading_node, NULL),
                           MAYCUP_RESULT_OK, MAYCUP_RESULT_PARSE_MISMATCH,
                           goto mismatch);

    if (parser->cur_token.type != MAYCUP_TOKENTYPE_NEWLINE) {
        goto mismatch;
    }
    MAYCUP_RELAY(advance(parser, lexer));

    return MAYCUP_RESULT_OK;

mismatch:
    MAYCUP_RELAY(maycup_delete_astnode(&parser->ast, heading_node));
    return MAYCUP_RESULT_PARSE_MISMATCH;
}

static MAYCUP_Result parse_paragraph(MAYCUP_Parser *parser, MAYCUP_Lexer *lexer,
                                     ssize_t parent) {
    ssize_t paranode;
    MAYCUP_RELAY(maycup_insert_astnode(&paranode, &parser->ast, parent,
                                       MAYCUP_ASTNODE_TYPE_PARAGRAPH));

    MAYCUP_RELAY_UNLESS_DO(parse_paragraph_section(parser, lexer, paranode),
                           MAYCUP_RESULT_OK, MAYCUP_RESULT_PARSE_MISMATCH,
                           goto mismatch);

    while (true) {
        MAYCUP_Result res = parse_paragraph_section(parser, lexer, paranode);
        switch (res) {
        case MAYCUP_RESULT_OK:
            continue;
        case MAYCUP_RESULT_PARSE_MISMATCH:
            goto loop_break;
        default:
            goto mismatch;
        }
    }

loop_break:
    return MAYCUP_RESULT_OK;

mismatch:
    MAYCUP_RELAY(maycup_delete_astnode(&parser->ast, paranode));
    return MAYCUP_RESULT_PARSE_MISMATCH;
}

static MAYCUP_Result parse_eof(MAYCUP_Parser *parser) {
    if (parser->cur_token.type == MAYCUP_TOKENTYPE_EOF) {
        return MAYCUP_RESULT_OK;
    } else {
        return MAYCUP_RESULT_PARSE_MISMATCH;
    }
}

static MAYCUP_Result parse_blocks(MAYCUP_Parser *parser, MAYCUP_Lexer *lexer) {
    while (parser->cur_token.type != MAYCUP_TOKENTYPE_EOF) {
        MAYCUP_Token tmp = (MAYCUP_Token){.type = MAYCUP_TOKENTYPE_NONE};

        MAYCUP_RELAY(maycup_lexer_checkpoint(lexer));
        MAYCUP_RELAY(maycup_token_duplicate(&tmp, &parser->cur_token));
        MAYCUP_RELAY_UNLESSOK_DO(
            parse_heading(parser, lexer, parser->root_astnode),
            MAYCUP_RESULT_PARSE_MISMATCH, goto drop_checkpoint);

        MAYCUP_RELAY(maycup_lexer_restore(lexer));
        MAYCUP_RELAY(maycup_token_dtor(&parser->cur_token));
        MAYCUP_RELAY(maycup_token_duplicate(&parser->cur_token, &tmp));
        MAYCUP_RELAY_UNLESSOK_DO(
            parse_paragraph(parser, lexer, parser->root_astnode),
            MAYCUP_RESULT_PARSE_MISMATCH, goto drop_checkpoint);

        MAYCUP_RELAY(maycup_lexer_restore(lexer));
        MAYCUP_RELAY(maycup_token_dtor(&parser->cur_token));
        MAYCUP_RELAY(maycup_token_duplicate(&parser->cur_token, &tmp));
        MAYCUP_RELAY_UNLESSOK_DO(parse_blank(parser, lexer),
                                 MAYCUP_RESULT_PARSE_MISMATCH,
                                 goto drop_checkpoint);

        MAYCUP_RELAY(maycup_lexer_restore(lexer));
        MAYCUP_RELAY(maycup_token_dtor(&parser->cur_token));
        MAYCUP_RELAY(maycup_token_duplicate(&parser->cur_token, &tmp));
        MAYCUP_RELAY_UNLESSOK_DO(parse_eof(parser),
                                 MAYCUP_RESULT_PARSE_MISMATCH, {
                                     maycup_lexer_drop_checkpoint(lexer);
                                     MAYCUP_RELAY(maycup_token_dtor(&tmp));
                                     break;
                                 });

        return MAYCUP_RESULT_PARSE_MISMATCH;

    drop_checkpoint:
        MAYCUP_RELAY(maycup_token_dtor(&tmp));
        maycup_lexer_drop_checkpoint(lexer);
        continue;
    }
    return MAYCUP_RESULT_OK;
}

MAYCUP_Result maycup_parser_ctor(MAYCUP_OUT MAYCUP_Parser *self) {
    if (self == NULL) {
        return MAYCUP_RESULT_ILLEGAL_ARGUMENT;
    }
    MAYCUP_RELAY(maycup_ast_ctor(&self->ast, &self->root_astnode));
    self->cur_token = (MAYCUP_Token){.type = MAYCUP_TOKENTYPE_NONE};
    return MAYCUP_RESULT_OK;
}

MAYCUP_Result maycup_parser_dtor(MAYCUP_OUT MAYCUP_Parser *self) {
    if (self == NULL || self->ast.data == NULL) {
        return MAYCUP_RESULT_ILLEGAL_ARGUMENT;
    }
    MAYCUP_RELAY(maycup_ast_dtor(&self->ast));
    return MAYCUP_RESULT_OK;
}

MAYCUP_Result maycup_parse(MAYCUP_INOUT MAYCUP_Parser *parser,
                           MAYCUP_INOUT MAYCUP_Lexer *lexer) {
    if (parser == NULL || lexer == NULL) {
        return MAYCUP_RESULT_ILLEGAL_ARGUMENT;
    }
    if (parser->cur_token.type == MAYCUP_TOKENTYPE_NONE) {
        MAYCUP_RELAY(advance(parser, lexer));
    }
    MAYCUP_RELAY(parse_blocks(parser, lexer));
    return MAYCUP_RESULT_OK;
}