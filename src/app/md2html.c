/**
 * @file maycup.c
 * @brief Sealed functions
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

#include "maycup.h"
#include "maycup/core/renderer.h"
#include "maycup/io/reader.h"

// mock def
#include "mock_funcs.h"

MAYCUP_Result MAYCUP_convert_file(MAYCUP_IN const char *input_file,
                            MAYCUP_IN const char *output_file) {
    MAYCUP_FileReader freader;
    MAYCUP_RELAY(MAYCUP_filereader_ctor(&freader, input_file));
    MAYCUP_FileWriter fwriter;
    MAYCUP_RELAY(MAYCUP_filewriter_ctor(&fwriter, output_file));
    MAYCUP_Lexer lexer;
    MAYCUP_RELAY(MAYCUP_lexer_ctor(&lexer, (MAYCUP_Reader *)&freader));
    MAYCUP_Parser parser;
    MAYCUP_RELAY(MAYCUP_parser_ctor(&parser));
    MAYCUP_Renderer renderer;
    MAYCUP_RELAY(MAYCUP_renderer_ctor(&renderer, (MAYCUP_Writer *)&fwriter));

    MAYCUP_RELAY(MAYCUP_parse(&parser, &lexer));
    MAYCUP_RELAY(MAYCUP_render(&renderer, &parser));

    MAYCUP_RELAY(MAYCUP_renderer_dtor(&renderer));
    MAYCUP_RELAY(MAYCUP_parser_dtor(&parser));
    MAYCUP_RELAY(MAYCUP_lexer_dtor(&lexer));
    MAYCUP_RELAY(MAYCUP_filereader_dtor(&freader));
    return MAYCUP_RESULT_OK;
}

MAYCUP_Result MAYCUP_convert_string(MAYCUP_IN const char *input_string,
                              MAYCUP_IN const size_t input_len,
                              MAYCUP_IN char *output_string,
                              MAYCUP_IN const size_t output_len) {
    MAYCUP_StringReader sreader;
    MAYCUP_RELAY(MAYCUP_stringreader_ctor(&sreader, input_string, input_len));
    MAYCUP_StringWriter swriter;
    if (output_len == 0) {
        MAYCUP_RELAY(MAYCUP_stringwriter_ctor_flexible(
            &swriter, MAYCUP_DEFAULT_STRWRITER_FLEXBUF_SIZE));
    } else {
        MAYCUP_RELAY(MAYCUP_stringwriter_ctor(&swriter, output_string, output_len));
    }
    MAYCUP_Lexer lexer;
    MAYCUP_RELAY(MAYCUP_lexer_ctor(&lexer, (MAYCUP_Reader *)&sreader));
    MAYCUP_Parser parser;
    MAYCUP_RELAY(MAYCUP_parser_ctor(&parser));
    MAYCUP_Renderer renderer;
    MAYCUP_RELAY(MAYCUP_renderer_ctor(&renderer, (MAYCUP_Writer *)&swriter));

    MAYCUP_RELAY(MAYCUP_parse(&parser, &lexer));
    MAYCUP_RELAY(MAYCUP_render(&renderer, &parser));

    MAYCUP_RELAY(MAYCUP_renderer_dtor(&renderer));
    MAYCUP_RELAY(MAYCUP_parser_dtor(&parser));
    MAYCUP_RELAY(MAYCUP_lexer_dtor(&lexer));
    MAYCUP_RELAY(MAYCUP_stringwriter_dtor(&swriter, NULL));
    MAYCUP_RELAY(MAYCUP_stringreader_dtor(&sreader));
    return MAYCUP_RESULT_OK;
}