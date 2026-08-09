/**
 * @file mock_funcs_undef.h
 * @brief Mock function macro undefinitions
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

#undef malloc
#undef realloc
#undef fopen
#undef fclose
#undef feof
#undef ftell
#undef fseek
#undef fputs
#undef vfprintf

#endif // TEST