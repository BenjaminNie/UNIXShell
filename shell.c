#include "shell.h"

char *user;
char computer[MAXHOSTNAMELEN];
int pos = 0;   

int main(void)
{
	
	int pid; //variable that gets fork() return
	int status; 
	char commandLine[LINE_LEN];
	char *pathv[MAXPATHS];
	int i;
	command_t* command  = malloc( sizeof(command_t) );

	// Get display prompt information
	user = getlogin();
	gethostname(computer,sizeof(computer));
	parsePath(pathv); /*Get directory paths from PATH*/

	printf("Entering....\n");
	while(TRUE){
		fflush(stdin);
		printPrompt();

		/*Read the command line and parse it*/
		readCommand(commandLine);
		parseCommand(commandLine,command);
		
		/*Check to see if internal commands were called*/
		if( internalChecker(command) ==1 ){	
			continue;
		}
 
		else if( internalChecker(command)==0){
			break;
		}
 		
		/*Get the full pathname for the file*/
		free(command->name);
		command->name = lookupPath(command->argv, pathv);		

		if(command->name == NULL) {//means that whatever they typed in doesn't exist
			printf("%s: command not found.",command->argv[0]);
			free(command->name);
			continue;
		}
		
		pid = fork(); 

		if(pid < 0) 
		{
			printf("FATAL error: no child produced. \n");
			exit(3);		
		}

		else if(pid == 0)	//Child
		{
			if(strcmp(command->argv[command->argc-1],"&") == 0)
				command->argv[command->argc-1] = NULL; //so that '&' is not passed as a parameter
			  
			/*Redirection*/
			if(command->argc > 1 && !strcmp(command->argv[1], ">"))
		        {
		            if (command->argc == 2)
		            {
		                printf("Syntax error near unexpeted token 'newline'\n");   
		            }
		            else
		            {
		                int    d;
		                FILE*    stream;
		                stream = fopen(command->argv[2], "w");
		               
		                d = fileno(stream);
		                dup2(d, STDOUT_FILENO);
		                command->argv[1]=NULL;
		                command->argv[2]=NULL;
		                execv(command->name,command->argv); 
		                close(d);                   
		            }       
		        }
			else
			{
				 execv(command->name,command->argv);
			}
		}	
		else			//Parent
		{	
			if(strcmp(command->argv[command->argc-1],"&") != 0) //will wait if the last param is anything but '&'
				waitpid(pid,&status,0);
			else
				continue;
		}
	}
	/*Shell termination*/
	printf("Exiting....\n");
	free(command->name);
	free(command);
	for(i=0;(pathv[i]!=NULL) && (i<MAXPATHS);i++){
		free( pathv[i] );
		pathv[i]=NULL;	
	}
}

/*
*	This function reads the PATH variable for this environment, and then builds an array
*	of the directories contained in PATH
*
*	@args:
*	dirs - array of strings.  Each element stores one directory contained inside path variable
*/
void parsePath(char *dirs[])
{
	char *pathEnvVar;
	char *directoryInPath;
	pos = 0; //index for dirs[] 
	
	// assign system's PATH variable to pathEnvVar
	pathEnvVar = getenv ("PATH");
	
	// set ":" literal as the delimiter for splitting path variable into individual directories
	directoryInPath = strtok(pathEnvVar,":");

	while (directoryInPath) {
		dirs[pos]= (char *) malloc(strlen(directoryInPath + 1));
		strcpy( dirs[pos], directoryInPath);
		directoryInPath = strtok(NULL, ":"); 
		pos++;
	}
		
	return;
}

/*
*	Prints the prompt.  Prints in the format of:
*	<username>@<hostname>:<current directory>
*
*/
void printPrompt()
{
	char cwd[MAXDIRLENGTH];
	getcwd(cwd,sizeof(cwd));
	printf ("%s@%s:%s$ ",user,computer,cwd) ;

	return;
}

/*
*	Populates the argv[] and argc fields.  argv[] corresponds to the flags passed 
*	with the command and argc represents the number of total command arguments	
*
*	@args
*	cLine - user input into the command line
*	cmd - data structure that saves user input into the command line 
*/
int parseCommand(char *cLine, command_t *cmd) 
{ 
	int argc;
	char **clPtr; 

	/* Initialization */ 
	argc = 0; 

	/* Fill argv[] */
	while((cmd->argv[argc] = strsep(&cLine, WHITESPACE)) != NULL) {
		printf ("The value or argc is %d and the vale of cmd-argv[argc] is %s\n", argc, cmd->argv[argc]);
		argc++;
	}

	cmd->argc = argc;
	return 0;
}

/*
*	Reads user input from stdin into a buffer
*
*	@args
*	buffer - string buffer to read user input into
*/
void readCommand(char *buffer)
{
	gets (buffer);

	return;
}

/*      
*       This function searches the directories identified by the dir argument to see if
*       argv[0] (the file name) appears there.Allocate a new string, place the full path name
*       in it, then return the string.
*
*	@args
*	argv - array of strings.  the first element contains the command and all following
*		elements contain flags specific to the command
*	dir - array of strings.  contains all the directories contained in path variable
*
*	@returns
	result - the directory which the command is located in
*/
char *lookupPath(char **argv, char **dir) {
	char *result; 
	char pName[MAX_PATH_LEN];
	int i;
	
	/* Check to see if file name is already an absolute path name */
	if(*argv[0] == '/') {
		result=argv[0];
		return result;
	}

	/* Look in PATH directories. Use access() to see if the file is in a dir. */
	for(i = 0; i < pos ; i++) { 
				
		strcpy(pName, dir[i]);
		strcat (pName, "/");	
		strcat (pName, argv[0]);
		
		if(access(pName,F_OK)==0){			
			result = (char*) malloc (strlen(pName)+1); 
			strcpy(result,pName);
			return result;		
		}

	}
	/* if file name not found in any path variable till now then*/
	return NULL;
}

/*
*	This function is used to check if the special commands were invoked and executes them
*
*	@args
*	cmd - data structure that contains the command the user entered
*/
int internalChecker(command_t *cmd)
{
	if( !strcmp(cmd->argv[0],"cd") )  //cd ..
	{
		if ( cmd->argc == 4 )
			chdir("..");

		else if ( cmd->argc == 1 )  //cd
			chdir("/");			

		else if ( cmd->argc == 2)  //cd [Filepath]
			if ( chdir( cmd->argv[1] ) )  
				printf("%s: Not a directory\n",cmd->argv[1]);
		
		else
			printf("Could not process command");

		return 1;
	}

	else if( !strcmp(cmd->argv[0],"pwd") && (cmd->argc==1) )
	{
		system("pwd");
		return 1;
	}

	else if( !strcmp(cmd->argv[0],"exit") || !strcmp(cmd->argv[0],"quit") )
	{	
		return 0;
	}

	else
		return 2;	
}
