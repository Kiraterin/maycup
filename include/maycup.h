/**
 * @file maycup.h
 * @brief A lightweight tool library converts markdown to html
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

#ifndef MAYCUP_H
#define MAYCUP_H

#include "maycup/result.h"
#include <stddef.h>

/**
 * @brief Convert a Markdown file into an HTML file
 * @param input_file In, path to input file
 * @param output_file In, path to output file
 * @return MAYCUP_Result
 */
MAYCUP_Result maycup_convert_file(const char *input_file,
                                  const char *output_file);

/**
 * @brief Convert a Markdown string into an HTML string
 * @param input_string In, the input string
 * @param input_len In, length of input string
 * @param output_string In, the output string buffer
 * @param output_len In, the max length of output
 * @return MAYCUP_Result
 */
MAYCUP_Result maycup_convert_string(const char *input_string,
                                    const size_t input_len, char *output_string,
                                    const size_t output_len);

#endif // MAYCUP_H