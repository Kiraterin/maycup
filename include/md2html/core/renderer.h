/**
 * @file renderer.h
 * @brief Renderer in md2html
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

#ifndef RENDERER_H
#define RENDERER_H

#include "md2html/base/common.h"
#include "md2html/core/lexer.h"
#include "md2html/core/parser.h"
#include "md2html/base/result.h"

typedef struct {
    char *output_file_path;
    FILE *fp;
} M2H_Renderer;

/**
 * @brief Construct a renderer
 * @param self Out, the renderer to construct
 * @param path In, the path to the output file
 * @return M2H_Result
 */
M2H_Result M2H_renderer_ctor(M2H_OUT M2H_Renderer *self,
                             M2H_IN const char *path);

/**
 * @brief Destruct a renderer
 * @param self Out, the renderer to destruct
 * @return M2H_Result
 */
M2H_Result M2H_renderer_dtor(M2H_OUT M2H_Renderer *self);

/**
 * @brief Render HTML file from AST
 * @param renderer In & out, the renderer which do render
 * @param parser In, the parser which provides AST
 * @return M2H_Result
 */
M2H_Result M2H_render(M2H_INOUT M2H_Renderer *renderer,
                      M2H_IN M2H_Parser *parser);

#endif // RENDERER_H