/**
 * @file test.c
 * @brief Test tool
 * @date 2026-07-22
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

#include "test.h"
#include <stdio.h>

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