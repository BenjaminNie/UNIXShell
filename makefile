shell: shell.h shell.c 
	gcc shell.c -o shell
gdb: shell.h shell.c
	gcc -ggdb -o shell_gdb shell.c
