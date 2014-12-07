#define LINE_LEN 80
#define MAX_ARGS 64
#define MAX_ARG_LEN 16
#define MAXPATHS 64
#define MAX_PATH_LEN 96
#define MAXDIRLENGTH 1024
#define MAXHOSTNAMELEN 80
#define WHITESPACE " .,\t\n"

#define TRUE 1
#define FALSE 0

#ifndef NULL
	#define NULL ((void*)0)
#endif

#include <stdio.h> //
#include <stdlib.h> //
#include <unistd.h> //
#include <string.h> //

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/param.h>



typedef struct command_s { 
	char *name;
	int argc;
	char *argv[MAX_ARGS];
} command_t;

char *lookupPath(char **, char**);
int  parseCommand(char *, command_t*);
void parsePath(char **);
void printPrompt();
void readCommand(char *);
int internalChecker(command_t*);
