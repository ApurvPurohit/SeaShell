/*
* Author: Apurv Purohit
* Dept. of Computer Science & Engineering
* Indian Institute of Technology Jammu
* GitHub/ApurvPurohit
* 10-03-21
*/
#include "SeaShell.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ctype.h>
#include <time.h>
#include <sys/wait.h>
#include <fcntl.h>
#define RESET "\033[0m"
#define BLACK "\033[30m"   /* Black */
#define RED "\033[31m"     /* Red */
#define GREEN "\033[32m"   /* Green */
#define YELLOW "\033[33m"  /* Yellow */
#define BLUE "\033[34m"    /* Blue */
#define MAGENTA "\033[35m" /* Magenta */
#define CYAN "\033[36m"    /* Cyan */
#define WHITE "\033[37m"   /* White */
#define BUFF_SIZE 100000   // Max size of history table
// Custom comparators MIN & MAX
#define MAX(a, b) ((a) > (b)) ? (a) : (b)
#define MIN(a, b) ((a) < (b)) ? (a) : (b)
// counters for book-keeping
int h_size = 0;
int bg = 0;
int all = 0;
struct c_des History[BUFF_SIZE];
struct p_info all_proc[BUFF_SIZE];
struct p_info bg_proc[BUFF_SIZE];
char currentWD[PATH_MAX];

