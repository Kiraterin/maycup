/**
 * @file common.h
 * @brief Common header
 * @date 2026-07-08
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

#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>
#include <stdint.h>

// Parameter ("Read" excludes checking value)
#define MAYCUP_IN    // Read only
#define MAYCUP_OUT   // Write only
#define MAYCUP_INOUT // Read & Write
#define MAYCUP_MOVE  // Move ownership

#endif // COMMON_H