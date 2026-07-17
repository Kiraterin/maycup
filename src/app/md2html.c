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

M2H_Result M2H_convert(M2H_IN const char *input_file,
                       M2H_IN const char *output_file) {
    M2H_Lexer lexer;
    M2H_Parser parser;
    M2H_Renderer renderer;
    M2H_RELAY(M2H_lexer_ctor(&lexer, input_file));
    M2H_RELAY(M2H_parser_ctor(&parser));
    M2H_RELAY(M2H_renderer_ctor(&renderer, output_file));

    M2H_RELAY(M2H_parse(&parser, &lexer));
    M2H_RELAY(M2H_render(&renderer, &parser));

    M2H_RELAY(M2H_renderer_dtor(&renderer));
    M2H_RELAY(M2H_parser_dtor(&parser));
    M2H_RELAY(M2H_lexer_dtor(&lexer));
    return M2H_RESULT_OK;
}