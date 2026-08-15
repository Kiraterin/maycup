/**
 * @file test.c
 * @brief Test tool
 * @date 2026-07-22
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

#include "test.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

void run_all_test_suites() {
    size_t pass_cnt_all = 0;
    size_t fail_cnt_all = 0;
    for (size_t i = 0; i < TEST_CTX.suite_cnt; ++i) {
        TestSuite *cur = &TEST_CTX.suites[i];
        printf(COLOR_LIGHT_GREEN "Running test suite: " COLOR_RESET "%s\n\n",
               cur->suite_name);
        size_t pass_cnt = 0;
        size_t fail_cnt = 0;
        for (size_t j = 0; j < cur->case_cnt; ++j) {
            TestResult res = cur->cases[j].case_func();
            switch (res) {
            case TEST_RESULT_PASS:
                printf(COLOR_LIGHT_GREEN "PASS " COLOR_RESET "%s\n",
                       cur->cases[j].case_name);
                ++pass_cnt;
                break;
            case TEST_RESULT_FAIL:
                printf(COLOR_LIGHT_RED "FAIL " COLOR_RESET "%s\n",
                       cur->cases[j].case_name);
                ++fail_cnt;
                break;
            }
            fflush(stdout);
        }
        if (pass_cnt == 0) {
            printf(COLOR_RESET);
        } else {
            printf(COLOR_LIGHT_GREEN);
        }
        printf("\n %zu" COLOR_RESET " Passed, ", pass_cnt);
        if (fail_cnt == 0) {
            printf(COLOR_RESET);
        } else {
            printf(COLOR_LIGHT_RED);
        }
        printf("%zu" COLOR_RESET " Failed\n\n\n", fail_cnt);
        pass_cnt_all += pass_cnt;
        fail_cnt_all += fail_cnt;
    }
    printf("=====================\n\n");
    printf("Summary: \n");
    if (pass_cnt_all == 0) {
        printf(COLOR_RESET);
    } else {
        printf(COLOR_LIGHT_GREEN);
    }
    printf("%zu" COLOR_RESET " Passed, ", pass_cnt_all);
    if (fail_cnt_all == 0) {
        printf(COLOR_RESET);
    } else {
        printf(COLOR_LIGHT_RED);
    }
    printf("%zu" COLOR_RESET " Failed\n\n", fail_cnt_all);
    printf("=====================\n\n\n");
}

bool assert_file_eq_file(const char *file_a, const char *file_b) {
    FILE *fpa, *fpb;
    fpa = fopen(file_a, "r");
    fpb = fopen(file_b, "r");
    if (fpa == NULL || fpb == NULL) {
        return false;
    }

    long sza, szb;
    fseek(fpa, 0, SEEK_END);
    fseek(fpb, 0, SEEK_END);
    sza = ftell(fpa);
    szb = ftell(fpb);
    if (sza != szb) {
        return false;
    }
    char *fa_content, *fb_content;
    fa_content = (char *)malloc(sza * sizeof(char) + 1);
    fb_content = (char *)malloc(szb * sizeof(char) + 1);
    fa_content[sza] = '\0';
    fb_content[szb] = '\0';
    rewind(fpa);
    rewind(fpb);
    fread(fa_content, sizeof(char), sza, fpa);
    fread(fb_content, sizeof(char), szb, fpb);

    bool res = strcmp(fa_content, fb_content) == 0;
    free(fa_content);
    free(fb_content);
    return res;
}

bool assert_str_eq_file(const char *str, const char *file) {
    FILE *fp = fopen(file, "r");
    if (fp == NULL) {
        return false;
    }

    fseek(fp, 0, SEEK_END);
    long sz = ftell(fp);
    char *content = (char *)malloc(sz * sizeof(char) + 1);
    content[sz] = '\0';
    rewind(fp);
    fread(content, sizeof(char), sz, fp);

    bool res = strcmp(str, content) == 0;
    free(content);
    return res;
}

void *malloc_mock(size_t p) {
    if (TEST_CTX.mock_state.m_malloc) {
        return NULL;
    } else {
        return malloc(p);
    }
}

void *realloc_mock(void *pa, size_t pb) {
    if (TEST_CTX.mock_state.m_realloc) {
        return NULL;
    } else {
        return realloc(pa, pb);
    }
}

FILE *fopen_mock(const char *pa, const char *pb) {
    if (TEST_CTX.mock_state.m_fopen) {
        return NULL;
    } else {
        return fopen(pa, pb);
    }
}

int fclose_mock(FILE *p) {
    if (TEST_CTX.mock_state.m_fclose) {
        return EOF;
    } else {
        return fclose(p);
    }
}

int feof_mock(FILE *p) {
    if (TEST_CTX.mock_state.m_feof) {
        return false;
    } else {
        return feof(p);
    }
}

long ftell_mock(FILE *p) {
    if (TEST_CTX.mock_state.m_ftell) {
        return -1L;
    } else {
        return ftell(p);
    }
}

long fseek_mock(FILE *pa, long pb, int pc) {
    if (TEST_CTX.mock_state.m_fseek) {
        return -1;
    } else {
        return fseek(pa, pb, pc);
    }
}

int fputs_mock(const char *pa, FILE *pb) {
    if (TEST_CTX.mock_state.m_fputs) {
        return EOF;
    } else {
        return fputs(pa, pb);
    }
}

int vfprintf_mock(FILE *pa, const char *pb, va_list pc) {
    if (TEST_CTX.mock_state.m_vfprintf) {
        return -1;
    } else {
        return vfprintf(pa, pb, pc);
    }
}

int vsnprintf_mock(char *pa, size_t pb, const char *pc, va_list pd) {
    if (TEST_CTX.mock_state.m_vsnprintf) {
        return -1;
    } else {
        return vsnprintf(pa, pb, pc, pd);
    }
}