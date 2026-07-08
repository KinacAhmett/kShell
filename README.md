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
- I/O redirection: output (`>`) and input (`<`)
- Pipes (`|`) — connect one command's output to another's input
- Graceful handling of EOF (Ctrl+D) and unknown commands

## How it works

The shell runs a loop with four stages:

1. **Read** — `kshell_read_line` reads one line from stdin one character at a
   time until it hits a newline (returns the line) or EOF (returns `NULL` to
   signal exit).
2. **Parse** — `kshell_split_line` walks the line and splits it into an
   `argv`-style array of words, writing `\0` at word boundaries and storing
   pointers into the original string. No copying, no `strtok`.
3. **Decide** — `kshell_execute` checks whether the command is a built-in, or
   contains a pipe (`|`), and routes it accordingly.
4. **Launch** — for a plain command, `kshell_launch` forks a child, applies any
   redirection, and runs it via `execvp`. For a pipe, `kshell_launch_pipe`
   creates a pipe, forks two children, and connects them with `dup2`.

### Why built-ins are separate

Commands like `cd` change the shell's own state (its working directory). If run
in a forked child, the change would die with the child and never reach the
parent. So built-ins must run in the shell process itself, not in a child.

### How redirection and pipes work

Redirection and pipes both rely on the same idea: a program always writes to
`stdout` (fd 1) and reads from `stdin` (fd 0), so instead of changing the
program, the shell changes what those descriptors point to using `dup2` —
before calling `execvp`. Redirection points fd 1 (or fd 0) at a file; a pipe
points one command's fd 1 and another's fd 0 at the two ends of a pipe.

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

```
kShell> ls -l
kShell> cd ..
kShell> ls > out.txt
kShell> sort < names.txt
kShell> ls | grep k
kShell> exit
```

Press **Ctrl+D** on an empty line to exit as well.

## Limitations

This is a learning project, not a full shell. Currently missing:

- No quoting or backslash escaping (arguments split on whitespace only)
- Only single pipes (`a | b`), not chains (`a | b | c`)
- No append (`>>`), globbing (`*.txt`), or background jobs (`&`)
- No signal handling (Ctrl+C)
- Fixed limits: 1024 chars per line, 64 arguments per command

## Roadmap

- [x] I/O redirection (`>`, `<`)
- [x] Pipes (`|`)
- [ ] Quoting support (`echo "two words"`)
- [ ] Multiple pipes (`a | b | c`)
- [ ] More built-ins (`help`, `pwd`)
- [ ] Signal handling (Ctrl+C)

## License

MIT