/**
 * @file reader.h
 * @brief Reader in IO layer
 * @date 2026-07-18
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

#ifndef READER_H
#define READER_H

#include "md2html/base/result.h"
#include <stdio.h>

typedef struct M2H_Reader M2H_Reader;

struct M2H_Reader {
    M2H_Result (*get_char)(M2H_Reader *self, M2H_OUT char *res);
    M2H_Result (*tell)(M2H_Reader *self, M2H_OUT long *res);
    M2H_Result (*seek)(M2H_Reader *self, M2H_IN long pos, M2H_IN int whence);
};

typedef struct {
    M2H_Reader base;
    FILE *fp;
} M2H_FileReader;

M2H_Result M2H_filereader_ctor(M2H_OUT M2H_FileReader *self, M2H_IN const char *path);
M2H_Result M2H_filereader_dtor(M2H_OUT M2H_FileReader *self);

#endif // READER_H