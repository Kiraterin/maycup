/**
 * @file writer.h
 * @brief Writer in IO layer
 * @date 2026-07-18
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

#ifndef WRITER_H
#define WRITER_H

#include "maycup/base/result.h"
#include <stdarg.h>
#include <stdio.h>

typedef struct MAYCUP_Writer MAYCUP_Writer;

MAYCUP_Result MAYCUP_writer_puts(void *self, MAYCUP_IN char *str);

MAYCUP_Result MAYCUP_writer_printf(void *self, MAYCUP_IN const char *format, ...);

struct MAYCUP_Writer {
    MAYCUP_Result (*puts)(MAYCUP_Writer *self, MAYCUP_IN char *str);
    MAYCUP_Result (*vprintf)(MAYCUP_Writer *self, MAYCUP_IN const char *format, va_list args);
};

typedef struct {
    MAYCUP_Writer base;
    FILE *fp;
} MAYCUP_FileWriter;

/**
 * @brief Construct a file writer
 * @param self Out, the file writer to construct
 * @param path In, the path that the file writer will read
 * @return MAYCUP_Result
 */
MAYCUP_Result MAYCUP_filewriter_ctor(MAYCUP_OUT MAYCUP_FileWriter *self,
                               MAYCUP_IN const char *path);

/**
 * @brief Destruct a file writer
 * @param self Out, the file writer to destruct
 * @return MAYCUP_Result
 */
MAYCUP_Result MAYCUP_filewriter_dtor(MAYCUP_OUT MAYCUP_FileWriter *self);

typedef struct {
    MAYCUP_Writer base;
    char *buf;
    size_t size;
    size_t cap;
    bool flexible;
} MAYCUP_StringWriter;

/**
 * @brief Construct a string writer with a given buffer
 * @param self Out, the string writer to construct
 * @param buf In, the buffer in writer
 * @param bufsz In, the length of buffer
 * @return MAYCUP_Result
 */
MAYCUP_Result MAYCUP_stringwriter_ctor(MAYCUP_OUT MAYCUP_StringWriter *self,
                                 MAYCUP_IN char *buf, MAYCUP_IN size_t bufsz);

#define MAYCUP_DEFAULT_STRWRITER_FLEXBUF_SIZE 128

/**
 * @brief Construct a string writer with an independent flexible buffer
 * @param self Out, the string writer to construct
 * @param bufsz In, the original length of buffer
 * @return MAYCUP_Result
 */
MAYCUP_Result MAYCUP_stringwriter_ctor_flexible(MAYCUP_OUT MAYCUP_StringWriter *self,
                                          MAYCUP_IN size_t bufsz);

/**
 * @brief Destruct a string writer
 * @param self Out, the string writer to destruct
 * @param buf Out, the buffer in writer. The buffer in string writer will be
 *            freed if param @c buf is NULL (if the buffer is flexible).
 *            Remember to free it after use.
 * @return MAYCUP_Result
 */
MAYCUP_Result MAYCUP_stringwriter_dtor(MAYCUP_OUT MAYCUP_StringWriter *self,
                                 MAYCUP_OUT char **buf);

#endif // WRITER_H