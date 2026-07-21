/**
 * @file md2html.c
 * @brief Sealed functions
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

#include "md2html.h"
#include "md2html/core/renderer.h"
#include "md2html/io/reader.h"

M2H_Result M2H_convert_file(M2H_IN const char *input_file,
                            M2H_IN const char *output_file) {
    M2H_FileReader freader;
    M2H_RELAY(M2H_filereader_ctor(&freader, input_file));
    M2H_FileWriter fwriter;
    M2H_RELAY(M2H_filewriter_ctor(&fwriter, output_file));
    M2H_Lexer lexer;
    M2H_RELAY(M2H_lexer_ctor(&lexer, (M2H_Reader *)&freader));
    M2H_Parser parser;
    M2H_RELAY(M2H_parser_ctor(&parser));
    M2H_Renderer renderer;
    M2H_RELAY(M2H_renderer_ctor(&renderer, (M2H_Writer *)&fwriter));

    M2H_RELAY(M2H_parse(&parser, &lexer));
    M2H_RELAY(M2H_render(&renderer, &parser));

    M2H_RELAY(M2H_renderer_dtor(&renderer));
    M2H_RELAY(M2H_parser_dtor(&parser));
    M2H_RELAY(M2H_lexer_dtor(&lexer));
    M2H_RELAY(M2H_filereader_dtor(&freader));
    return M2H_RESULT_OK;
}

M2H_Result M2H_convert_string(M2H_IN const char *input_string,
                              M2H_IN const size_t input_len,
                              M2H_IN char *output_string,
                              M2H_IN const size_t output_len) {
    M2H_StringReader sreader;
    M2H_RELAY(M2H_stringreader_ctor(&sreader, input_string, input_len));
    M2H_StringWriter swriter;
    if (output_len == 0) {
        M2H_RELAY(M2H_stringwriter_ctor_flexible(
            &swriter, M2H_DEFAULT_STRWRITER_FLEXBUF_SIZE));
    } else {
        M2H_RELAY(M2H_stringwriter_ctor(&swriter, output_string, output_len));
    }
    M2H_Lexer lexer;
    M2H_RELAY(M2H_lexer_ctor(&lexer, (M2H_Reader *)&sreader));
    M2H_Parser parser;
    M2H_RELAY(M2H_parser_ctor(&parser));
    M2H_Renderer renderer;
    M2H_RELAY(M2H_renderer_ctor(&renderer, (M2H_Writer *)&swriter));

    M2H_RELAY(M2H_parse(&parser, &lexer));
    M2H_RELAY(M2H_render(&renderer, &parser));

    M2H_RELAY(M2H_renderer_dtor(&renderer));
    M2H_RELAY(M2H_parser_dtor(&parser));
    M2H_RELAY(M2H_lexer_dtor(&lexer));
    M2H_RELAY(M2H_stringwriter_dtor(&swriter, NULL));
    M2H_RELAY(M2H_stringreader_dtor(&sreader));
    return M2H_RESULT_OK;
}