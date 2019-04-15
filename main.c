#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

void readCmmdLine()
{
    char cmmdLine[1000];            //String of characters with size 1000
    char * ifExists;                //If & exists in the command line
    int flag = 0;

    while (1)
    {
        printf(">>");
        gets(cmmdLine);                 //Taking the command line from the user
        ifExists = strchr(cmmdLine, '&');      //Function to search for a character, ifExists will be NULL if the given character hasn't been found
        if (  ifExists != NULL )
            flag = 1;                   //If '&' character is found flag will be set to 1 and given to the parent process in the forking function so we can run for ex. firefox and write a new command line at the same time
        parseCmmdLine(cmmdLine, flag);        //Since we've got the command line we need to split it via parseCmmdLine function
    }
}

void parseCmmdLine( char line[], int flag )
{
    char * args[100];               //args is an array of strings which will carry the splited command line
    int i = 0;

    args[i] = strtok(line, " ");    //strtok is a function from string.h library which is used to split the command line with space as delemiter

    if ( strcmp(args[0], "exit") == 0 )
        exit(0);

    if ( args[i] != '\0' )            //if there is a command line
    {
        for (  i=1 ; i<=100; i++ )
            args[i] = strtok('\0', " ");    //After spliting part of the command line a '\0' is added instead so we begin from it and again to the specified delemiter
    }
    else
        readCmmdLine();         //If there is no input the console will start again till it receives a command line

    forking(args, flag);
}

void forking(  char * arguments[], int flag )
{
    pid_t pid;
    pid = fork();               //Fork function which duplicate process
    int status;

    //If pid >= 0 ----> fork success

    if (  pid == 0 )            //Child Process
    {
        execvp(arguments[0], arguments);  //Exectue the child process

        if ( execvp( arguments[0], arguments ) == -1 )
            perror("Error");   //prints the system's error message
    }
    else if (  pid > 0 )           //Parent Process
    {
        if( flag == 0 )
            wait(&status);            //Takes the address of an integer variable and returns 0 in this address if one of its child processes terminates
    }

    else
        perror("Forking Failed!");
}

void signalHandler(int signum);

int main()
{
    signal(SIGCHLD, signalHandler); //function to handel signal

    readCmmdLine();             //function to take command line from the user, read it and send it to parseCmmdLine function

    return 0;
}

void signalHandler(int signum)
{
    FILE * file = fopen("logFile", "a");    //append to file
    pid_t pid;

    if (  file == NULL )            //Tests if file is not exists
        perror("Error! Couldn't open file");

    if (  pid > 0 )                           //Parent Process
        fprintf(file, "%d Child process was terminated\n", getpid());

    fclose(file);
}
