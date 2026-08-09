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
#include <string.h>

#include "fixture/input/writer_str_puts_test.h"

static const char *const runtime_wfile = "./build/test/tmp/filewriter.txt";

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
    ASSERT_EQ(maycup_writer_printf(&fw, ""), MAYCUP_RESULT_ERRNO,fail);

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
    ASSERT_EQ(maycup_writer_printf(NULL, ""),
              MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);
    ASSERT_EQ(maycup_writer_printf(&fw, NULL), MAYCUP_RESULT_ILLEGAL_ARGUMENT,
              fail);
    ASSERT_EQ(fw.base.puts(NULL, ""),
              MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);
    ASSERT_EQ(fw.base.puts((MAYCUP_Writer *)&fw, NULL),
              MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);

    ASSERT_OK(maycup_filewriter_dtor(&fw), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_filewriter_dtor(&fw);
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

TEST_SUITE_END