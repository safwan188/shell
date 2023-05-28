#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include "stdio.h"
#include "errno.h"
#include "stdlib.h"
#include "unistd.h"
#include <string.h>

#define BufferSize 510
#define MaxArgCount 10
#define MAX_BG_PROCESSES 10
#define MaxBgProcesses 10
int n; // size of var array

typedef struct var {
    char *name;
    char *value;
} var;

typedef struct bg_process {
    pid_t pid;
    bool suspended;
} bg_process;

void tstp_handler(int sig);
void run_commands(char ***commands, int num_commands, bool background,int withvar);

int foreground_process = 0;
bg_process bg_processes[MaxBgProcesses];
int bg_count = 0;
typedef struct {
    pid_t pid;
    bool suspended;
} BgProcess;



/* return pointer to var if exists, null otherwise. */
var* lookup(var *variables, char *named) {


    for (int i = 0; i < n; i++) {
        if (!strcmp(variables[i].name, named)) {
            return  variables+i;
        }
    }
    return NULL;
}

var* saveVar(char *varName, char *varVal, var *EnvVariables) {
    var *exist = lookup(EnvVariables, varName);
    if (exist != NULL) { // var already exists -> change the value
        exist->value = calloc(strlen(varVal)+1, sizeof(char));
        strcpy(exist->value, varVal);
    } else { // var doesn't exist -> add new value
        n++;
        EnvVariables = realloc(EnvVariables, n * sizeof(var));
        EnvVariables[n-1].name = malloc(sizeof(char)*strlen(varName)+1);
        EnvVariables[n-1].value = malloc(sizeof(char)*strlen(varVal)+1);
        strcpy(EnvVariables[n-1].name, varName);
        strcpy(EnvVariables[n-1].value, varVal);
    }
    return EnvVariables;
}

void myfree(char *command, char *nextCommand, char *dir, var *EnvVariables) {
    free(command);
    free(nextCommand);
    for (int i = 0; i < n; i++) {
        free(EnvVariables[i].name);
        free(EnvVariables[i].value);
    }
    free(EnvVariables);
    free(dir);
}

void sig_handler(int sig) {
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG )) > 0) {
        for (int i = 0; i < bg_count; i++) {
            if (bg_processes[i].pid == pid) {
                if (WIFSTOPPED(status)) {
                    bg_processes[i].suspended = true;
                } else if (WIFCONTINUED(status)) {
                    bg_processes[i].suspended = false;
                } else {
                    for (int j = i; j < bg_count - 1; j++) {
                        bg_processes[j] = bg_processes[j + 1];
                    }
                    bg_count--;
                }
                break;
            }
        }
    }
}
void resume_background_process(int index);
void run_commands(char ***commands, int num_commands, bool background,int withvar) {
    int pipefd[2];
    int fd_in = 0;

    for (int i = 0; i < num_commands; i++) {
        if (i < num_commands - 1) {
            pipe(pipefd);
        }

        pid_t pid = fork();
        if (pid == 0) {
            if (fd_in != 0) {
                dup2(fd_in, STDIN_FILENO);
                close(fd_in);
            }

            if (i < num_commands - 1) {
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[0]);
                close(pipefd[1]);
            }
            // If the command is echo, remove the newline character









            execvp(commands[i][0], commands[i]);
            printf("ERR\n");
            exit(1); // Remove the myfree() call from here, let the child process exit
        }
        else {
            if (!background) {
                foreground_process = pid;
                int status;
                waitpid(pid, &status, WUNTRACED);
            } else {

                bg_processes[bg_count].pid = pid;
                bg_processes[bg_count].suspended = false;
                bg_count++;
            }
        }
        if (fd_in != 0) {
            close(fd_in);
        }

        if (i < num_commands - 1) {
            close(pipefd[1]);
            fd_in = pipefd[0];
        }


    }
}
void tstp_handler(int sig) {
    if (foreground_process > 0) {
        kill(foreground_process, SIGSTOP);
        //printf("\nProcess %d suspended\n", foreground_process);
        bg_processes[bg_count].pid = foreground_process;
        bg_processes[bg_count].suspended = true;
        bg_count++;
        foreground_process = 0;

    }
}

