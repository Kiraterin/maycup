/**
 * @file writer.h
 * @brief Writer in IO layer
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

#ifndef WRITER_H
#define WRITER_H

#include "md2html/base/result.h"
#include <stdio.h>

typedef struct M2H_Writer M2H_Writer;

struct M2H_Writer {
    M2H_Result (*puts)(M2H_Writer *self, M2H_IN char *str);
    M2H_Result (*printf)(M2H_Writer *self, M2H_IN const char *format, ...);
};

typedef struct {
    M2H_Writer base;
    FILE *fp;
} M2H_FileWriter;

/**
 * @brief Construct a file writer
 * @param self Out, the file writer to construct
 * @param path In, the path that the file writer will read
 * @return M2H_Result
 */
M2H_Result M2H_filewriter_ctor(M2H_OUT M2H_FileWriter *self,
                               M2H_IN const char *path);

/**
 * @brief Destruct a file writer
 * @param self Out, the file writer to destruct
 * @return M2H_Result
 */
M2H_Result M2H_filewriter_dtor(M2H_OUT M2H_FileWriter *self);

typedef struct {
    M2H_Writer base;
    char *buf;
    size_t size;
    size_t cap;
    bool flexible;
} M2H_StringWriter;

/**
 * @brief Construct a string writer with a given buffer
 * @param self Out, the string writer to construct
 * @param buf In, the buffer in writer
 * @param bufsz In, the length of buffer
 * @return M2H_Result
 */
M2H_Result M2H_stringwriter_ctor(M2H_OUT M2H_StringWriter *self,
                                 M2H_IN char *buf, M2H_IN size_t bufsz);

#define M2H_DEFAULT_STRWRITER_FLEXBUF_SIZE 128

/**
 * @brief Construct a string writer with an independent flexible buffer
 * @param self Out, the string writer to construct
 * @param bufsz In, the original length of buffer
 * @return M2H_Result
 */
M2H_Result M2H_stringwriter_ctor_flexible(M2H_OUT M2H_StringWriter *self,
                                          M2H_IN size_t bufsz);

/**
 * @brief Destruct a string writer
 * @param self Out, the string writer to destruct
 * @param buf Out, the buffer in writer. The buffer in string writer will be
 *            freed if param @c buf is NULL (if the buffer is flexible).
 *            Remember to free it after use.
 * @return M2H_Result
 */
M2H_Result M2H_stringwriter_dtor(M2H_OUT M2H_StringWriter *self,
                                 M2H_OUT char **buf);

#endif // WRITER_H