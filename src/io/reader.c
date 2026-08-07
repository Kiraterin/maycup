/**
 * @file reader.c
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

#include "maycup/io/reader.h"
#include <string.h>

// mock def
#include "mock_funcs.h"

MAYCUP_Result maycup_reader_get_char(void *self, MAYCUP_OUT int *res) {
    if (self == NULL) {
        return MAYCUP_RESULT_ILLEGAL_ARGUMENT;
    }
    return ((MAYCUP_Reader *)self)->get_char(self, res);
}

MAYCUP_Result maycup_reader_tell(void *self, MAYCUP_OUT long *res) {
    if (self == NULL) {
        return MAYCUP_RESULT_ILLEGAL_ARGUMENT;
    }
    return ((MAYCUP_Reader *)self)->tell(self, res);
}

MAYCUP_Result maycup_reader_seek(void *self, MAYCUP_IN long offset) {
    if (self == NULL) {
        return MAYCUP_RESULT_ILLEGAL_ARGUMENT;
    }
    return ((MAYCUP_Reader *)self)->seek(self, offset);
}

static MAYCUP_Result filereader_get_char(MAYCUP_IN MAYCUP_FileReader *self,
                                         MAYCUP_OUT int *res) {
    if (self == NULL) {
        return MAYCUP_RESULT_ILLEGAL_ARGUMENT;
    }
    int _res = fgetc(self->fp);
    if (_res == EOF && !feof(self->fp)) {
        return MAYCUP_RESULT_ERRNO;
    }
    if (res != NULL) {
        *res = _res;
    }
    return MAYCUP_RESULT_OK;
}

static MAYCUP_Result filereader_tell(MAYCUP_IN MAYCUP_FileReader *self,
                                     MAYCUP_OUT long *res) {
    if (self == NULL || res == NULL) {
        return MAYCUP_RESULT_ILLEGAL_ARGUMENT;
    }
    *res = ftell(self->fp);
    if (*res == -1L) {
        return MAYCUP_RESULT_ERRNO;
    }
    return MAYCUP_RESULT_OK;
}

static MAYCUP_Result filereader_seek(MAYCUP_IN MAYCUP_FileReader *self,
                                     MAYCUP_IN long offset) {
    if (self == NULL) {
        return MAYCUP_RESULT_ILLEGAL_ARGUMENT;
    }
    if (fseek(self->fp, offset, SEEK_SET)) {
        return MAYCUP_RESULT_ERRNO;
    }
    return MAYCUP_RESULT_OK;
}

MAYCUP_Result maycup_filereader_ctor(MAYCUP_OUT MAYCUP_FileReader *self,
                                     MAYCUP_IN const char *path) {
    if (self == NULL || path == NULL) {
        return MAYCUP_RESULT_ILLEGAL_ARGUMENT;
    }
    self->fp = fopen(path, "r");
    if (self->fp == NULL) {
        return MAYCUP_RESULT_ERRNO;
    }
    self->base.get_char =
        (MAYCUP_Result (*)(MAYCUP_Reader *, int *))filereader_get_char;
    self->base.tell =
        (MAYCUP_Result (*)(MAYCUP_Reader *, long *))filereader_tell;
    self->base.seek = (MAYCUP_Result (*)(MAYCUP_Reader *, long))filereader_seek;
    return MAYCUP_RESULT_OK;
}

MAYCUP_Result maycup_filereader_dtor(MAYCUP_OUT MAYCUP_FileReader *self) {
    if (self == NULL || self->fp == NULL) {
        return MAYCUP_RESULT_ILLEGAL_ARGUMENT;
    }
    if (fclose(self->fp) == EOF) {
        return MAYCUP_RESULT_ERRNO;
    }
    self->fp = NULL;
    self->base = (MAYCUP_Reader){};
    return MAYCUP_RESULT_OK;
}

static MAYCUP_Result
stringreader_get_char(MAYCUP_INOUT MAYCUP_StringReader *self,
                      MAYCUP_OUT int *res) {
    if (self == NULL) {
        return MAYCUP_RESULT_ILLEGAL_ARGUMENT;
    }
    int _res;
    if (self->cur >= self->end) {
        _res = EOF;
    } else {
        _res = *self->cur;
        ++self->cur;
    }
    if (res != NULL) {
        *res = _res;
    }
    return MAYCUP_RESULT_OK;
}

static MAYCUP_Result stringreader_tell(MAYCUP_IN MAYCUP_StringReader *self,
                                       MAYCUP_OUT long *res) {
    if (self == NULL || res == NULL) {
        return MAYCUP_RESULT_ILLEGAL_ARGUMENT;
    }
    *res = self->cur - self->begin;
    return MAYCUP_RESULT_OK;
}

static MAYCUP_Result string_reader_seek(MAYCUP_OUT MAYCUP_StringReader *self,
                                        MAYCUP_IN long offset) {
    if (self == NULL) {
        return MAYCUP_RESULT_ILLEGAL_ARGUMENT;
    }
    if (offset < 0) {
        return MAYCUP_RESULT_ILLEGAL_ARGUMENT;
    }
    if (self->begin + offset > self->end) {
        return MAYCUP_RESULT_ILLEGAL_ARGUMENT;
    }
    self->cur = self->begin + offset;
    return MAYCUP_RESULT_OK;
}

MAYCUP_Result maycup_stringreader_ctor(MAYCUP_OUT MAYCUP_StringReader *self,
                                       MAYCUP_IN const char *str,
                                       MAYCUP_IN const size_t len) {
    if (self == NULL || str == NULL || strlen(str) < len) {
        return MAYCUP_RESULT_ILLEGAL_ARGUMENT;
    }
    self->begin = self->cur = str;
    self->end = str + len;
    self->base.get_char =
        (MAYCUP_Result (*)(MAYCUP_Reader *, int *))stringreader_get_char;
    self->base.tell =
        (MAYCUP_Result (*)(MAYCUP_Reader *, long *))stringreader_tell;
    self->base.seek =
        (MAYCUP_Result (*)(MAYCUP_Reader *, long))string_reader_seek;
    return MAYCUP_RESULT_OK;
}

MAYCUP_Result maycup_stringreader_dtor(MAYCUP_OUT MAYCUP_StringReader *self) {
    if (self == NULL || self->begin == NULL || self->cur == NULL ||
        self->end == NULL) {
        return MAYCUP_RESULT_ILLEGAL_ARGUMENT;
    }
    self->base = (MAYCUP_Reader){};
    self->begin = self->cur = self->end = NULL;
    return MAYCUP_RESULT_OK;
}