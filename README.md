# kShell

A minimal Unix shell written from scratch in C, built as a learning project to
understand how shells and Unix processes work under the hood.

Inspired by Stephen Brennan's ["Tutorial - Write a Shell in C"](https://brennan.io/2015/01/16/write-a-shell-in-c/),
but with core parts (line reading and tokenizing) re-implemented by hand, and
extended well beyond the tutorial (redirection, pipes, signals, background jobs)
using CS:APP and the man pages.

## Features

- Interactive read–parse–execute loop
- Manual line reading, character by character (`getchar`), with a fixed-size buffer
- Manual tokenizer (whitespace-separated), written without `strtok`
- External command execution via `fork` + `execvp` + `wait`
- Built-in commands (`cd`, `exit`) dispatched through a function-pointer table
- I/O redirection: output (`>`) and input (`<`)
- Pipes (`|`) — connect one command's output to another's input
- Signal handling: `Ctrl+C` (SIGINT) kills the running command but not the shell
- Background jobs (`&`) with automatic reaping of finished children via a SIGCHLD handler
- Graceful handling of EOF (Ctrl+D) and unknown commands

## How it works

The shell runs a read–parse–execute loop:

1. **Read** — `kshell_read_line` reads one line from stdin one character at a
   time until newline (returns the line) or EOF (returns `NULL` to signal exit).
2. **Parse** — `kshell_split_line` splits the line into an `argv`-style array by
   walking it and writing `\0` at word boundaries, storing pointers into the
   original string. No copying, no `strtok`.
3. **Decide** — `kshell_execute` checks for a built-in, a pipe (`|`), and routes
   accordingly.
4. **Launch** — `kshell_launch` forks a child, applies redirection and background
   handling, and runs the command via `execvp`. `kshell_launch_pipe` creates a
   pipe, forks two children, and connects them with `dup2`.

### Key design points

- **Built-ins run in the shell process** — commands like `cd` change the shell's
  own state (working directory). In a forked child, that change would die with
  the child, so built-ins must run in the shell itself.
- **Redirection and pipes both use `dup2`** — a program always writes to `stdout`
  (fd 1) and reads from `stdin` (fd 0), so instead of changing the program, the
  shell changes what those descriptors point to before calling `execvp`.
- **Signals** — the shell ignores SIGINT (`SIG_IGN`) so `Ctrl+C` doesn't kill it,
  while each child resets SIGINT to default (`SIG_DFL`) so it can be interrupted.
  A SIGCHLD handler reaps finished background jobs so no zombies accumulate.

## Build

Requires `gcc` and a Unix-like environment (Linux, WSL, macOS).

```bash
gcc -Wall kShell.c -o kShell
```

## Usage

```bash
./kShell
```

Example commands:

```
kShell> ls -l
kShell> cd ..
kShell> ls > out.txt
kShell> sort < names.txt
kShell> ls | grep k
kShell> sleep 30 &
```

`Ctrl+C` interrupts the running command; `Ctrl+D` (or `exit`) quits the shell.

## Limitations

A learning project, not a full shell. Currently missing:

- No quoting or backslash escaping (arguments split on whitespace only)
- Only single pipes (`a | b`), not chains (`a | b | c`)
- No append (`>>`), globbing (`*.txt`), or full job control (`fg`/`bg`, Ctrl+Z)
- Fixed limits: 1024 chars per line, 64 arguments per command

## Roadmap

- [x] I/O redirection (`>`, `<`)
- [x] Pipes (`|`)
- [x] Signal handling (Ctrl+C)
- [x] Background jobs (`&`) with SIGCHLD reaping

## License

MIT