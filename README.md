# Maycup

[![License](https://img.shields.io/badge/License-GPLv3-bright_green.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Github](https://img.shields.io/badge/Github-repo-white?logo=github)](https://github.com/Kiraterin/maycup)
[![Language](https://img.shields.io/badge/Language-C23-blue)](https://www.c-language.org/)

A tiny Markdown-to-HTML tool written in C.

The name "Maycup" is a wordplay on "Markdown" ("May c up" and "Mar k down") and a homophone of "make up", which means to "make up" a Markdown file by converting it into HTML.

## Goals

- Develop a lightweight md to html tool for my personal website
- Learn how to maintain an open-source project
- Learn software engineering practices, including how to write coherent, maintainable and testable code
- No AI generated code, write all of the code by hand
- Zero memory leaks under ASan

## Quick Start

```bash
# Clone and build
git clone https://github.com/Kiraterin/maycup.git
cd maycup
make release
# Convert this file into HTML
./build/release/bin/maycup README.md -o README.html
# (Optional) Install for the current user
PREFIX=~/.local make install
# (Optional) Install system-wide
sudo make install
```

## Features
- Headings
- Paragraphs with no style

## Usage

### Synopsis

```bash
maycup <input_file> [options]
```

### Options

| Option      | Required | Value            | Description     |
| ----------- | -------- | ---------------- | --------------- |
| `-o <file>` | Yes      | `path/to/output` | Set output path |
| `-h`        | No       | None             | Print usage     |
| `-v`        | No       | None             | Print version   |

### Example

Convert markdown file into HTML file:

```bash
maycup input.md -o output.html
```

## Build Instructions

### Installation

Run `make install` to install the program.
Set the `PREFIX` environment variable to customize the installation path. (e.g. `PREFIX=~/.local make install`)
By default, `PREFIX` is `/usr/local` and three types of files will be installed to `/usr/local/bin`, `/usr/local/lib` and `/usr/local/include`.
Use `sudo` when installing system-wide.

### Build Configurations

There are three build configurations: `release`, `debug` and `test`.
Use `make <config>` to build with a specified configuration, e.g. `make release`.
Use `make all` to build all target configurations and generate the coverage report (but not to install).
Build artifacts are generated in `./build/<config>/`.

For the `release` and `debug` configuration, final build artifacts are:
- `./build/<config>/bin/maycup`: The CLI program
- `./build/<config>/lib/libmaycup.a`: The static library
- `./build/<config>/lib/libmaycup.so.<version>`: The shared library

e.g. `./build/release/lib/libmaycup.so.0.1.0` is the shared library under `release` configuration, and the version is `0.1.0`.

In the `test` configuration, only `./build/test/bin/maycup_test` is generated and tests are run automatically after compilation.

### Generate Test Coverage Report

The report page is `./build/test/cov/index.html`

```bash
make cov
```

### Clean

```bash
make clean
```

## License

This project is licensed under the [GPL-3.0 License](LICENSE).

## TODO

- [x] Basic lexer-parser structure
- [x] Support source file compilation in recursive folders
- [x] Support debug/release configuration
- [x] Basic test cases
- [ ] Full support of markdown features
