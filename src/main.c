/**
 * @file main.c
 * @brief CLI entry point
 * @date 2026-07-08
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

#include "lexer.h"
#include "parser.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc == 2) {
        M2H_Lexer lexer;
        M2H_Token token;
        M2H_UNWRAP(M2H_lexer_ctor(&lexer, argv[1]));
        do {
            M2H_UNWRAP(M2H_next_token(&token, &lexer));
            M2H_print_token(&token);
            M2H_UNWRAP(M2H_token_dtor(&token));
        } while (token.type != M2H_TOKENTYPE_EOF);
        M2H_UNWRAP(M2H_lexer_dtor(&lexer));
    } else {
        M2H_Lexer lexer;
        M2H_Parser parser;
        M2H_UNWRAP(M2H_lexer_ctor(&lexer, ".vscode/test.md"));
        M2H_UNWRAP(M2H_parser_ctor(&parser));
        M2H_UNWRAP(M2H_parse(&parser, &lexer));
        M2H_print_ast(&parser.ast, parser.root_astnode);
        M2H_UNWRAP(M2H_parser_dtor(&parser));
        M2H_UNWRAP(M2H_lexer_dtor(&lexer));
    }

    return 0;
}