void master_execute(char *args[50], char *cmd2, int len, int firstCall)
{ /* implementing all commands of our shell i.e. user-defined and terminal commands */
    /* recursive for situations like 'EXEC n' and '!histn' commands */
    /* also detects and executes context-senstive (&), pipeline and IO redirection commands */
    checkrunning(); /* checks if any background process changed state*/
    int id_for_hist = 0;
    if (args[0] == NULL) /*No Command*/
        return;
    if (strcmp(args[0], "STOP") == 0 && (args[1] == NULL)) /*CAUGHT STOP COMMAND*/
    {
        int status;
        for (int i = 0; i < bg; i++) // KILLING ALL STOPPED BACKGROUND PROCESSES
        {
            if (waitpid(bg_proc[i].pid, &status, WNOHANG) != 0)
                continue;
            else
                kill(bg_proc[i].pid, SIGKILL); /*sending signal SIGKILL*/
        }
        printf("\n%sGoodbye, SeaShell loves you!%s\n\n", MAGENTA, RESET);
        exit(0);
    }
    else if (strcmp(args[0], "HISTORY") == 0) /*CAUGHT COMMAND HISTORY*/
    {
        if (args[1] == NULL)
        {
            printf("\nHISTORY ERROR: No arg \nUsage -> HISTORY <Arg>\nArgs:\n1. BRIEF: Shows only the command name.\n2. FULL: Shows the entire command.\n");
            return;
        }
        else if ((strcmp(args[1], "BRIEF") == 0) && (args[2] == NULL))
            show_history(0); // FLAG=0 FOR BRIEF HISTORY

        else if ((strcmp(args[1], "FULL") == 0) && (args[2] == NULL))
            show_history(1); // FLAG =1 FOR FULL HISTORY
        if (firstCall)       /*Command added to HISTORY only for the first time. This is to avoid multiple entries in case of recursion. */
            addHistory(cmd2);
    }
    else if (strcmp("pid", args[0]) == 0 && (args[1] == NULL)) /*CAUGHT pid command*/
    {
        if (firstCall)
            addHistory(cmd2);
        pid_t pid;
        pid = getpid();
        char path[50];
        // getting process details from proc/pid/stat dir
        // as in the implementation of ls
        sprintf(path, "/proc/%d/status", pid);
        FILE *filepointer = fopen(path, "r");
        if (filepointer == NULL)
        {
            printf("Unable to open status File, No Process of given pid %d\n", pid);
            return;
        }
        char procInfo[100];
        int k = 0;
        printf("Shell Process\nPID: %d\n", pid);
        while (fgets(procInfo, sizeof(procInfo), (FILE *)filepointer) != NULL)
        { // process details corresponding to PID
            if (k == 0)
            {
                printf("%s", procInfo); // Process Name
            }
            if (k == 2)
            {
                printf("%s", procInfo); // Process State
                break;
            }
            k++;
        }
    }
    else if (strcmp("pid", args[0]) == 0 && strcmp("current", args[1]) == 0 && (args[2] == NULL))
    { /* Currently Executing/Stopped processes, CAUGHT 'pid current' command*/
        if (firstCall)
            addHistory(cmd2);
        if (bg == 0)
        {
            printf("--- NO PROCESSES CURRENTLY RUNNING ---\n");
            return;
        }
        int status;
        int k = 1;
        printf("\nList of currently executing processes spawned from this shell:\n");
        for (int i = 0; i < bg; i++)
        {
            if (waitpid(bg_proc[i].pid, &status, WNOHANG) != 0)
                continue;
            else
                printf("%8d. Command: %-40s PID: %d\n", (k++), bg_proc[i].name, bg_proc[i].pid);
        }
        if (k == 1)
            printf("--- NO PROCESSES CURRENTLY RUNNING ---\n");
    }
    else if (strcmp("pid", args[0]) == 0 && strcmp("all", args[1]) == 0 && (args[2] == NULL))
    { /* All spawned processes, CAUGHT 'pid all' command*/
        if (firstCall)
            addHistory(cmd2);
        if (all == 0)
        {
            printf("--- NO PROCESSES SPAWNED ---\n");
            return;
        }
        printf("\nList of all processes spawned from this shell:\n");
        for (int i = 0; i < all; i++)
        {
            printf("%8d. Command: %-40s PID: %d\n", (i + 1), all_proc[i].name, all_proc[i].pid);
        }
    }
    else if (isdigit(*(args[0] + 4)) && (*args[0] == 'h') && (*(args[0] + 1) == 'i') && (*(args[0] + 2) == 's') && (*(args[0] + 3) == 't') && args[1] == NULL)
    { /* CAUGHT COMMAND ----- HISTN */
        int idx = atoi(args[0] + 4) - 1;
        if (idx == -1)
        {
            printf("Error: Invalid Command: Command number does not exist\n");
            return;
        }
        int k = MAX(h_size - idx, 0); // TO HANDLE OVERFLOW
        int j = MIN(h_size, k + idx); // TO HANDLE OVERFLOW
        int a = 1;
        for (int i = k; i < j; ++i)
        {
            if (History[i].full && *(History[i].full) && History[i].full[strlen(History[i].full) - 1] == '\n')
                printf("%d. %s", (a++), History[i].full);
            else
                printf("%d. %s\n", (a++), History[i].full);
        }
        if (firstCall)
            addHistory(cmd2);
    }
    else if (isdigit(*(args[0] + 5)) && (*(args[0] + 1) == 'h') && (*(args[0] + 2) == 'i') && (*(args[0] + 3) == 's') && (*(args[0] + 4) == 't') && (*args[0] == '!') && args[1] == NULL)
    {                                        /* CAUGHT COMMAND ----- !HISTN */
        id_for_hist = atoi(args[0] + 5) - 1; /* getting the value of 'n' and the corresponding index i.e 'n-1' */
        if ((id_for_hist > h_size) || (id_for_hist < 0))
        {
            printf("Error: Invalid Command: Command number does not exist\n");
            return;
        }
        else
        {
            char *cmd = malloc(sizeof(char) * 100);
            char *args[50];
            printf("\nExecuting Command: %s\n", History[id_for_hist].full);
            strcpy(cmd, History[id_for_hist].full);
            int len = parse(cmd, args);
            master_execute(args, History[id_for_hist].full, len, 0); // recursive call to execute commands from history
        }
        if (firstCall)
            addHistory(cmd2);
    }
    else if (strcmp(args[0], "EXEC") == 0)
    { /* CAUGHT COMMAND EXEC -- <CMD_NAME/CMD_IDX> */
        if (args[1] == NULL)
        {
            printf("\nEXEC ERROR: No arg \nUsage -> EXEC <cmd/idx>\nArgs:\n1. cmd: Any supported command.\n2. idx: Index of any command in HISTORY.\n");
            return;
        }
        else
        {
            char **cmd_args = &args[1];
            if (isdigit(**cmd_args) && args[2] == NULL) // EXEC N(idx)
            {
                int idx = atoi(args[1]);
                if ((idx > h_size) || (idx <= 0))
                {
                    printf("Error: Invalid Command; Command number does not exist\n");
                    return;
                }
                printf("\nExecuting Command: %s\n", History[idx - 1].full);
                char *cmd = malloc(sizeof(char) * 100);
                char *args[50];
                strcpy(cmd, History[idx - 1].full);
                int len = parse(cmd, args);
                master_execute(args, History[idx - 1].full, len, 0); // recursive call to execute commands from history
            }
            else if (isalpha(**cmd_args)) // EXEC cmd
            {
                printf("\nExecuting Command: %s\n", &cmd2[5]);
                char *cmd = malloc(sizeof(char) * 100);
                char *args[50];
                strcpy(cmd, &cmd2[5]);
                int len = parse(cmd, args);
                master_execute(args, &cmd2[5], len, 0); // recursive call to execute commands from history
            }
        }
        if (firstCall)
            addHistory(cmd2);
    }
    else if (strcmp("cd", args[0]) == 0)
    {                        /* CAUGHT cd command, needs to be handled by our shell.*/
        if (args[1] == NULL) //error
        {
            printf("Enter the path after cd ,Usage -> cd <pathname>\n");
            return;
        }
        char *replceTilda = args[1];
        if (args[1][0] == '~') /* '~' stands for our shell working directory */
            replceTilda = replace(args[1], "~", currentWD);
        if (chdir(replceTilda) == -1)
            perror("cd Error");
        if (firstCall)
            addHistory(cmd2);
    }
    else
    { /*ALL OTHER CASES: COMMANDS INVOLVING IO REDIRECTION, PIPES, CONTEXT FLAGS (&: BACKGROUND)*/
        if (firstCall)
            addHistory(cmd2);
        int check = 0;
        for (int i = 0; i < 64; ++i) /*check for IO redirection or pipes*/
        {
            if (args[i] == NULL)
                break;
            else if ((strcmp(args[i], "<") == 0) || (strcmp(args[i], ">") == 0) || (strcmp(args[i], "|") == 0))
            {
                check = 1;
                break;
            }
        }
        if (check) /*The command involves pipes and/or IO redirection */
        {          /*NOTE: IO redirection and pipes only supported for foreground processes */
            char *cmdd = malloc(sizeof(char) * 100);
            strcpy(cmdd, cmd2);
            char *args[64];
            parse(cmdd, args);
            char *cmd[64];                                 // necessary to form a new command variable, extracting pipelined command
            char *ip = (char *)malloc(100 * sizeof(char)); /* for storing input file if any */
            char *op = (char *)malloc(100 * sizeof(char)); /* for storing output file if any */
            int k = 0;
            for (int i = 0; i < 64; ++i)
            {
                if (args[i] == NULL)
                    break;
                else if (strcmp(args[i], "<") == 0)
                {
                    strcpy(ip, args[i + 1]); /*input file extracted*/
                    ++i;
                    continue;
                }
                else if (strcmp(args[i], ">") == 0)
                {
                    strcpy(op, args[i + 1]); /*output file extracted*/
                    ++i;
                    continue;
                }
                else
                {
                    cmd[k] = (char *)malloc(sizeof(char) * 100); /*command with args*/
                    strcpy(cmd[k], args[i]);
                    k++;
                }
            }
            cmd[k++] = NULL;
            char **cmd_for_pipeline[64]; /*An array of char* arrays to store array of individual tokenized commands*/
            char *cmd_line[64][64];
            int cnt = 0;  // for each cmd
            int cnt2 = 0; // for each entry of cmd_for_pipeline
            int cnt3 = 0; // for # array in entry of cmd_for_pipeline
            /*This loop basically transforms a command eg. cat file.txt | wc | sort */
            /* to {{"cat","file.txt",NULL},{"wc",NULL},{"sort",NULL},NULL}. Done for the pipeline function*/
            for (int i = 0; i < k; ++i)
            {
                if (cmd[i] == NULL)
                {
                    cmd_line[cnt3][cnt] = NULL;
                    cmd_for_pipeline[cnt2++] = cmd_line[cnt3];
                    cmd_for_pipeline[cnt2] = NULL;
                    cnt3++;
                    break;
                }
                else if (strcmp(cmd[i], "|") == 0)
                {
                    cmd_line[cnt3][cnt] = NULL;
                    cmd_for_pipeline[cnt2++] = cmd_line[cnt3];
                    cnt = 0;
                    cnt3++;
                    continue;
                }
                else
                {
                    cmd_line[cnt3][cnt] = (char *)malloc(sizeof(char) * 100);
                    strcpy(cmd_line[cnt3][cnt], cmd[i]);
                    cnt++;
                }
            }
            cmd2[strlen(cmd2) - 1] = '\0';                   /*null termination*/
            pipelineio_exec(cmd2, cmd_for_pipeline, ip, op); /*execution of pipeline*/
            memset(cmd_line, 0, sizeof(cmd_line));
        }
        else /*The command does'nt involve redirection.*/
            getContext(args, len - 1);
    }
}
int foreGroundProcess(char *line[50], int len)
{
    //Execution of foreground process
    line[len] = NULL; /*null terminated for exec*/
    pid_t pid;
    int status;
    pid = fork();
    if (pid == 0)
    {
        // Child process

        if (execvp(line[0], line) == -1)
        {
            perror("Error");
        }
        exit(0);
    }
    else if (pid < 0)
    {
        // Error forking
        perror("fork Error");
    }
    else
    {
        // Parent process
        do
        {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status)); // Process exited or signalled
        // FOR ALL PROCESS (PID) ENTRY
        char n[100] = "";
        for (int i = 0; i < len; i++)
        {
            strcat(n, line[i]);
            strcat(n, " ");
        }
        all_proc[all].name = (char *)malloc(sizeof(char) * 100);
        strcpy(all_proc[all].name, n);
        all_proc[all].pid = pid;
        all++;
        memset(n, 0, sizeof n); // RESET
    }
    return 1;
}
int backGroundProcess(char *line[50], int len)
{
    line[len] = NULL; /*null terminated for exec*/
    pid_t pid;
    int status;
    pid = fork();
    if (pid == 0)
    {
        // Child process
        // setpgid(pid,pgid)
        //pid =0 -> pid of calling process
        setpgid(0, 0); /*To make the pid of the current process it's pgid.*/
        if (execvp(line[0], line) == -1)
        {
            perror("Error");
        }
        exit(0);
    }
    else if (pid < 0)
    {
        // Error forking
        perror("fork Error");
    }
    else
    {
        // FOR ALL PROCESS (PID) ENTRY
        char n[100] = "";
        for (int i = 0; i < len; i++)
        {
            strcat(n, line[i]);
            strcat(n, " ");
        }
        all_proc[all].name = (char *)malloc(sizeof(char) * 100);
        strcpy(all_proc[all].name, n);
        all_proc[all].pid = pid;
        all++;

        // FOR BACKGROUND PROCESS (PID) ENTRY
        bg_proc[bg].name = (char *)malloc(sizeof(char) * 100);
        strcpy(bg_proc[bg].name, n);
        bg_proc[bg].pid = pid;
        bg++;
        memset(n, 0, sizeof n); // RESET
    }
    return 1;
}
void handle_sigint(int sig) // SIGNAL HANDLER FOR INTERRUPT HANDLING
{
    printf("\n---------- Interrupted ----------\n");
}
void addHistory(char *entry) /* for adding record to history*/
{
    if (h_size > BUFF_SIZE) // History Buffer Filled
    {
        printf("Alas! History Full: Please run again.");
        return;
    }
    History[h_size].full = malloc(sizeof(char) * 100);
    History[h_size].brief = malloc(sizeof(char) * 20);
    strcpy(History[h_size].full, entry);
    char temp[100];
    strcpy(temp, entry);
    char *head = strtok(temp, " "); /*command name for history brief*/
    // For Brief History
    if (head != NULL)
        strcpy(History[h_size].brief, head);
    else
        strcpy(History[h_size].brief, entry);
    memset(temp, 0, sizeof(temp)); /*Reset*/
    h_size++;                      /*h_size=size of HISTORY*/
}
void show_history(int flag) /* showing history --brief and --full */
{
    if (flag == 0) // Option for BRIEF History
    {
        printf("\n---| HISTORY BRIEF |---\n\n");
        if (h_size == 0)
        {
            printf("NO RECORD FOUND\n");
            return;
        }
        for (int i = 0; i < h_size; ++i)
        {
            // This is just a check to see if the last character of the string is line-feed.
            // For better visibility and properly formatted output
            if (History[i].brief && *(History[i].brief) && History[i].brief[strlen(History[i].brief) - 1] == '\n')
                printf("%8d. %s", (i + 1), History[i].brief);
            else
                printf("%8d. %s\n", (i + 1), History[i].brief);
        }
    }
    if (flag == 1) // Option for FULL History
    {
        printf("\n---| HISTORY FULL |---\n\n");
        if (h_size == 0)
        {
            printf("NO RECORD FOUND\n");
            return;
        }
        for (int i = 0; i < h_size; ++i)
        {
            // This is also just a check to see if the last character of the string is line-feed.
            // For better visibility and properly formatted output.
            if (History[i].full && *(History[i].full) && History[i].full[strlen(History[i].full) - 1] == '\n')
                printf("%8d. %s", (i + 1), History[i].full);
            else
                printf("%8d. %s\n", (i + 1), History[i].full);
        }
    }
}
char *replace(const char *s, const char *curr, const char *newW)
{ /*to replace all occurences of substring in a string */
    char *result;
    int i, cnt = 0;
    int newWlen = strlen(newW);
    int oldWlen = strlen(curr);

    // Counting the number of times old word
    // occur in the string
    for (i = 0; s[i] != '\0'; i++)
    {
        if (strstr(&s[i], curr) == &s[i])
        {
            cnt++;
            // Jumping to index after the old word.
            i += oldWlen - 1;
        }
    }
    // Making new string of enough length
    result = (char *)malloc(i + cnt * (newWlen - oldWlen) + 1);
    i = 0;
    while (*s)
    {
        // compare the substring with the result
        if (strstr(s, curr) == s)
        {
            strcpy(&result[i], newW);
            i += newWlen;
            s += oldWlen;
        }
        else
            result[i++] = *s++;
    }
    result[i] = '\0'; /*Every string is null-terminated.*/
    return result;
}
extern void checkrunning() // checks for running child process
{
    int status;
    pid_t a;
    a = waitpid(-1, &status, WNOHANG);
    if (a > 0) /* process status changed */
    {
        // PID = a
        printf("Process with Name: ");
        for (int i = 0; i < bg; ++i) /* to get process name corresponding to state-changed pid*/
        {
            if (bg_proc[i].pid == a)
            {
                printf("%s", bg_proc[i].name);
                break;
            }
        }
        printf("PID: %d, Exited Normally!\n", a);
    }
}
int getContext(char *line[50], int len) /*to get context of execution: foreground or background (&)*/
{
    if (strcmp(line[len - 1], "&") == 0)
    {
        //Back Ground Process call
        line[len - 1] = NULL;
        return backGroundProcess(line, len - 1);
    }
    else
        //ForeGround Process call
        return foreGroundProcess(line, len);
}
int parse(char *currentline, char *argv[]) /*generic routine to parse/tokenize the input command by user*/
{                                          /*routine also handles arbitrary white-space. Eg. ls      -al*/
    int i = 0;
    int j = 0;
    while ((currentline[i] != EOF) && (currentline[i] != '\0') && (currentline[i] != '\n') && (i < 1000))
    {
        while (currentline[i] == ' ') // removes starting white space and handles empty cmd
        {
            currentline[i++] = '\0';
            if ((currentline[i] == EOF) || (currentline[i] == '\n'))
            {
                currentline[i] = '\0';
                break;
            }
        }
        if ((currentline[i] == EOF) || (currentline[i] == '\0') || (currentline[i] == '\n'))
        { /*handling end of command reached situation */
            currentline[i] = '\0';
            break;
        }
        argv[j++] = &currentline[i]; /*adding non white space tokens*/
        while ((currentline[i] != ' ') && (currentline[i] != EOF) && (currentline[i] != '\0') && (currentline[i] != '\n'))
        { /*eliminating white space before next token*/
            i++;
        }
        if ((currentline[i] == EOF) || (currentline[i] == '\0') || (currentline[i] == '\n'))
        { /*handling end of command reached situation */
            currentline[i] = '\0';
            break;
        }
    }
    argv[j] = NULL; // Execvp Command Args are NULL terminated
    return (j + 1);
}
static void pipelineio_exec(char *full, char ***cmd, char *ip, char *op)
{ /*routine for running commands which include pipeline and IO redirection. */
    /* Eg. commands like: cat file.txt > output, cat file.txt | wc | sort > output etc. */
    /* Supports multiple pipe commands, I/O redirection (both <,>) and combination of both. */
    int fd[2];
    pid_t pid;
    int fdd = 0; /* Backup */
    /*loop over commands by sharing pipes */
    while (*cmd != NULL)
    {
        pipe(fd);
        if ((pid = fork()) == -1)
        {
            perror("fork Error");
            exit(1);
        }
        else if (pid == 0)
        {
            dup2(fdd, STDIN_FILENO);
            if (*(cmd + 1) != NULL) /* not pipe end */
            {
                dup2(fd[1], 1);
            }
            else /* Last command of the pipe */
            {
                int fd0;
                if ((fd0 = open(ip, O_RDONLY, 0)) >= 0) /* Input File Valid */
                {
                    dup2(fd0, STDIN_FILENO); /* redirecting standard input to a file */
                    close(fd0);              /* necessary */
                }
                else if (strcmp(ip, "") != 0)
                {
                    perror("Input File Error");
                    return;
                }
                int fd1;
                if ((fd1 = creat(op, 0644)) >= 0) /* Output File Valid */
                {
                    dup2(fd1, STDOUT_FILENO); /* output to be displayed on terminal redirected to file */
                    close(fd1);               /* with the descriptor 'fd1' */
                }
            }
            close(fd[0]); /*close reading end for execution*/
            execvp((*cmd)[0], *cmd);
            perror("Error");
            exit(1);
        }
        else
        {
            wait(NULL);                                              /* Collect childs */
            all_proc[all].name = (char *)malloc(sizeof(char) * 100); /* Adding entries for pid commands */
            strcpy(all_proc[all].name, full);
            all_proc[all].pid = pid;
            all++;
            close(fd[1]); /* Closing the writing end for next iteration */
            fdd = fd[0];
            cmd++; /* next command in pipe */
        }
    }
}
void exec_bat(char *file)
{
    FILE *f = fopen(file, "r");
    if (f == NULL) // File DNE
    {
        perror("Error opening file:");
        return;
    }
    printf("\nRunning the Batch Commands File (%s):\n\n", file);
    char *cmd = malloc(sizeof(char) * 100);
    char cmd2[100];
    char *args[50];
    int len=0;
    size_t len_cmd = 0;
    while (getline(&cmd, &len_cmd, f) != -1)
    {
        printf("%s", cmd);
        strcpy(cmd2, cmd);
        len = parse(cmd, args);
        master_execute(args, cmd2, len, 1); /* execute command */
        printf("\n");
    }
    fclose(f);
    free(cmd); // freeing memory
}
int main(int argc, char *argv[])
{
    signal(SIGINT, handle_sigint); // signal to handle interrupt
    char currentWD[200];
    char shellsWD[200];
    getcwd(currentWD, sizeof(currentWD));
    char *host = malloc(sizeof(char) * 100);
    gethostname(host, 100);
    char *userName = getenv("USER");
    int id_for_hist = 0;
    if (argc >= 2) // Atleast one batch file given !
    {
        for (int i = 1; i < argc; ++i)
        {
            exec_bat(argv[i]);
        }
        printf("\n\n ----- (Post Batch Processing Interactive Mode) -----\n\n");
    }
    while (1)
    {
        checkrunning();
        char *cmd = malloc(sizeof(char) * 100);
        char cmd2[100];
        char *args[50];
        getcwd(shellsWD, sizeof(shellsWD));
        char *cwd1 = shellsWD;
        if (strncmp(currentWD, shellsWD, strlen(currentWD)) == 0) /*if pwd contains shells directory, replace by ~ */
            cwd1 = replace(shellsWD, currentWD, "");
        // PROMPT
        printf("<%s%s%s@%s%s%s:%s~%s%s>", YELLOW, userName, RESET, BLUE, host, RESET, GREEN, cwd1, RESET);
        fgets(cmd, 100, stdin); /* get command */
        strcpy(cmd2, cmd);
        int len = parse(cmd, args);
        master_execute(args, cmd2, len, 1); /* execute command */
        printf("\n");
        free(cmd); // freeing memory
    }
    return 0;
}