void resume_background_process(int index) {
    if (bg_processes[index].suspended) {
        kill(bg_processes[index].pid, SIGCONT);
        bg_processes[index].suspended = false;
    }
}
int main() {
    signal(SIGTSTP, tstp_handler);
    signal(SIGCHLD, sig_handler);
    //int foreground_process = 0;
    bg_processes[MaxBgProcesses];

    int i, argc, mod = 0, status = 0, enterCounter = 0;
    char *argv[MaxArgCount];
    char *command = (char*) malloc(sizeof(char)*BufferSize);
    char *nextCommand = (char*) malloc(sizeof(char)*BufferSize);
    if (command == NULL || nextCommand == NULL) {
        printf("ERR\n");
        exit(1);
    }
    char *token = NULL;
    char *token1 = NULL;
    char *temp = NULL;
    char temp1 [5000];
    for (int j = 0; j < 5000; ++j) {
        temp1[j]='\0';
    }

    var *EnvVariables = NULL;
    var *variable = NULL;
    n = 0;

    int numCmds = 0, numArgs = 0;
    char *dir = getcwd(NULL, 0);

    while (1) {

        for (int j = 0; j < 5000; ++j) {
            temp1[j]='\0';
        }

        if (enterCounter == 3) {
            break;
        }

        if (mod) {
            strcpy(command, nextCommand);
            mod = 0;
        } else {
            printf("#cmd:%d|#args:%d@%s> ", numCmds, numArgs, dir);
            fgets(command, BufferSize, stdin);
            command[strlen(command) - 1] = '\0'; // replace \n with \0
        }
            if (strlen(command) == 0) {
                enterCounter++;
                continue;
            }
        /* Is command empty */
        enterCounter=0;
        char* first,*second,* middle;
        temp = strchr(command, ';');
        first = strchr(command, '"');
        if (temp!=NULL){
        second = strchr(temp, '"');}
        if (temp  != NULL && first  != NULL && second  != NULL && temp-first && second-temp) {
            if((middle = strchr(second, ';')) != NULL){
                *middle='~';
                strcpy(nextCommand, ++middle);
                command = strtok(command, "~");
                mod = 1;
                middle = NULL;

            }

            }else if((temp != NULL&& first ==NULL) ||( temp!=NULL && first !=NULL && temp-first)){
            strcpy(nextCommand, ++temp);
            command = strtok(command, ";");
            mod = 1;
            temp = NULL;
            }

        /* parse command */
        i = 0;

        for (int j = 0; j < strlen(command); ++j) {

             if (command[j+1]=='|' && command[j]!= ' '){
                char  c[2];
                c[0]=command[j];
                c[1]='\0';
                strcat(temp1,c);
                c[0]=' ';
                strcat(temp1,c);

            } else if(command[j]=='\"'){
                 char *t= strchr(command,'$');
                 if(t!=NULL){
                     while (t!=NULL) {
                         if(*t==' '){

                             break;
                         }
                         if(*t==command[j]){
                             command[j]=' ';
                             char  c[2];
                             c[0]='@';
                             c[1]='\0';
                             strcat(temp1,c);
                             t++;
                             break;
                         }
                         t++;
                     }
                 }
                 continue;


             }
             else if (command[j]=='|' && command[j+1]!= ' '){
                char  c[2];
                c[0]=command[j];
                c[1]='\0';
                strcat(temp1,c);
                c[0]=' ';
                strcat(temp1,c);

            }else{
                char  c[2];
                c[0]=command[j];
                c[1]='\0';
                strcat(temp1,c);
            }
        }
        token = strtok(temp1, " ");
        while (token) {










            argv[i++] = token;

            token = strtok(NULL, " ");
        }
        if (i > MaxArgCount) { // more then 10 args -> illegal command
            continue;
        }
        argv[i] = NULL;
        argc = i;

        if (strchr(argv[0], '=')) {
            char *varName = strtok(argv[0], "=");
            char *varVal = strtok(NULL, "=");
            if (varName != NULL && varVal != NULL)
                EnvVariables = saveVar(varName, varVal, EnvVariables);
            continue;
        }

        numArgs += argc;
        numCmds++;
        int withvar=0;
        /* replace variables with values */
        for (int i = 0; i < argc; i++) {
            char d[5000];
            char k[5000];
            for (int j = 0; j < 5000; ++j) {
                k[j]='\0';
            }
            if (argv[i][0] == '$') {
                strcpy(k,argv[i]);
                char * p1,*p;
                if((first= strchr(k,'@'))!=NULL){
                     p= strtok(k,"@");
                     p1= strtok(NULL,"@");
                    int ll=0;
                    variable = lookup(EnvVariables, ++p);


                } else{
                    variable = lookup(EnvVariables, ++argv[i]);
                }

                if (variable != NULL) {
                    if(p1){
                        strcat(d,variable->value);
                        strcat(d,p1);
                        argv[i]=d;

                    }else {
                        argv[i] = variable->value;
                    }
                    p1=NULL;


                }
            }
        }

        if (!strcmp(argv[0], "cd")) {
            printf("cd not supported\n");
            continue;
        }        if (!strcmp(argv[0], "bg")) {
            if (argc == 1) {
                int index = 0;
                if (index >= 0 ) {
                    resume_background_process(index);
                } else {
                    printf("Invalid background process index.\n");
                }
            } else {
                printf("Usage: bg <index>\n");
            }
            continue;
        }

        bool background = false;
        if (!strcmp(argv[argc - 1], "&")) {
            background = true;
            argv[--argc] = NULL;
        }

        char ***commands = NULL;
        int num_commands = 0;

        commands = (char ***)malloc((num_commands) * sizeof(char **));
        commands[num_commands] = &argv[0];
        num_commands++;

        for (int i = 1; i < argc; i++) {
            if (!strcmp(argv[i], "|")) {
                argv[i] = NULL;
                commands = (char ***)realloc(commands, (num_commands + 1) * sizeof(char **));
                commands[num_commands] = &argv[i + 1];
                num_commands++;
            }
        }

        if (num_commands == 1) {
            if (!strcmp(argv[argc - 2], ">")) {
                int fd = open(argv[argc - 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                argv[argc-2]=NULL;
                argv[argc-1]=NULL;
                if (fd == -1) {
                    perror("open");
                    continue;
                }

                int stdout_fd = dup(STDOUT_FILENO);
                dup2(fd, STDOUT_FILENO);

                run_commands(commands, num_commands, background,withvar);

                dup2(stdout_fd, STDOUT_FILENO);
                close(stdout_fd);
            } else {
                run_commands(commands, num_commands, background,withvar);
            }
        } else {
            run_commands(commands, num_commands, background,withvar);
        }

        free(commands);

    }

    myfree(command, nextCommand, dir, EnvVariables);

    return 0;
}

