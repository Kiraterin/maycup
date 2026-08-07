/**
 * @file main.c
 * @brief CLI entry point
 * @date 2026-07-08
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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// mock def
#include "mock_funcs.h"

const char *usage = "Usage: maycup input_file -o output_file\n";

int main(int argc, char *argv[]) {
    int opt;
    char *input = NULL;
    char *output = NULL;

    opterr = 0;

    while ((opt = getopt(argc, argv, ":h::o:")) != -1) {
        switch (opt) {
        case 'h': {
            printf("%s", usage);
            return EXIT_SUCCESS;
        }
        case 'o': {
            output = optarg;
            break;
        }
        case ':': {
            fprintf(stderr, "Error: expect option -%c\n", optopt);
            goto err_msg;
        }
        case '?': {
            fprintf(stderr, "Error: unknown option -%c\n", optopt);
            goto err_msg;
        }
        }
    }

    if (optind < argc) {
        input = argv[optind];
        if (optind + 1 < argc) {
            fprintf(stderr, "Error: only one input file can be converted\n");
            goto err_msg;
        }
    }

    if (input == NULL) {
        fprintf(stderr, "Error: expect input file\n");
        goto err_msg;
    }

    if (output == NULL) {
        fprintf(stderr, "Error: expect output file\n");
        goto err_msg;
    }

    MAYCUP_UNWRAP(MAYCUP_convert_file(input, output));

    return EXIT_SUCCESS;

err_msg:
    fprintf(stderr, "%s", usage);
    return EXIT_FAILURE;
}