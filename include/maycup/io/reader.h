/**
 * @file reader.h
 * @brief Reader in IO layer
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

#ifndef READER_H
#define READER_H

#include "maycup/base/result.h"
#include <stdio.h>

/**
 * @brief Get a char from reader
 * @note When @p res is @c NULL , the char will be discarded and the cursor
 *       move to next char
 * @param self Pointer to the reader
 * @param res Out, the char got
 */
MAYCUP_Result MAYCUP_reader_get_char(void *self, MAYCUP_OUT int *res);

/**
 * @brief Get the cursor of a reader
 * @param self Pointer to the reader
 * @param res Out, the offset from beginning got, cannot be @c NULL
 */
MAYCUP_Result MAYCUP_reader_tell(void *self, MAYCUP_OUT long *res);

/**
 * @brief Set the cursor of a reader to a given place
 * @param self The reader
 * @param offset In, the destined offset from beginning
 */
MAYCUP_Result MAYCUP_reader_seek(void *self, MAYCUP_IN long offset);

typedef struct MAYCUP_Reader MAYCUP_Reader;

struct MAYCUP_Reader {
    MAYCUP_Result (*get_char)(MAYCUP_Reader *self, MAYCUP_OUT int *res);
    MAYCUP_Result (*tell)(MAYCUP_Reader *self, MAYCUP_OUT long *res);
    MAYCUP_Result (*seek)(MAYCUP_Reader *self, MAYCUP_IN long offset);
};

typedef struct {
    MAYCUP_Reader base;
    FILE *fp;
} MAYCUP_FileReader;

/**
 * @brief Construct a file reader
 * @param self Out, the file reader to construct
 * @param path In, the path that the file reader will read, cannot be @c NULL
 * @return MAYCUP_Result
 */
MAYCUP_Result MAYCUP_filereader_ctor(MAYCUP_OUT MAYCUP_FileReader *self,
                               MAYCUP_IN const char *path);

/**
 * @brief Destruct a file reader
 * @param self Out, the file reader to destruct
 * @return MAYCUP_Result
 */
MAYCUP_Result MAYCUP_filereader_dtor(MAYCUP_OUT MAYCUP_FileReader *self);

typedef struct {
    MAYCUP_Reader base;
    const char *begin;
    const char *cur;
    const char *end;
} MAYCUP_StringReader;

/**
 * @brief Construct a string reader
 * @param self Out, the file reader to construct
 * @param path In, the string that the string reader will read
 * @param len In, the length of the string
 * @return MAYCUP_Result
 */
MAYCUP_Result MAYCUP_stringreader_ctor(MAYCUP_OUT MAYCUP_StringReader *self,
                                 MAYCUP_IN const char *str,
                                 MAYCUP_IN const size_t len);

/**
 * @brief Destruct a string reader
 * @param self Out, the string reader to destruct
 * @return MAYCUP_Result
 */
MAYCUP_Result MAYCUP_stringreader_dtor(MAYCUP_OUT MAYCUP_StringReader *self);

#endif // READER_H