# Custom Shell

This is a simple custom shell implementation in C that supports basic shell functionalities such as executing commands, handling built-in commands, and batch mode execution. The shell also supports I/O redirection.

## Features

- **Basic Command Execution:** The shell can execute external commands from specified paths in the `path` built-in command or from the default `/bin/` directory.

- **Built-In Commands:** The shell supports the following built-in commands:
  - `exit`: Exits the shell.
  - `cd`: Changes the current working directory.
  - `path`: Sets the search paths for external commands.

- **I/O Redirection:** The shell can redirect standard output to a file using the `>` operator.

- **Interactive Mode:** Run the shell interactively, where you can enter commands directly.

- **Batch Mode:** Run the shell in batch mode, where it reads and executes commands from a specified batch file.

## Prerequisites

Before using this shell, make sure you have the following installed:

- GCC (GNU Compiler Collection)
- Linux-based Operating System (Tested on Ubuntu)

## Getting Started

1. Clone the repository:

   ```shell
   git clone https://github.com/your-username/custom-shell.git

2. Compile the code:

   ```shell
   make witsshell

3. Run the shell interactively:

   ```shell
   ./witsshell

4. Run the shell in batch mode with a batch file:

   ```shell
   ./custom_shell batch_file.txt

