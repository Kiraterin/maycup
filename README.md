# md2html

[![License](https://img.shields.io/badge/License-GPLv3-bright_green.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Github](https://img.shields.io/badge/Github-repo-white?logo=github)](https://github.com/Kiraterin/md2html)
[![Language](https://img.shields.io/badge/Language-blue?logo=c)](https://www.c-language.org/)

A tiny Markdown-to-HTML tool written in C.

## Goals

- Develop a lightweight md to html tool for my personal website
- Learn software engineering practices, including how to write coherent, maintainable and testable code
- Write all of the code by hand without AI-generation
- Achieve zero memory leaks under ASan

## Usage

### Synopsis

```bash
md2html <input_file> [options]
```

### Options

| Option        | Required | Value              | Description     |
| ------------- | -------- | ------------------ | --------------- |
| `-o <file>`   | Yes      | `path/to/output`   | Set output path |
| `-h`          | No       | None               | Print usage     |

### Example

Convert markdown file to HTML file:

```bash
md2html input.md -o output.html
```

## Build Instructions

### Configurations

There are three build configurations: `release`, `debug` and `test`.
Use `make <config>` to build with a specified configuration, e.g. `make release`.
Build artifacts are generated in
- `./build/<config>/` intermediate build artifacts
- `./bin/<config>/` final build artifact

where `<config>` can be `release`, `debug` or `test`.

Tests will be run automatically after compilation if `<config>` is `test`.

### Make All Configurations

```bash
make all
```

### Clean

```bash
make clean
```

## License

This project is licensed under the [GPL-3.0 License](LICENSE).

## TODO

- [X] Basic lexer-parser structure
- [X] Support source file compilation in recursive folders
- [X] Support debug/release configuration
- [ ] Basic test cases
- [ ] Full support of markdown features
