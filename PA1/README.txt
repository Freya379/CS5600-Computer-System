Name: YU MA – Freya
Login Info: Github ID: freyayuma

updated again for the new shellnew.c to correct the batch file and exit problem.

1.	My shell.c Design Overall (please note I used exit not quit, in the PA1 description, it says quit, but they are the same thing):

•	Architecture: The shell program, implemented in C, adopts a modular design approach. It defines a series of built-in commands (such as cd, help, exit, and history) and is capable of executing external commands. The program utilizes fork() and execvp() system calls to create new processes and execute commands.
•	Command Processing: The program can process command-line arguments separated by spaces and supports the execution of multiple commands separated by semicolons (;). It also provides a simple history feature that records executed commands.
•	Interactive and Batch Modes: The shell supports two modes of operation. In interactive mode, it prompts the user for command input; in batch mode, it reads and executes commands from a file.

2.	Complete specification, Describe how you handled any ambiguities in the specification:

•	The Command-Line Parsing: I used strtok() function, with ";" as the command separator and " \t\r\n\a" as the delimiter for arguments within a command.
•	Empty Commands and Extra Whitespaces: it is designed to consider possible empty commands (e.g., no commands between ";") and extra whitespaces in the command line. Before executing a command, it checks whether the command string is empty to avoid trying to execute an empty command. The strtok() function already adequately handles extra whitespaces.

3.	Known bugs or problems:

•	Error Handling: When encountering a command that cannot be executed, the program prints an error message but does not terminate execution.
•	History Commands Limitation: The number of history commands is limited to 1024, which may not be suitable for long-running shell sessions.
•	Distinguishing Between Built-in and External Commands: In processing commands, the program first checks if the command is a built-in one; if not, it tries to execute it as an external command. This approach is simple and effective but means built-in command names cannot have the same name as external programs existing in the system path.

4.	Reference:
https://nettee.github.io/posts/2019/Tutorial-Write-a-Shell-in-C/
https://zhuanlan.zhihu.com/p/360923356



