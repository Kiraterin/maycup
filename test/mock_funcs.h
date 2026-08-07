/**
 * @file mock_funcs.h
 * @brief Mock function declarations and macro definitions
 * @date 2026-08-02
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

#ifdef TEST

#include <stdio.h>

#define malloc malloc_mock
#define realloc realloc_mock
#define fopen fopen_mock
#define fclose fclose_mock
#define feof feof_mock
#define ftell ftell_mock
#define fseek fseek_mock

void *malloc_mock(size_t p);
void *realloc_mock(void *pa, size_t pb);
FILE *fopen_mock(const char *pa, const char *pb);
int fclose_mock(FILE *p);
int feof_mock(FILE *p);
long ftell_mock(FILE *p);
long fseek_mock(FILE *pa, long pb, int pc);

#endif // TEST
