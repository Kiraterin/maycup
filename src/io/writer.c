/**
 * @file writer.c
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

#include "maycup/writer.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

// mock def
#include "mock_funcs.h"

MAYCUP_Result maycup_writer_puts(void *self,  const char *str) {
    if (self == NULL) {
        return MAYCUP_RESULT_ILLEGAL_ARGUMENT;
    }
    return ((MAYCUP_Writer *)self)->puts(self, str);
}
MAYCUP_Result maycup_writer_printf(void *self,  const char *format,
                                   ...) {
    if (self == NULL) {
        return MAYCUP_RESULT_ILLEGAL_ARGUMENT;
    }
    va_list args;
    va_start(args, format);
    MAYCUP_Result res = ((MAYCUP_Writer *)self)->vprintf(self, format, args);
    va_end(args);
    return res;
}

static MAYCUP_Result filewriter_puts( MAYCUP_FileWriter *self,
                                      const char *str) {
    if (self == NULL || str == NULL) {
        return MAYCUP_RESULT_ILLEGAL_ARGUMENT;
    }
    if (fputs(str, self->fp) < 0) {
        return MAYCUP_RESULT_ERRNO;
    }
    return MAYCUP_RESULT_OK;
}

static MAYCUP_Result filewriter_vprintf( MAYCUP_FileWriter *self,
                                         const char *format,
                                        va_list args) {
    if (self == NULL || format == NULL) {
        return MAYCUP_RESULT_ILLEGAL_ARGUMENT;
    }
    if (vfprintf(self->fp, format, args) < 0) {
        return MAYCUP_RESULT_ERRNO;
    }
    return MAYCUP_RESULT_OK;
}

MAYCUP_Result maycup_filewriter_ctor( MAYCUP_FileWriter *self,
                                      const char *path) {
    if (self == NULL) {
        return MAYCUP_RESULT_ILLEGAL_ARGUMENT;
    }
    self->fp = fopen(path, "w");
    if (self->fp == NULL) {
        return MAYCUP_RESULT_ERRNO;
    }
    self->base.puts =
        (MAYCUP_Result (*)(MAYCUP_Writer *, const char *))filewriter_puts;
    self->base.vprintf = (MAYCUP_Result (*)(MAYCUP_Writer *, const char *,
                                            va_list))filewriter_vprintf;
    return MAYCUP_RESULT_OK;
}

MAYCUP_Result maycup_filewriter_dtor( MAYCUP_FileWriter *self) {
    if (self == NULL || self->fp == NULL) {
        return MAYCUP_RESULT_ILLEGAL_ARGUMENT;
    }
    if (fclose(self->fp) == EOF) {
        return MAYCUP_RESULT_ERRNO;
    }
    self->fp = NULL;
    self->base = (MAYCUP_Writer){};
    return MAYCUP_RESULT_OK;
}

static MAYCUP_Result string_writer_flexible_reserve(MAYCUP_StringWriter *self,
                                                    size_t cap) {
    if (self == NULL || cap == 0) {
        return MAYCUP_RESULT_ILLEGAL_ARGUMENT;
    }
    if (cap <= self->cap) {
        return MAYCUP_RESULT_OK;
    }
    char *new = realloc(self->buf, cap * sizeof(char));
    if (new == NULL) {
        return MAYCUP_RESULT_MALLOC_FAIL;
    }
    self->buf = new;
    self->cap = cap;
    return MAYCUP_RESULT_OK;
}

static MAYCUP_Result stringwriter_puts( MAYCUP_StringWriter *self,
                                        const char *str) {
    if (self == NULL || str == NULL) {
        return MAYCUP_RESULT_ILLEGAL_ARGUMENT;
    }
    size_t dest_size = self->size + strlen(str);
    if (self->cap < dest_size) {
        if (self->flexible) {
            MAYCUP_RELAY(string_writer_flexible_reserve(self, dest_size * 2));
        } else {
            return MAYCUP_RESULT_ILLEGAL_ARGUMENT;
        }
    }

    strncat(self->buf, str, self->cap - self->size);
    self->size = dest_size;

    return MAYCUP_RESULT_OK;
}

static MAYCUP_Result stringwriter_vprintf( MAYCUP_StringWriter *self,
                                           const char *format,
                                          va_list args) {
    if (self == NULL || format == NULL) {
        return MAYCUP_RESULT_ILLEGAL_ARGUMENT;
    }

    va_list args_fmt;
    va_copy(args_fmt, args);

    int fmt_size = vsnprintf(NULL, 0, format, args_fmt);
    va_end(args_fmt);
    if (fmt_size < 0) {
        return MAYCUP_RESULT_ERRNO;
    }

    size_t dest_size = self->size + (size_t)fmt_size;
    if (self->cap < dest_size) {
        if (self->flexible) {
            MAYCUP_RELAY(string_writer_flexible_reserve(self, dest_size * 2));
        } else {
            return MAYCUP_RESULT_ILLEGAL_ARGUMENT;
        }
    }
    int printf_res = vsnprintf(self->buf + self->size - 1,
                               self->cap - self->size + 1, format, args);
    if (printf_res < 0) {
        return MAYCUP_RESULT_ERRNO;
    }
    self->size = dest_size;

    return MAYCUP_RESULT_OK;
}

MAYCUP_Result maycup_stringwriter_ctor( MAYCUP_StringWriter *self,
                                        char *buf,
                                        size_t bufsz) {
    if (self == NULL || buf == NULL || bufsz == 0) {
        return MAYCUP_RESULT_ILLEGAL_ARGUMENT;
    }
    self->buf = buf;
    self->buf[0] = '\0';
    self->size = 1;
    self->cap = bufsz;
    self->flexible = false;
    self->base.puts =
        (MAYCUP_Result (*)(MAYCUP_Writer *, const char *))stringwriter_puts;
    self->base.vprintf = (MAYCUP_Result (*)(MAYCUP_Writer *, const char *,
                                            va_list))stringwriter_vprintf;
    return MAYCUP_RESULT_OK;
}

MAYCUP_Result
maycup_stringwriter_ctor_flexible( MAYCUP_StringWriter *self,
                                   size_t bufsz) {
    if (self == NULL || bufsz == 0) {
        return MAYCUP_RESULT_ILLEGAL_ARGUMENT;
    }
    self->size = 1;
    self->cap = bufsz;
    self->buf = (char *)malloc(bufsz * sizeof(char));
    if (self->buf == NULL) {
        return MAYCUP_RESULT_MALLOC_FAIL;
    }
    self->buf[0] = '\0';
    self->flexible = true;
    self->base.puts =
        (MAYCUP_Result (*)(MAYCUP_Writer *, const char *))stringwriter_puts;
    self->base.vprintf = (MAYCUP_Result (*)(MAYCUP_Writer *, const char *,
                                            va_list))stringwriter_vprintf;
    return MAYCUP_RESULT_OK;
}

MAYCUP_Result maycup_stringwriter_dtor( MAYCUP_StringWriter *self,
                                        char **buf) {
    if (self == NULL || self->buf == NULL) {
        return MAYCUP_RESULT_ILLEGAL_ARGUMENT;
    }
    if (buf == NULL) {
        if (self->flexible) {
            free(self->buf);
        }
        self->buf = NULL;
    } else {
        *buf = self->buf;
    }
    self->base = (MAYCUP_Writer){};
    self->buf = NULL;
    self->cap = self->size = 0;
    return MAYCUP_RESULT_OK;
}