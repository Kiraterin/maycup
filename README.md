# md2html

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

| Option        | Necessary | Value              | Description     |
| ------------- | --------- | ------------------ | --------------- |
| `-o <file>` | Yes       | `path/to/output` | Set output path |
| `-h`        | No        | None               | Print usage     |

### Example

Convert markdown file to HTML file:

```bash
md2html input.md -o output.html
```

## TODO

### Core

- [X] Basic lexer-parser structure
- [ ] Basic test cases

### Build System

- Refactor the Makefile
  - [X] Support source file compilation in recursive folders
  - [ ] Support debug/release configuration
