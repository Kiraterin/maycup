/**
 * @file renderer.h
 * @brief Renderer in maycup
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

#ifndef RENDERER_H
#define RENDERER_H

#include "maycup/base/common.h"
#include "maycup/base/result.h"
#include "maycup/core/lexer.h"
#include "maycup/core/parser.h"
#include "maycup/io/writer.h"

typedef struct {
    MAYCUP_Writer *writer;
} MAYCUP_Renderer;

/**
 * @brief Construct a renderer
 * @param self Out, the renderer to construct
 * @param writer In, the writer which the renderer will use
 * @return MAYCUP_Result
 */
MAYCUP_Result maycup_renderer_ctor(MAYCUP_OUT MAYCUP_Renderer *self,
                                   MAYCUP_IN MAYCUP_Writer *writer);

/**
 * @brief Destruct a renderer
 * @param self Out, the renderer to destruct
 * @return MAYCUP_Result
 */
MAYCUP_Result maycup_renderer_dtor(MAYCUP_OUT MAYCUP_Renderer *self);

/**
 * @brief Render HTML file from AST
 * @param renderer In & out, the renderer which do render
 * @param parser In, the parser which provides AST
 * @return MAYCUP_Result
 */
MAYCUP_Result maycup_render(MAYCUP_INOUT MAYCUP_Renderer *renderer,
                            MAYCUP_IN MAYCUP_Parser *parser);

#endif // RENDERER_H