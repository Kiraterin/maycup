/**
 * @file writer.c
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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "md2html/io/writer.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

static M2H_Result filewriter_puts(M2H_IN M2H_FileWriter *self,
                                  M2H_IN char *str) {
    if (fputs(str, self->fp) < 0) {
        return M2H_RESULT_ERRNO;
    }
    return M2H_RESULT_OK;
}

static M2H_Result filewriter_printf(M2H_IN M2H_FileWriter *self,
                                    M2H_IN const char *format, ...) {
    va_list args;
    va_start(args, format);
    if (vfprintf(self->fp, format, args) < 0) {
        return M2H_RESULT_ERRNO;
    }
    va_end(args);
    return M2H_RESULT_OK;
}

M2H_Result M2H_filewriter_ctor(M2H_OUT M2H_FileWriter *self,
                               M2H_IN const char *path) {
    self->fp = fopen(path, "w");
    if (self->fp == NULL) {
        return M2H_RESULT_ERRNO;
    }
    self->base.puts = (M2H_Result (*)(M2H_Writer *, char *))filewriter_puts;
    self->base.printf =
        (M2H_Result (*)(M2H_Writer *, const char *, ...))filewriter_printf;
    return M2H_RESULT_OK;
}

M2H_Result M2H_filewriter_dtor(M2H_OUT M2H_FileWriter *self) {
    if (self->fp == NULL) {
        return M2H_RESULT_ILLEGAL_ARGUMENT;
    }
    if (fclose(self->fp) == EOF) {
        return M2H_RESULT_ERRNO;
    }
    self->fp = NULL;
    return M2H_RESULT_OK;
}

static M2H_Result string_writer_flexible_reserve(M2H_StringWriter *self,
                                                 size_t cap) {
    if (cap == 0) {
        return M2H_RESULT_ILLEGAL_ARGUMENT;
    }
    if (cap <= self->cap) {
        return M2H_RESULT_OK;
    }
    char *new = realloc(self->buf, cap * sizeof(char));
    if (new == NULL) {
        return M2H_RESULT_MALLOC_FAIL;
    }
    self->buf = new;
    self->cap = cap;
    return M2H_RESULT_OK;
}

static M2H_Result stringwriter_puts(M2H_INOUT M2H_StringWriter *self,
                                    M2H_IN char *str) {
    size_t dest_size = self->size + strlen(str);
    if (self->cap < dest_size) {
        if (self->flexible) {
            M2H_RELAY(string_writer_flexible_reserve(self, dest_size * 2));
        } else {
            return M2H_RESULT_ILLEGAL_ARGUMENT;
        }
    }

    strncat(self->buf, str, self->cap - self->size);
    self->size = dest_size;

    return M2H_RESULT_OK;
}

static M2H_Result stringwriter_printf(M2H_IN M2H_StringWriter *self,
                                      M2H_IN const char *format, ...) {
    va_list args;

    va_start(args, format);
    int fmt_size = vsnprintf(NULL, 0, format, args);
    va_end(args);
    if (fmt_size < 0) {
        return M2H_RESULT_ERRNO;
    }

    size_t dest_size = self->size + (size_t)fmt_size;
    if (self->cap < dest_size) {
        if (self->flexible) {
            M2H_RELAY(string_writer_flexible_reserve(self, dest_size * 2));
        } else {
            return M2H_RESULT_ILLEGAL_ARGUMENT;
        }
    }

    va_start(args, format);
    int printf_res = vsnprintf(self->buf + self->size - 1,
                               self->cap - self->size + 1, format, args);
    va_end(args);
    if (printf_res < 0) {
        return M2H_RESULT_ERRNO;
    }
    self->size = dest_size;

    return M2H_RESULT_OK;
}

M2H_Result M2H_stringwriter_ctor(M2H_OUT M2H_StringWriter *self,
                                 M2H_IN char *buf, M2H_IN size_t bufsz) {
    self->buf = buf;
    self->buf[0] = '\0';
    self->size = 1;
    self->cap = bufsz;
    self->flexible = false;
    self->base.puts = (M2H_Result (*)(M2H_Writer *, char *))stringwriter_puts;
    self->base.printf =
        (M2H_Result (*)(M2H_Writer *, const char *, ...))stringwriter_printf;
    return M2H_RESULT_OK;
}

M2H_Result M2H_stringwriter_ctor_flexible(M2H_OUT M2H_StringWriter *self,
                                          M2H_IN size_t bufsz) {
    if (bufsz == 0) {
        return M2H_RESULT_ILLEGAL_ARGUMENT;
    }
    self->size = 1;
    self->cap = bufsz;
    self->buf = (char *)malloc(bufsz * sizeof(char));
    if (self->buf == NULL) {
        return M2H_RESULT_MALLOC_FAIL;
    }
    self->buf[0] = '\0';
    self->flexible = true;
    self->base.puts = (M2H_Result (*)(M2H_Writer *, char *))stringwriter_puts;
    self->base.printf =
        (M2H_Result (*)(M2H_Writer *, const char *, ...))stringwriter_printf;
    return M2H_RESULT_OK;
}

M2H_Result M2H_stringwriter_dtor(M2H_OUT M2H_StringWriter *self,
                                 M2H_OUT char **buf) {
    if (buf == NULL) {
        if (self->flexible) {
            free(self->buf);
        }
        self->buf = NULL;
    } else {
        *buf = self->buf;
    }
    self->buf = NULL;
    self->cap = self->size = 0;
    return M2H_RESULT_OK;
}