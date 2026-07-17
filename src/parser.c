/**
 * @file parser.c
 * @brief Parser in md2html
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

#include "md2html/parser.h"
#include <ctype.h>
#include <string.h>

#define M2H_VEC_T char
#define M2H_VEC_DISPT Char
#include "md2html/vector.h"
#undef M2H_VEC_DISPT
#undef M2H_VEC_T

static M2H_Result advance(M2H_Parser *parser, M2H_Lexer *lexer) {
    M2H_RELAY(M2H_token_dtor(&parser->cur_token));
    M2H_RELAY(M2H_next_token(&parser->cur_token, lexer));
    return M2H_RESULT_OK;
}

static M2H_Result parse_inline_text(M2H_Parser *parser, M2H_Lexer *lexer,
                                    ssize_t parent, char *prefix) {
    if (parent == -1) {
        return M2H_RESULT_ILLEGAL_ARGUMENT;
    }

    M2H_VectorChar buffer;
    M2H_RELAY(M2H_vector_char_ctor(&buffer, 16));
    char *buffer_end;
    size_t destlen;

    if (prefix != NULL) {
        // +1 for '\0'
        destlen = strlen(prefix) + 1;
        if (destlen >= buffer.cap) {
            M2H_RELAY(M2H_vector_char_reserve(&buffer, destlen * 2));
        }
        buffer_end = stpcpy(buffer.ptr, prefix);
        // ignore the '\0'
        buffer.len = destlen - 1;
    }

    while (true) {
        buffer_end = buffer.ptr + buffer.len;
        switch (parser->cur_token.type) {
        case M2H_TOKENTYPE_LITERAL: {
            M2H_RELAY(M2H_vector_char_pushback(
                &buffer, (char)parser->cur_token.literal));
            break;
        }
        case M2H_TOKENTYPE_TEXT: {
            // +1 for '\0'
            destlen = strlen(parser->cur_token.text) + 1 + buffer.len;
            if (destlen >= buffer.cap) {
                M2H_RELAY(M2H_vector_char_reserve(&buffer, destlen * 2));
                buffer_end = buffer.ptr + buffer.len;
            }

            if (buffer_end == buffer.ptr) {
                // trim front spaces
                char *begin = parser->cur_token.text;
                while (isspace(*begin)) {
                    ++begin;
                }
                destlen = strlen(begin) + 1 + buffer.len;
                buffer_end = stpcpy(buffer_end, begin);
            } else {
                buffer_end = stpcpy(buffer_end, parser->cur_token.text);
            }

            // ignore the '\0'
            buffer.len = destlen - 1;

            break;
        }
        default:
            goto loop_break;
        }
        M2H_RELAY(advance(parser, lexer));
    }

loop_break: {
    M2H_RELAY(M2H_vector_char_pushback(&buffer, '\0'));
    ssize_t insertee;
    M2H_RELAY(M2H_insert_astnode(&insertee, &parser->ast, parent,
                                 M2H_ASTNODE_TYPE_TEXT));
    M2H_RELAY(M2H_astnode_data_text_ctor(&parser->ast.data[insertee].text,
                                         buffer.ptr, M2H_TEXTSTYLE_PLAIN,
                                         true));
    return M2H_RESULT_OK;
}
}

static M2H_Result parse_heading_mark(M2H_Parser *parser, M2H_Lexer *lexer,
                                     uint8_t *level) {
    if (parser->cur_token.type != M2H_TOKENTYPE_LITERAL ||
        parser->cur_token.literal != '#') {
        return M2H_RESULT_PARSE_MISMATCH;
    }

    M2H_RELAY(advance(parser, lexer));

    uint8_t _level = 1;
    while (parser->cur_token.type == M2H_TOKENTYPE_LITERAL &&
           parser->cur_token.literal == '#') {
        ++_level;
        if (_level > 6) {
            return M2H_RESULT_PARSE_MISMATCH;
        }
        M2H_RELAY(advance(parser, lexer));
    }

    if (level != NULL) {
        *level = _level;
    }

    return M2H_RESULT_OK;
}

static M2H_Result parse_heading_mark_textbegin(M2H_Parser *parser,
                                               M2H_Lexer *lexer,
                                               uint8_t *level) {
    M2H_RELAY(parse_heading_mark(parser, lexer, level));

    if (parser->cur_token.type != M2H_TOKENTYPE_TEXT ||
        parser->cur_token.text[0] == ' ') {
        return M2H_RESULT_PARSE_MISMATCH;
    }

    return M2H_RESULT_OK;
}

static M2H_Result parse_para_begin_char(M2H_Parser *parser) {
    if (parser->cur_token.type != M2H_TOKENTYPE_TEXT ||
        parser->cur_token.text[0] == '#') {
        return M2H_RESULT_PARSE_MISMATCH;
    }
    return M2H_RESULT_OK;
}

static M2H_Result parse_blank(M2H_Parser *parser, M2H_Lexer *lexer) {
    if (parser->cur_token.type != M2H_TOKENTYPE_NEWLINE) {
        return M2H_RESULT_PARSE_MISMATCH;
    }
    while (parser->cur_token.type == M2H_TOKENTYPE_NEWLINE) {
        M2H_RELAY(advance(parser, lexer));
    }

    return M2H_RESULT_OK;
}

static M2H_Result parse_paragraph_section(M2H_Parser *parser, M2H_Lexer *lexer,
                                          ssize_t paranode) {
    if (parser->cur_token.type == M2H_TOKENTYPE_NEWLINE) {
        return M2H_RESULT_PARSE_MISMATCH;
    }

    char heading_marker[7] = {};
    uint8_t level = 0;
    {
        M2H_Token tmp = (M2H_Token){.type = M2H_TOKENTYPE_NONE};

        M2H_RELAY(M2H_lexer_checkpoint(lexer));
        M2H_RELAY(M2H_token_duplicate(&tmp, &parser->cur_token));
        M2H_RELAY_UNLESSOK_DO(
            parse_heading_mark_textbegin(parser, lexer, &level),
            M2H_RESULT_PARSE_MISMATCH, goto drop_checkpoint);

        M2H_RELAY(M2H_lexer_restore(lexer));
        M2H_RELAY(M2H_token_dtor(&parser->cur_token));
        M2H_RELAY(M2H_token_duplicate(&parser->cur_token, &tmp));
        M2H_RELAY_UNLESSOK_DO(parse_para_begin_char(parser),
                              M2H_RESULT_PARSE_MISMATCH, goto drop_checkpoint);

        return M2H_RESULT_PARSE_MISMATCH;

    drop_checkpoint:
        M2H_RELAY(M2H_token_dtor(&tmp));
        M2H_lexer_drop_checkpoint(lexer);
    }

    if (level != 0) {
        for (int i = 0; i < level; ++i) {
            heading_marker[i] = '#';
        }
    }

    M2H_RELAY(parse_inline_text(parser, lexer, paranode, level == 0 ? NULL : heading_marker));

    if (parser->cur_token.type != M2H_TOKENTYPE_NEWLINE) {
        return M2H_RESULT_PARSE_MISMATCH;
    }
    M2H_RELAY(advance(parser, lexer));

    return M2H_RESULT_OK;
}

static M2H_Result parse_heading(M2H_Parser *parser, M2H_Lexer *lexer,
                                ssize_t parent) {
    ssize_t heading_node;
    M2H_RELAY(M2H_insert_astnode(&heading_node, &parser->ast, parent,
                                 M2H_ASTNODE_TYPE_HEADING));

    uint8_t level;
    M2H_RELAY_UNLESS_DO(parse_heading_mark(parser, lexer, &level),
                        M2H_RESULT_OK, M2H_RESULT_PARSE_MISMATCH,
                        goto mismatch);

    parser->ast.data[heading_node].heading.level = level;

    if (parser->cur_token.type != M2H_TOKENTYPE_TEXT ||
        parser->cur_token.text[0] != ' ') {
        goto mismatch;
    }

    M2H_RELAY_UNLESS_DO(parse_inline_text(parser, lexer, heading_node, NULL),
                        M2H_RESULT_OK, M2H_RESULT_PARSE_MISMATCH,
                        goto mismatch);

    if (parser->cur_token.type != M2H_TOKENTYPE_NEWLINE) {
        goto mismatch;
    }
    M2H_RELAY(advance(parser, lexer));

    return M2H_RESULT_OK;

mismatch:
    M2H_RELAY(M2H_delete_astnode(&parser->ast, heading_node));
    return M2H_RESULT_PARSE_MISMATCH;
}

static M2H_Result parse_paragraph(M2H_Parser *parser, M2H_Lexer *lexer,
                                  ssize_t parent) {
    ssize_t paranode;
    M2H_RELAY(M2H_insert_astnode(&paranode, &parser->ast, parent,
                                 M2H_ASTNODE_TYPE_PARAGRAPH));

    M2H_RELAY_UNLESS_DO(parse_paragraph_section(parser, lexer, paranode),
                        M2H_RESULT_OK, M2H_RESULT_PARSE_MISMATCH,
                        goto mismatch);

    while (true) {
        M2H_Result res = parse_paragraph_section(parser, lexer, paranode);
        switch (res) {
        case M2H_RESULT_OK:
            continue;
        case M2H_RESULT_PARSE_MISMATCH:
            goto loop_break;
        default:
            goto mismatch;
        }
    }

loop_break:
    return M2H_RESULT_OK;

mismatch:
    M2H_RELAY(M2H_delete_astnode(&parser->ast, paranode));
    return M2H_RESULT_PARSE_MISMATCH;
}

static M2H_Result parse_eof(M2H_Parser *parser) {
    if (parser->cur_token.type == M2H_TOKENTYPE_EOF) {
        return M2H_RESULT_OK;
    } else {
        return M2H_RESULT_PARSE_MISMATCH;
    }
}

static M2H_Result parse_blocks(M2H_Parser *parser, M2H_Lexer *lexer) {
    while (parser->cur_token.type != M2H_TOKENTYPE_EOF) {
        M2H_Token tmp = (M2H_Token){.type = M2H_TOKENTYPE_NONE};

        M2H_RELAY(M2H_lexer_checkpoint(lexer));
        M2H_RELAY(M2H_token_duplicate(&tmp, &parser->cur_token));
        M2H_RELAY_UNLESSOK_DO(
            parse_heading(parser, lexer, parser->root_astnode),
            M2H_RESULT_PARSE_MISMATCH, goto drop_checkpoint);

        M2H_RELAY(M2H_lexer_restore(lexer));
        M2H_RELAY(M2H_token_dtor(&parser->cur_token));
        M2H_RELAY(M2H_token_duplicate(&parser->cur_token, &tmp));
        M2H_RELAY_UNLESSOK_DO(
            parse_paragraph(parser, lexer, parser->root_astnode),
            M2H_RESULT_PARSE_MISMATCH, goto drop_checkpoint);

        M2H_RELAY(M2H_lexer_restore(lexer));
        M2H_RELAY(M2H_token_dtor(&parser->cur_token));
        M2H_RELAY(M2H_token_duplicate(&parser->cur_token, &tmp));
        M2H_RELAY_UNLESSOK_DO(parse_blank(parser, lexer),
                              M2H_RESULT_PARSE_MISMATCH, goto drop_checkpoint);

        M2H_RELAY(M2H_lexer_restore(lexer));
        M2H_RELAY(M2H_token_dtor(&parser->cur_token));
        M2H_RELAY(M2H_token_duplicate(&parser->cur_token, &tmp));
        M2H_RELAY_UNLESSOK_DO(parse_eof(parser), M2H_RESULT_PARSE_MISMATCH, {
            M2H_lexer_drop_checkpoint(lexer);
            M2H_RELAY(M2H_token_dtor(&tmp));
            break;
        });

        return M2H_RESULT_PARSE_MISMATCH;

    drop_checkpoint:
        M2H_RELAY(M2H_token_dtor(&tmp));
        M2H_lexer_drop_checkpoint(lexer);
        continue;
    }
    return M2H_RESULT_OK;
}

M2H_Result M2H_parser_ctor(M2H_OUT M2H_Parser *self) {
    M2H_RELAY(M2H_ast_ctor(&self->ast, &self->root_astnode));
    self->cur_token = (M2H_Token){.type = M2H_TOKENTYPE_NONE};
    return M2H_RESULT_OK;
}

M2H_Result M2H_parser_dtor(M2H_OUT M2H_Parser *self) {
    M2H_RELAY(M2H_ast_dtor(&self->ast));
    return M2H_RESULT_OK;
}

M2H_Result M2H_parse(M2H_INOUT M2H_Parser *parser, M2H_INOUT M2H_Lexer *lexer) {
    if (parser->cur_token.type == M2H_TOKENTYPE_NONE) {
        M2H_RELAY(advance(parser, lexer));
    }
    M2H_RELAY(parse_blocks(parser, lexer));
    return M2H_RESULT_OK;
}