# Shell Communication Project 🐚📡

## Overview 🌐

This C project implements a basic shell that supports communication and interaction using various shell commands and functionalities, such as variable assignment, `cd` command, background process management, and I/O redirection using pipes.

## Features 🌟

- **Variable Assignment**: Assign values to variables which can be utilized in the shell.
- **Background Process Management**: Manage, resume, and interact with processes running in the background.
- **I/O Redirection and Piping**: Redirect the output of commands to files and support command piping.
- **Command Execution**: Execute commands and manage arguments with consideration to variable values.

## Prerequisites 📋

Ensure you have a C compiler installed to compile and run the code.

## Compilation 🛠️

To compile the code, navigate to the project directory and use the following command:

```sh
gcc -o myshell baisc_shell.c


Usage 💻
Execute the compiled output to start the shell:
./myshell
variable_name=value
command_name &
command1 | command2
command > filename
Special Commands 🔑
bg: Resume a suspended background process.
Code Structure 🏗️
Variables and Structures: Defined structures like var and bg_process for variable storage and background process management.
Function Definitions: Various functions like lookup, saveVar, run_commands, and signal handlers.
Main Loop: Continuously accepts and processes user inputs, managing command parsing and execution.
Limitations & Notes 📜
The cd command is not supported.
Ensure to adhere to the maximum allowed buffer sizes, arguments, and background processes as defined in the macros.
The code handles signal processing for managing background processes.
