/**
 * @file writer.c
 * @brief Test for writer
 * @date 2026-08-09
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

#include "maycup/io/writer.h"
#include "test.h"
#include <stdio.h>
#include <string.h>

#include "fixture/input/writer_str_puts_test.h"

static const char *const runtime_wfile = "./build/test/tmp/filewriter.txt";

#define OUTPUT_STR_SIZE 32768
char output_str[OUTPUT_STR_SIZE];

TEST_CASE(filewriter_ctor_normal) {
    MAYCUP_FileWriter fw;
    ASSERT_OK(maycup_filewriter_ctor(&fw, runtime_wfile), fail);
    ASSERT_NEQ(fw.base.puts, NULL, fail);
    ASSERT_NEQ(fw.base.vprintf, NULL, fail);
    ASSERT_NEQ(fw.fp, NULL, fail);

    ASSERT_OK(maycup_filewriter_dtor(&fw), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_filewriter_dtor(&fw);
    return TEST_RESULT_FAIL;
}

TEST_CASE(filewriter_ctor_fopen_fail) {
    MAYCUP_FileWriter fw;

    MOCK_ON(fopen);

    ASSERT_EQ(maycup_filewriter_ctor(&fw, runtime_wfile), MAYCUP_RESULT_ERRNO,
              fail);

    MOCK_OFF(fopen);
    return TEST_RESULT_PASS;
fail:
    MOCK_OFF(fopen);
    return TEST_RESULT_FAIL;
}

TEST_CASE(filewriter_ctor_illegal_arg) {
    ASSERT_EQ(maycup_filewriter_ctor(NULL, runtime_wfile),
              MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);
    return TEST_RESULT_PASS;
fail:
    return TEST_RESULT_FAIL;
}

TEST_CASE(filewriter_dtor_normal) {
    MAYCUP_FileWriter fw;
    ASSERT_OK(maycup_filewriter_ctor(&fw, runtime_wfile), fail);
    ASSERT_OK(maycup_filewriter_dtor(&fw), fail);
    ASSERT_EQ(fw.base.puts, NULL, fail);
    ASSERT_EQ(fw.base.vprintf, NULL, fail);
    ASSERT_EQ(fw.fp, NULL, fail);
    return TEST_RESULT_PASS;
fail:
    maycup_filewriter_dtor(&fw);
    return TEST_RESULT_FAIL;
}

TEST_CASE(filewriter_dtor_double) {
    MAYCUP_FileWriter fw;
    ASSERT_OK(maycup_filewriter_ctor(&fw, runtime_wfile), fail);
    ASSERT_OK(maycup_filewriter_dtor(&fw), fail);
    ASSERT_EQ(maycup_filewriter_dtor(&fw), MAYCUP_RESULT_ILLEGAL_ARGUMENT,
              fail);
    return TEST_RESULT_PASS;
fail:
    maycup_filewriter_dtor(&fw);
    return TEST_RESULT_FAIL;
}

TEST_CASE(filewriter_dtor_fclose_fail) {
    MAYCUP_FileWriter fw;

    MOCK_ON(fclose);

    ASSERT_OK(maycup_filewriter_ctor(&fw, runtime_wfile), fail);
    ASSERT_EQ(maycup_filewriter_dtor(&fw), MAYCUP_RESULT_ERRNO, fail);

    MOCK_OFF(fclose);
    ASSERT_OK(maycup_filewriter_dtor(&fw), fail);
    return TEST_RESULT_PASS;
fail:
    MOCK_OFF(fclose);
    maycup_filewriter_dtor(&fw);
    return TEST_RESULT_FAIL;
}

TEST_CASE(filewriter_dtor_illegal_arg) {
    MAYCUP_FileWriter fw;

    ASSERT_OK(maycup_filewriter_ctor(&fw, runtime_wfile), fail);
    ASSERT_EQ(maycup_filewriter_dtor(NULL), MAYCUP_RESULT_ILLEGAL_ARGUMENT,
              fail);

    ASSERT_OK(maycup_filewriter_dtor(&fw), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_filewriter_dtor(&fw);
    return TEST_RESULT_FAIL;
}

TEST_CASE(filewriter_puts_normal) {
    MAYCUP_FileWriter fw;
    FILE *fp;
    char buf[1024];
    long sz;

    ASSERT_OK(maycup_filewriter_ctor(&fw, runtime_wfile), fail);
    ASSERT_OK(maycup_writer_puts(&fw, writer_str_puts_test), fail);
    ASSERT_OK(maycup_filewriter_dtor(&fw), fail);
    fp = fopen(runtime_wfile, "r");
    fseek(fp, 0, SEEK_END);
    sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    fread(buf, sizeof(char), sz, fp);
    ASSERT_EQ(strcmp(buf, writer_str_puts_test), 0, fail);

    ASSERT_OK(maycup_filewriter_ctor(&fw, runtime_wfile), fail);
    ASSERT_OK(maycup_writer_puts(&fw, "123 \n"), fail);
    ASSERT_OK(maycup_writer_puts(&fw, "456"), fail);
    ASSERT_OK(maycup_writer_puts(&fw, "\t789"), fail);
    ASSERT_OK(maycup_filewriter_dtor(&fw), fail);
    fp = fopen(runtime_wfile, "r");
    fseek(fp, 0, SEEK_END);
    sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    memset(buf, 0, 1024);
    fread(buf, sizeof(char), sz, fp);
    ASSERT_EQ(strcmp(buf, "123 \n456\t789"), 0, fail);

    return TEST_RESULT_PASS;
fail:
    if (fw.fp != NULL) {
        maycup_filewriter_dtor(&fw);
    }
    return TEST_RESULT_FAIL;
}

TEST_CASE(filewriter_puts_fputs_fail) {
    MAYCUP_FileWriter fw;

    MOCK_ON(fputs);

    ASSERT_OK(maycup_filewriter_ctor(&fw, runtime_wfile), fail);
    ASSERT_EQ(maycup_writer_puts(&fw, writer_str_puts_test),
              MAYCUP_RESULT_ERRNO, fail);

    MOCK_OFF(fputs);
    ASSERT_OK(maycup_filewriter_dtor(&fw), fail);
    return TEST_RESULT_PASS;
fail:
    MOCK_OFF(fputs);
    maycup_filewriter_dtor(&fw);
    return TEST_RESULT_FAIL;
}

TEST_CASE(filewriter_puts_illegal_arg) {
    MAYCUP_FileWriter fw;

    ASSERT_OK(maycup_filewriter_ctor(&fw, runtime_wfile), fail);
    ASSERT_EQ(maycup_writer_puts(NULL, writer_str_puts_test),
              MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);
    ASSERT_EQ(maycup_writer_puts(&fw, NULL), MAYCUP_RESULT_ILLEGAL_ARGUMENT,
              fail);
    ASSERT_EQ(fw.base.puts(NULL, writer_str_puts_test),
              MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);
    ASSERT_EQ(fw.base.puts((MAYCUP_Writer *)&fw, NULL),
              MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);

    ASSERT_OK(maycup_filewriter_dtor(&fw), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_filewriter_dtor(&fw);
    return TEST_RESULT_FAIL;
}

TEST_CASE(filewriter_printf_normal) {
    MAYCUP_FileWriter fw;
    FILE *fp;
    char buf[1024];
    long sz;

    ASSERT_OK(maycup_filewriter_ctor(&fw, runtime_wfile), fail);
    ASSERT_OK(maycup_writer_printf(&fw, "123 %d", 10), fail);
    ASSERT_OK(maycup_filewriter_dtor(&fw), fail);
    fp = fopen(runtime_wfile, "r");
    fseek(fp, 0, SEEK_END);
    sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    fread(buf, sizeof(char), sz, fp);
    ASSERT_EQ(strcmp(buf, "123 10"), 0, fail);

    return TEST_RESULT_PASS;
fail:
    if (fw.fp != NULL) {
        maycup_filewriter_dtor(&fw);
    }
    return TEST_RESULT_FAIL;
}

TEST_CASE(filewriter_printf_vfprintf_fail) {
    MAYCUP_FileWriter fw;

    MOCK_ON(vfprintf);

    ASSERT_OK(maycup_filewriter_ctor(&fw, runtime_wfile), fail);
    ASSERT_EQ(maycup_writer_printf(&fw, ""), MAYCUP_RESULT_ERRNO, fail);

    MOCK_OFF(vfprintf);
    ASSERT_OK(maycup_filewriter_dtor(&fw), fail);
    return TEST_RESULT_PASS;
fail:
    MOCK_OFF(vfprintf);
    maycup_filewriter_dtor(&fw);
    return TEST_RESULT_FAIL;
}

TEST_CASE(filewriter_printf_illegal_arg) {
    MAYCUP_FileWriter fw;

    ASSERT_OK(maycup_filewriter_ctor(&fw, runtime_wfile), fail);
    ASSERT_EQ(maycup_writer_printf(NULL, ""), MAYCUP_RESULT_ILLEGAL_ARGUMENT,
              fail);
    ASSERT_EQ(maycup_writer_printf(&fw, NULL), MAYCUP_RESULT_ILLEGAL_ARGUMENT,
              fail);
    ASSERT_EQ(fw.base.puts(NULL, ""), MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);
    ASSERT_EQ(fw.base.puts((MAYCUP_Writer *)&fw, NULL),
              MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);

    ASSERT_OK(maycup_filewriter_dtor(&fw), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_filewriter_dtor(&fw);
    return TEST_RESULT_FAIL;
}

TEST_CASE(stringwriter_ctor_normal) {
    MAYCUP_StringWriter sw;
    ASSERT_OK(maycup_stringwriter_ctor(&sw, output_str, OUTPUT_STR_SIZE), fail);
    ASSERT_NEQ(sw.base.puts, NULL, fail);
    ASSERT_NEQ(sw.base.vprintf, NULL, fail);
    ASSERT_EQ(sw.flexible, false, fail);
    ASSERT_EQ(sw.buf, output_str, fail);
    ASSERT_EQ(sw.cap, OUTPUT_STR_SIZE, fail);
    ASSERT_EQ(sw.size, 1, fail);

    ASSERT_OK(maycup_stringwriter_dtor(&sw, NULL), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_stringwriter_dtor(&sw, NULL);
    return TEST_RESULT_FAIL;
}

TEST_CASE(stringwriter_ctor_illegal_arg) {
    MAYCUP_StringWriter sw;

    ASSERT_EQ(maycup_stringwriter_ctor(NULL, output_str, OUTPUT_STR_SIZE),
              MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);
    ASSERT_EQ(maycup_stringwriter_ctor(&sw, NULL, OUTPUT_STR_SIZE),
              MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);
    ASSERT_EQ(maycup_stringwriter_ctor(&sw, output_str, 0),
              MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);

    return TEST_RESULT_PASS;
fail:
    return TEST_RESULT_FAIL;
}

TEST_CASE(stringwriter_dtor_normal_nores) {
    MAYCUP_StringWriter sw;

    ASSERT_OK(maycup_stringwriter_ctor(&sw, output_str, OUTPUT_STR_SIZE), fail);
    ASSERT_OK(maycup_stringwriter_dtor(&sw, NULL), fail);
    ASSERT_EQ(sw.base.puts, NULL, fail);
    ASSERT_EQ(sw.base.vprintf, NULL, fail);
    ASSERT_EQ(sw.buf, NULL, fail);
    ASSERT_EQ(sw.cap, 0, fail);
    ASSERT_EQ(sw.size, 0, fail);

    return TEST_RESULT_PASS;
fail:
    maycup_stringwriter_dtor(&sw, NULL);
    return TEST_RESULT_FAIL;
}

TEST_CASE(stringwriter_dtor_normal_res) {
    MAYCUP_StringWriter sw;

    ASSERT_OK(maycup_stringwriter_ctor(&sw, output_str, OUTPUT_STR_SIZE), fail);
    char *res;
    ASSERT_OK(maycup_stringwriter_dtor(&sw, &res), fail);
    ASSERT_EQ(sw.base.puts, NULL, fail);
    ASSERT_EQ(sw.base.vprintf, NULL, fail);
    ASSERT_EQ(sw.buf, NULL, fail);
    ASSERT_EQ(sw.cap, 0, fail);
    ASSERT_EQ(sw.size, 0, fail);
    ASSERT_EQ(res, output_str, fail);

    return TEST_RESULT_PASS;
fail:
    maycup_stringwriter_dtor(&sw, NULL);
    return TEST_RESULT_FAIL;
}

TEST_CASE(stringwriter_dtor_double) {
    MAYCUP_StringWriter sw;
    ASSERT_OK(maycup_stringwriter_ctor(&sw, output_str, OUTPUT_STR_SIZE), fail);
    ASSERT_OK(maycup_stringwriter_dtor(&sw, NULL), fail);
    ASSERT_EQ(maycup_stringwriter_dtor(&sw, NULL),
              MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);
    return TEST_RESULT_PASS;
fail:
    if (sw.buf != NULL) {
        maycup_stringwriter_dtor(&sw, NULL);
    }
    return TEST_RESULT_FAIL;
}

TEST_CASE(stringwriter_dtor_illegal_arg) {
    MAYCUP_StringWriter sw;

    ASSERT_OK(maycup_stringwriter_ctor(&sw, output_str, OUTPUT_STR_SIZE), fail);
    ASSERT_EQ(maycup_stringwriter_dtor(NULL, NULL),
              MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);

    ASSERT_OK(maycup_stringwriter_dtor(&sw, NULL), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_stringwriter_dtor(&sw, NULL);
    return TEST_RESULT_FAIL;
}

TEST_CASE(stringwriter_puts_normal) {
    MAYCUP_StringWriter sw;

    ASSERT_OK(maycup_stringwriter_ctor(&sw, output_str, OUTPUT_STR_SIZE), fail);
    ASSERT_OK(maycup_writer_puts(&sw, writer_str_puts_test), fail);
    ASSERT_OK(maycup_stringwriter_dtor(&sw, NULL), fail);
    ASSERT_EQ(strcmp(output_str, writer_str_puts_test), 0, fail);

    ASSERT_OK(maycup_stringwriter_ctor(&sw, output_str, OUTPUT_STR_SIZE), fail);
    ASSERT_OK(maycup_writer_puts(&sw, "123 \n"), fail);
    ASSERT_OK(maycup_writer_puts(&sw, "456"), fail);
    ASSERT_OK(maycup_writer_puts(&sw, "\t789"), fail);
    ASSERT_OK(maycup_stringwriter_dtor(&sw, NULL), fail);
    ASSERT_EQ(strcmp(output_str, "123 \n456\t789"), 0, fail);

    return TEST_RESULT_PASS;
fail:
    maycup_stringwriter_dtor(&sw, NULL);
    return TEST_RESULT_FAIL;
}

TEST_CASE(stringwriter_puts_short) {
    MAYCUP_StringWriter sw;

    ASSERT_OK(maycup_stringwriter_ctor(&sw, output_str, 1), fail);
    ASSERT_EQ(maycup_writer_puts(&sw, "123"), MAYCUP_RESULT_ILLEGAL_ARGUMENT,
              fail);
    ASSERT_EQ(maycup_writer_puts(&sw, "1"), MAYCUP_RESULT_ILLEGAL_ARGUMENT,
              fail);

    return TEST_RESULT_PASS;
fail:
    maycup_stringwriter_dtor(&sw, NULL);
    return TEST_RESULT_FAIL;
}

TEST_CASE(stringwriter_puts_length_exceeded) {
    MAYCUP_StringWriter sw;

    ASSERT_OK(maycup_stringwriter_ctor(&sw, output_str, 1), fail);
    while (sw.size < sw.cap) {
        ASSERT_OK(maycup_writer_puts(&sw, "1"), fail);
    }
    ASSERT_EQ(maycup_writer_puts(&sw, "1"), MAYCUP_RESULT_ILLEGAL_ARGUMENT,
              fail);

    return TEST_RESULT_PASS;
fail:
    maycup_stringwriter_dtor(&sw, NULL);
    return TEST_RESULT_FAIL;
}

TEST_CASE(stringwriter_puts_illegal_arg) {
    MAYCUP_StringWriter sw;

    ASSERT_OK(maycup_stringwriter_ctor(&sw, output_str, OUTPUT_STR_SIZE), fail);
    ASSERT_EQ(maycup_writer_puts(NULL, writer_str_puts_test),
              MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);
    ASSERT_EQ(maycup_writer_puts(&sw, NULL), MAYCUP_RESULT_ILLEGAL_ARGUMENT,
              fail);
    ASSERT_EQ(sw.base.puts(NULL, writer_str_puts_test),
              MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);
    ASSERT_EQ(sw.base.puts((MAYCUP_Writer *)&sw, NULL),
              MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);

    ASSERT_OK(maycup_stringwriter_dtor(&sw, NULL), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_stringwriter_dtor(&sw, NULL);
    return TEST_RESULT_FAIL;
}

TEST_CASE(stringwriter_printf_normal) {
    MAYCUP_StringWriter sw;

    ASSERT_OK(maycup_stringwriter_ctor(&sw, output_str, OUTPUT_STR_SIZE), fail);
    ASSERT_OK(maycup_writer_printf(&sw, "123 %d", 10), fail);
    ASSERT_OK(maycup_writer_printf(&sw, "123 %s", "456"), fail);
    ASSERT_OK(maycup_writer_printf(&sw, "%s\nabcd", "789"), fail);
    ASSERT_OK(maycup_stringwriter_dtor(&sw, NULL), fail);
    ASSERT_EQ(strcmp(output_str, "123 10123 456789\nabcd"), 0, fail);

    return TEST_RESULT_PASS;
fail:
    maycup_stringwriter_dtor(&sw, NULL);
    return TEST_RESULT_FAIL;
}

TEST_CASE(stringwriter_printf_length_exceeded) {
    MAYCUP_StringWriter sw;

    ASSERT_OK(maycup_stringwriter_ctor(&sw, output_str, 1), fail);
    while (sw.size < sw.cap) {
        ASSERT_OK(maycup_writer_printf(&sw, "%c", 'a'), fail);
    }
    ASSERT_EQ(maycup_writer_printf(&sw, "%c", 'a'),
              MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);

    return TEST_RESULT_PASS;
fail:
    maycup_stringwriter_dtor(&sw, NULL);
    return TEST_RESULT_FAIL;
}

TEST_CASE(stringwriter_printf_vsnprintf_fail) {
    MAYCUP_StringWriter sw;

    MOCK_ON(vsnprintf);

    ASSERT_OK(maycup_stringwriter_ctor(&sw, output_str, OUTPUT_STR_SIZE), fail);
    ASSERT_EQ(maycup_writer_printf(&sw, ""), MAYCUP_RESULT_ERRNO, fail);

    MOCK_OFF(vsnprintf);
    ASSERT_OK(maycup_stringwriter_dtor(&sw, NULL), fail);
    return TEST_RESULT_PASS;
fail:
    MOCK_OFF(vsnprintf);
    maycup_stringwriter_dtor(&sw, NULL);
    return TEST_RESULT_FAIL;
}

TEST_CASE(stringwriter_printf_illegal_arg) {
    MAYCUP_StringWriter sw;

    ASSERT_OK(maycup_stringwriter_ctor(&sw, output_str, OUTPUT_STR_SIZE), fail);
    ASSERT_EQ(maycup_writer_printf(NULL, ""), MAYCUP_RESULT_ILLEGAL_ARGUMENT,
              fail);
    ASSERT_EQ(maycup_writer_printf(&sw, NULL), MAYCUP_RESULT_ILLEGAL_ARGUMENT,
              fail);
    ASSERT_EQ(sw.base.puts(NULL, ""), MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);
    ASSERT_EQ(sw.base.puts((MAYCUP_Writer *)&sw, NULL),
              MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);

    ASSERT_OK(maycup_stringwriter_dtor(&sw, NULL), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_stringwriter_dtor(&sw, NULL);
    return TEST_RESULT_FAIL;
}

TEST_CASE(stringwriter_flexible_ctor_normal) {
    MAYCUP_StringWriter sw;
    const size_t bufsz = 128;

    ASSERT_OK(maycup_stringwriter_ctor_flexible(&sw, bufsz), fail);
    ASSERT_NEQ(sw.base.puts, NULL, fail);
    ASSERT_NEQ(sw.base.vprintf, NULL, fail);
    ASSERT_EQ(sw.flexible, true, fail);
    ASSERT_NEQ(sw.buf, NULL, fail);
    ASSERT_EQ(sw.cap, bufsz, fail);
    ASSERT_EQ(sw.size, 1, fail);

    ASSERT_OK(maycup_stringwriter_dtor(&sw, NULL), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_stringwriter_dtor(&sw, NULL);
    return TEST_RESULT_FAIL;
}

TEST_CASE(stringwriter_flexible_ctor_malloc_fail) {
    MAYCUP_StringWriter sw;
    const size_t bufsz = 128;

    MOCK_ON(malloc);

    ASSERT_EQ(maycup_stringwriter_ctor_flexible(&sw, bufsz),
              MAYCUP_RESULT_MALLOC_FAIL, fail);

    MOCK_OFF(malloc);
    return TEST_RESULT_PASS;
fail:
    MOCK_OFF(malloc);
    return TEST_RESULT_FAIL;
}

TEST_CASE(stringwriter_flexible_ctor_illegal_arg) {
    MAYCUP_StringWriter sw;
    const size_t bufsz = 128;

    ASSERT_EQ(maycup_stringwriter_ctor_flexible(NULL, bufsz),
              MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);
    ASSERT_EQ(maycup_stringwriter_ctor_flexible(&sw, 0),
              MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);

    return TEST_RESULT_PASS;
fail:
    return TEST_RESULT_FAIL;
}

TEST_CASE(stringwriter_flexible_puts_normal) {
    MAYCUP_StringWriter sw;
    const size_t bufsz = 8;

    ASSERT_OK(maycup_stringwriter_ctor_flexible(&sw, bufsz), fail);
    for (int i = 1; i <= 3; ++i) {
        ASSERT_OK(maycup_writer_puts(&sw, "12345678900987654321"), fail);
    }

    ASSERT_EQ(
        strcmp(sw.buf,
               "123456789009876543211234567890098765432112345678900987654321"),
        0, fail);

    ASSERT_OK(maycup_stringwriter_dtor(&sw, NULL), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_stringwriter_dtor(&sw, NULL);
    return TEST_RESULT_FAIL;
}

TEST_CASE(stringwriter_flexible_puts_realloc_fail) {
    MAYCUP_StringWriter sw;
    const size_t bufsz = 8;

    MOCK_ON(realloc);

    ASSERT_OK(maycup_stringwriter_ctor_flexible(&sw, bufsz), fail);
    ASSERT_EQ(maycup_writer_puts(&sw, "12345678900987654321"),
              MAYCUP_RESULT_MALLOC_FAIL, fail);

    MOCK_OFF(realloc);
    ASSERT_OK(maycup_stringwriter_dtor(&sw, NULL), fail);
    return TEST_RESULT_PASS;
fail:
    MOCK_OFF(realloc);
    maycup_stringwriter_dtor(&sw, NULL);
    return TEST_RESULT_FAIL;
}

TEST_CASE(stringwriter_flexible_printf_normal) {
    MAYCUP_StringWriter sw;
    const size_t bufsz = 8;

    ASSERT_OK(maycup_stringwriter_ctor_flexible(&sw, bufsz), fail);
    for (int i = 1; i <= 3; ++i) {
        ASSERT_OK(maycup_writer_printf(&sw, "%s %d", "asdasd", 123123), fail);
    }

    ASSERT_EQ(strcmp(sw.buf, "asdasd 123123asdasd 123123asdasd 123123"), 0,
              fail);

    ASSERT_OK(maycup_stringwriter_dtor(&sw, NULL), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_stringwriter_dtor(&sw, NULL);
    return TEST_RESULT_FAIL;
}

TEST_CASE(stringwriter_flexible_printf_realloc_fail) {
    MAYCUP_StringWriter sw;
    const size_t bufsz = 8;

    MOCK_ON(realloc);

    ASSERT_OK(maycup_stringwriter_ctor_flexible(&sw, bufsz), fail);
    ASSERT_EQ(maycup_writer_printf(&sw, "%s %d", "asdasdasdasd", 123123123123),
              MAYCUP_RESULT_MALLOC_FAIL, fail);

    MOCK_OFF(realloc);
    ASSERT_OK(maycup_stringwriter_dtor(&sw, NULL), fail);
    return TEST_RESULT_PASS;
fail:
    MOCK_OFF(realloc);
    maycup_stringwriter_dtor(&sw, NULL);
    return TEST_RESULT_FAIL;
}

TEST_CASE(filewriter_module_common) {
    MAYCUP_FileWriter fw;

    ASSERT_OK(maycup_filewriter_ctor(&fw, runtime_wfile), fail);

    for (size_t i = 1; i <= 20; ++i) {
        ASSERT_OK(maycup_writer_puts(&fw, writer_str_puts_test), fail);
    }
    for (int i = 1; i <= 100; ++i) {
        ASSERT_OK(maycup_writer_printf(
                      &fw, "cur num: %d, next num: %d; %d * %d == %d.\n", i,
                      i + 1, i, i + 1, i * (i + 1)),
                  fail);
    }

    ASSERT_OK(maycup_filewriter_dtor(&fw), fail);

    ASSERT_FILE_EQ_FILE(runtime_wfile,
                        "test/unit/fixture/expected/writer_module.txt", fail);
    return TEST_RESULT_PASS;
fail:
    maycup_filewriter_dtor(&fw);
    return TEST_RESULT_FAIL;
}

TEST_CASE(stringwriter_module_common) {
    MAYCUP_StringWriter sw;

    ASSERT_OK(maycup_stringwriter_ctor(&sw, output_str, OUTPUT_STR_SIZE), fail);

    for (size_t i = 1; i <= 20; ++i) {
        ASSERT_OK(maycup_writer_puts(&sw, writer_str_puts_test), fail);
    }
    for (int i = 1; i <= 100; ++i) {
        ASSERT_OK(maycup_writer_printf(
                      &sw, "cur num: %d, next num: %d; %d * %d == %d.\n", i,
                      i + 1, i, i + 1, i * (i + 1)),
                  fail);
    }

    ASSERT_STR_EQ_FILE(sw.buf, "test/unit/fixture/expected/writer_module.txt",
                       fail);

    ASSERT_OK(maycup_stringwriter_dtor(&sw, NULL), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_stringwriter_dtor(&sw, NULL);
    return TEST_RESULT_FAIL;
}

TEST_CASE(stringwriter_flexible_module_common) {
    MAYCUP_StringWriter sw;

    ASSERT_OK(maycup_stringwriter_ctor_flexible(
                  &sw, MAYCUP_DEFAULT_STRWRITER_FLEXBUF_SIZE),
              fail);

    for (size_t i = 1; i <= 20; ++i) {
        ASSERT_OK(maycup_writer_puts(&sw, writer_str_puts_test), fail);
    }
    for (int i = 1; i <= 100; ++i) {
        ASSERT_OK(maycup_writer_printf(
                      &sw, "cur num: %d, next num: %d; %d * %d == %d.\n", i,
                      i + 1, i, i + 1, i * (i + 1)),
                  fail);
    }

    ASSERT_STR_EQ_FILE(sw.buf, "test/unit/fixture/expected/writer_module.txt",
                       fail);

    ASSERT_OK(maycup_stringwriter_dtor(&sw, NULL), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_stringwriter_dtor(&sw, NULL);
    return TEST_RESULT_FAIL;
}

TEST_SUITE_BEGIN(writer)

TEST_CASE_ADD(filewriter_ctor_normal);
TEST_CASE_ADD(filewriter_ctor_fopen_fail);
TEST_CASE_ADD(filewriter_ctor_illegal_arg);

TEST_CASE_ADD(filewriter_dtor_normal);
TEST_CASE_ADD(filewriter_dtor_double);
TEST_CASE_ADD(filewriter_dtor_fclose_fail);
TEST_CASE_ADD(filewriter_dtor_illegal_arg);

TEST_CASE_ADD(filewriter_puts_normal);
TEST_CASE_ADD(filewriter_puts_fputs_fail);
TEST_CASE_ADD(filewriter_puts_illegal_arg);

TEST_CASE_ADD(filewriter_printf_normal);
TEST_CASE_ADD(filewriter_printf_vfprintf_fail);
TEST_CASE_ADD(filewriter_printf_illegal_arg);

TEST_CASE_ADD(stringwriter_ctor_normal);
TEST_CASE_ADD(stringwriter_ctor_illegal_arg);

TEST_CASE_ADD(stringwriter_dtor_normal_nores);
TEST_CASE_ADD(stringwriter_dtor_normal_res);
TEST_CASE_ADD(stringwriter_dtor_double);
TEST_CASE_ADD(stringwriter_dtor_illegal_arg);

TEST_CASE_ADD(stringwriter_puts_normal);
TEST_CASE_ADD(stringwriter_puts_short);
TEST_CASE_ADD(stringwriter_puts_length_exceeded);
TEST_CASE_ADD(stringwriter_puts_illegal_arg);

TEST_CASE_ADD(stringwriter_printf_normal);
TEST_CASE_ADD(stringwriter_printf_length_exceeded);
TEST_CASE_ADD(stringwriter_printf_vsnprintf_fail);
TEST_CASE_ADD(stringwriter_printf_illegal_arg);

TEST_CASE_ADD(stringwriter_flexible_ctor_normal);
TEST_CASE_ADD(stringwriter_flexible_ctor_malloc_fail);
TEST_CASE_ADD(stringwriter_flexible_ctor_illegal_arg);

TEST_CASE_ADD(stringwriter_flexible_puts_normal);
TEST_CASE_ADD(stringwriter_flexible_puts_realloc_fail);

TEST_CASE_ADD(stringwriter_flexible_printf_normal);
TEST_CASE_ADD(stringwriter_flexible_printf_realloc_fail);

TEST_CASE_ADD(filewriter_module_common);
TEST_CASE_ADD(stringwriter_module_common);
TEST_CASE_ADD(stringwriter_flexible_module_common);

TEST_SUITE_END