# kShell

A minimal Unix shell written from scratch in C, built as a learning project to
understand how shells and Unix processes work under the hood.

Inspired by Stephen Brennan's ["Tutorial - Write a Shell in C"](https://brennan.io/2015/01/16/write-a-shell-in-c/),
but with core parts (line reading and tokenizing) re-implemented by hand to
learn the mechanics rather than copy them.

## Features

- Interactive read–parse–execute loop
- Manual line reading, character by character (`getchar`), with a fixed-size buffer
- Manual tokenizer (whitespace-separated), written without `strtok`
- External command execution via `fork` + `execvp` + `wait`
- Built-in commands handled inside the shell itself:
  - `cd` — change directory
  - `exit` — quit the shell
- Built-ins dispatched through a function-pointer table (easy to extend)
- Graceful handling of EOF (Ctrl+D) and unknown commands

## How it works

The shell runs a loop with four stages:

1. **Read** — `kshell_read_line` reads one line from stdin one character at a
   time until it hits a newline (returns the line) or EOF (returns `NULL` to
   signal exit).
2. **Parse** — `kshell_split_line` walks the line and splits it into an
   `argv`-style array of words, writing `\0` at word boundaries and storing
   pointers into the original string. No copying, no `strtok`.
3. **Decide** — `kshell_execute` checks whether the command is a built-in by
   looking it up in a table. If so, it calls the matching built-in function.
4. **Launch** — if it's not a built-in, `kshell_launch` forks a child process,
   replaces it with the target program via `execvp`, and the parent waits for
   it to finish.

### Why built-ins are separate

Commands like `cd` change the shell's own state (its working directory). If run
in a forked child, the change would die with the child and never reach the
parent. So built-ins must run in the shell process itself, not in a child.

## Build

Requires `gcc` and a Unix-like environment (Linux, WSL, macOS).

```bash
gcc -Wall kShell.c -o kShell
```

## Usage

```bash
./kShell
```

Then type commands at the prompt:

kShell> ls -l
kShell> cd ..
kShell> pwd
kShell> exit

Press **Ctrl+D** on an empty line to exit as well.

## Limitations

This is a learning project, not a full shell. Currently missing:

- No quoting or backslash escaping (arguments split on whitespace only)
- No piping (`|`) or redirection (`>`, `<`)
- No globbing (`*.txt`)
- Fixed limits: 1024 chars per line, 64 arguments per command

## Roadmap

- [ ] Quoting support (`echo "two words"`)
- [ ] I/O redirection (`>`, `<`)
- [ ] Pipes (`|`)
- [ ] More built-ins (`help`, `pwd`)
- [ ] Signal handling (Ctrl+C)

## License

MIT