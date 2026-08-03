/**
 * @file reader.c
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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "mock_macros.h"
#include "md2html/io/reader.h"

static M2H_Result filereader_get_char(M2H_IN M2H_FileReader *self, M2H_OUT int *res) {
    int _res = fgetc(self->fp);
    if (_res == EOF && !feof(self->fp)) {
        return M2H_RESULT_ERRNO;
    }
    if (res != NULL) {
        *res = _res;
    }
    return M2H_RESULT_OK;
}

static M2H_Result filereader_tell(M2H_IN M2H_FileReader *self, M2H_OUT long *res) {
    long _res = ftell(self->fp);
    if (_res == -1L) {
        return M2H_RESULT_ERRNO;
    }
    if (res != NULL) {
        *res = _res;
    }
    return M2H_RESULT_OK;
}

static M2H_Result filereader_seek(M2H_IN M2H_FileReader *self, M2H_IN long offset) {
    if (fseek(self->fp, offset, SEEK_SET)) {
        return M2H_RESULT_ERRNO;
    }
    return M2H_RESULT_OK;
}

M2H_Result M2H_filereader_ctor(M2H_OUT M2H_FileReader *self,
                               M2H_IN const char *path) {
    self->fp = fopen(path, "r");
    if (self->fp == NULL) {
        return M2H_RESULT_ERRNO;
    }
    self->base.get_char =
        (M2H_Result (*)(M2H_Reader *, int *))filereader_get_char;
    self->base.tell = (M2H_Result (*)(M2H_Reader *, long *))filereader_tell;
    self->base.seek = (M2H_Result (*)(M2H_Reader *, long))filereader_seek;
    return M2H_RESULT_OK;
}

M2H_Result M2H_filereader_dtor(M2H_OUT M2H_FileReader *self) {
    if (self->fp == NULL) {
        return M2H_RESULT_ILLEGAL_ARGUMENT;
    }
    if (fclose(self->fp) == EOF) {
        return M2H_RESULT_ERRNO;
    }
    self->fp = NULL;
    return M2H_RESULT_OK;
}

static M2H_Result stringreader_get_char(M2H_INOUT M2H_StringReader *self,
                                        M2H_OUT int *res) {
    int _res;
    if (self->iter >= self->end) {
        _res = EOF;
    } else {
        _res = *self->iter;
        ++self->iter;
    }
    if (res != NULL) {
        *res = _res;
    }
    return M2H_RESULT_OK;
}

static M2H_Result stringreader_tell(M2H_IN M2H_StringReader *self, M2H_OUT long *res) {
    long _res = self->iter - self->begin;
    if (res != NULL) {
        *res = _res;
    }
    return M2H_RESULT_OK;
}

static M2H_Result string_reader_seek(M2H_OUT M2H_StringReader *self,
                                     M2H_IN long offset) {
    if (offset < 0) {
        return M2H_RESULT_ILLEGAL_ARGUMENT;
    }
    if (self->begin + offset > self->end) {
        return M2H_RESULT_ILLEGAL_ARGUMENT;
    }
    self->iter = self->begin + offset;
    return M2H_RESULT_OK;
}

M2H_Result M2H_stringreader_ctor(M2H_OUT M2H_StringReader *self,
                                 M2H_IN const char *str,
                                 M2H_IN const size_t len) {
    if (str == NULL) {
        return M2H_RESULT_ILLEGAL_ARGUMENT;
    }
    self->begin = self->iter = str;
    self->end = str + len;
    self->base.get_char =
        (M2H_Result (*)(M2H_Reader *, int *))stringreader_get_char;
    self->base.tell = (M2H_Result (*)(M2H_Reader *, long *))stringreader_tell;
    self->base.seek = (M2H_Result (*)(M2H_Reader *, long))string_reader_seek;
    return M2H_RESULT_OK;
}

M2H_Result M2H_stringreader_dtor(M2H_OUT M2H_StringReader *self) {
    self->begin = self->iter = self->end = NULL;
    return M2H_RESULT_OK;
}