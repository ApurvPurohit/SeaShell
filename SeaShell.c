#include <time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <ctype.h>
#define BUFF_SIZE 100000 // Max size of history table
// Custom comparators MIN & MAX
#define MAX(a, b) ((a) > (b)) ? (a) : (b)
#define MIN(a, b) ((a) < (b)) ? (a) : (b)
// counters for book-keeping
int h_size = 0;
int bg = 0;
int all = 0;
struct c_des // Struct for maintaining HISTORY
{
    char *brief;
    char *full;
};
struct p_info // Struct for maintaining PID records
{
    char *name;
    int pid;
};
struct c_des History[BUFF_SIZE];
struct p_info all_proc[BUFF_SIZE];
struct p_info bg_proc[BUFF_SIZE];
char deviceName[100], currentWD[PATH_MAX];
char *userName;
char args[PATH_MAX];
int childpid;
char *childProcName;
void handle_sigint(int sig) // SIGNAL HANDLER FOR INTERRUPT HANDLING
{
    printf("\n---------- Interrupted ----------\n");
}
int getPath(char *list[50], int len)
{ // returns the path
    int i;
    for (i = 1; i < len; i++)
    {
        if (list[i][0] != '-' && list[i][0] != '&')
        {
            return i;
        }
    }
    return -1;
}
void addHistory(char *currentline) // adding records to history
{
    if (h_size > BUFF_SIZE) // Buffer Filled
    {
        printf("Alas! History Full: Please run again.");
        return;
    }
    History[h_size].full = malloc(sizeof(char) * 100);
    History[h_size].brief = malloc(sizeof(char) * 20);
    strcpy(History[h_size].full, currentline);
    char temp[100];
    strcpy(temp, currentline);
    char *head = strtok(temp, " ");
    if (head != NULL) // For Brief History
        strcpy(History[h_size].brief, head);
    else // For Full History
        strcpy(History[h_size].brief, currentline);
    h_size++;
}
void show_history(int flag)
{
    if (flag == 0) // Option for BRIEF History
    {
        for (int i = 0; i < h_size; ++i)
        {
            // This is just a check to see if the last character of the string is line-feed.
            // For better visibility.
            if (History[i].brief && *(History[i].brief) && History[i].brief[strlen(History[i].brief) - 1] == '\n')
                printf("%d. %s", (i + 1), History[i].brief);
            else
                printf("%d. %s\n", (i + 1), History[i].brief);
        }
    }
    if (flag == 1) // Option for FULL History
    {
        for (int i = 0; i < h_size; ++i)
        {
            // This is also just a check to see if the last character of the string is line-feed.
            // For better visibility.
            if (History[i].full && *(History[i].full) && History[i].full[strlen(History[i].full) - 1] == '\n')
                printf("%d. %s", (i + 1), History[i].full);
            else
                printf("%d. %s\n", (i + 1), History[i].full);
        }
    }
}
char *replace(const char *s, const char *curr, const char *newW)
{ // to replace all occurences of substring
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
    result[i] = '\0';
    return result;
}

void replace_Char(char *str, char oldChar, char newChar) // replace all chars in a string
{
    int i = 0;
    while (str[i] != '\0')
    {
        /* If occurrence of character is found */
        if (str[i] == oldChar)
        {
            str[i] = newChar;
        }

        i++;
    }
}
extern void checkrunning() // checks for running child process
{
    int st;
    int *status;
    pid_t a;
    status = &st;
    a = waitpid(-1, status, WNOHANG);
    if (a > 0)
    {
        // PID = a
        printf("Process with Name: ");
        for (int i = 0; i < bg; ++i)
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
int foreGroundProcess(char *line[50], int len)
{
    //Execution of foreground process
    line[len] = NULL;
    pid_t pid;
    //pid_t wpid;
    int status;
    pid = fork();
    if (pid == 0)
    {
        // Child process

        if (execvp(line[0], line) == -1)
        {
            perror("ERROR");
        }
        exit(0);
    }
    else if (pid < 0)
    {
        // Error forking
        perror("lsh");
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
    line[len] = NULL;
    pid_t pid;
    int status;
    pid = fork();
    childpid = getpid();
    if (pid == 0)
    {
        // Child process
        // setpgid(pid,pgid)
        //pid =0 -> pid of calling process
        setpgid(0, 0); /*To make the pid of the current process it's pgid.*/
        if (execvp(line[0], line) == -1)
        {
            perror("ERROR");
        }
        exit(0);
    }
    else if (pid < 0)
    {
        // Error forking
        perror("lsh");
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
int getContext(char *line[50], int len)
{
    if (strcmp(line[len - 1], "&") == 0)
    {
        //Back Ground Process call
        line[len - 1] = NULL;
        return backGroundProcess(line, len - 1);
    }
    else
    {
        //ForeGround Process call
        childProcName = line[0];
        return foreGroundProcess(line, len);
    }
}

int get_args(char *list[50], int len)
{ // getting the args of ls command
    if (len == 1)
        return 0;
    int ret = 0;
    int i;
    for (i = 1; i < len; i++)
    {
        if (strcmp(list[i], "-l") == 0)
            ret += 2;
        else if (strcmp(list[i], "-a") == 0)
            ret += 1;
        else if (strcmp(list[i], "-la") == 0)
            return 3;
        else if (strcmp(list[i], "-al") == 0)
            return 3;
    }
    return ret;
}
void parse(char *currentline, char *argv[])
{
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
        {
            currentline[i] = '\0';
            break;
        }
        argv[j++] = &currentline[i];
        while ((currentline[i] != ' ') && (currentline[i] != EOF) && (currentline[i] != '\0') && (currentline[i] != '\n'))
        {
            i++;
        }
        if ((currentline[i] == EOF) || (currentline[i] == '\0') || (currentline[i] == '\n'))
        {
            currentline[i] = '\0';
            break;
        }
    }
    argv[j] = NULL; // Execvp Command Args are NULL terminated
}
void usr_def_execute(char *list[50], int len)
{
    // implementing inbuilt commands of the shell
    // These include the commands: cd,pwd,ls and pid <all/current>
    checkrunning();
    if (len == 0)
    {
        return;
    }
    if (strcmp("cd", list[0]) == 0) // caught cd command
    {
        if (len < 1) //error
        {
            printf("Enter the path after cd ,Usage -> cd <pathname>\n");
        }
        char *replceTilda = list[1];
        if (list[1][0] == '~')
        {
            replceTilda = replace(list[1], "~", currentWD);
        }
        if (chdir(replceTilda) == -1)
        {
            if (errno = ENOENT)
                printf("No dir called : %s\n", list[1]);
            else
            {
                printf("Error. Unable to open dir.");
            }
        }
    }
    else if (strcmp("pwd", list[0]) == 0) // caught pwd
    {
        char pwd[PATH_MAX];
        getcwd(pwd, sizeof(pwd));
        printf("%s\n", pwd);
    }
    else if (strcmp("ls", list[0]) == 0) // caught ls
    {
        struct dirent *de; // struct for file systems
        DIR *dr = NULL;
        int flg = 0;
        for (int i = 1; i < len; i++)
        {
            if (list[i][0] != '-' && list[i][0] != '&')
            {
                flg = 1;
                //char * path;
                dr = opendir(list[i]);
                break;
            }
        }
        if (flg == 0)
            dr = opendir(".");

        char *file[5000];
        if (dr == NULL)
        {
            if (errno = ENOENT) // raised when dir DNE
                perror("No such directory exists");
            else
                perror("Could not open file directory");
            return;
        }
        int noFile = 0;
        long int blksize = 0;
        int infoLvl = get_args(list, len); // getting the args of ls cmd
        while ((de = readdir(dr)) != NULL)
        {
            file[noFile++] = de->d_name;
            if (infoLvl == 0)
            {
                if (strcmp(".", de->d_name) != 0 && strcmp("..", de->d_name) != 0)
                    printf("%s ", file[noFile - 1]);
            }
            else if (infoLvl == 1)
                printf("%s ", file[noFile - 1]);

            else if ((infoLvl == 2 && strcmp(".", de->d_name) != 0 && strcmp("..", de->d_name) != 0) || infoLvl == 3)
            {
                struct stat fileStat;
                struct passwd *pwd;
                char date[12];
                int addrsI = getPath(list, len);
                if (addrsI > 0)
                {
                    char path1[100];
                    sprintf(path1, "%s%s", list[addrsI], file[noFile - 1]);
                    if (stat(path1, &fileStat) < 0)
                        return;
                }
                else
                {
                    if (stat(file[noFile - 1], &fileStat) < 0)
                        return;
                }
                //protection information
                printf((S_ISDIR(fileStat.st_mode)) ? "d" : "-");
                printf((fileStat.st_mode & S_IRUSR) ? "r" : "-");
                printf((fileStat.st_mode & S_IWUSR) ? "w" : "-");
                printf((fileStat.st_mode & S_IXUSR) ? "x" : "-");
                printf((fileStat.st_mode & S_IRGRP) ? "r" : "-");
                printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
                printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");
                printf((fileStat.st_mode & S_IROTH) ? "r" : "-");
                printf((fileStat.st_mode & S_IWOTH) ? "w" : "-");
                printf((fileStat.st_mode & S_IXOTH) ? "x" : "-");
                printf(" %ld ", fileStat.st_nlink);

                if ((pwd = getpwuid(fileStat.st_uid)) != NULL)
                    printf(" %-8.8s", pwd->pw_name);
                else
                    printf(" %-8d", fileStat.st_uid);

                if ((pwd = getpwuid(fileStat.st_gid)) != NULL)
                    printf(" %-8.8s", pwd->pw_name);
                else
                    printf(" %-8d", fileStat.st_gid);

                printf(" %8ld ", fileStat.st_size);
                strftime(date, 20, "%b %d %H:%M ", localtime(&(fileStat.st_mtime)));
                printf(" %s ", date);
                blksize += fileStat.st_blocks;
                printf("%s\n", file[noFile - 1]);
            }
        }
        if (infoLvl >= 2)
        {
            blksize /= 2;
            printf("total: %ld\n", blksize);
            blksize = 0;
        }
        closedir(dr);
    }
    else if (len == 1 && strcmp("pid", list[0]) == 0)
    { // Get our shells PID
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
    else if (len == 2 && strcmp("pid", list[0]) == 0 && strcmp("current", list[1]) == 0)
    { // Currently Executing/Stopped processes
        int status;
        int k = 1;
        printf("\nList of currently executing processes spawned from this shell:\n");
        for (int i = 0; i < bg; i++)
        {
            if (waitpid(bg_proc[i].pid, &status, WNOHANG) != 0)
            {
                continue;
            }
            else
            {
                printf("%d. Command: %10s PID: %d\n", (k++), bg_proc[i].name, bg_proc[i].pid);
            }
        }
        if (k == 1)
            printf("--- NO PROCESSES CURRENTLY RUNNING ---\n");
    }
    else if (len == 2 && strcmp("pid", list[0]) == 0 && strcmp("all", list[1]) == 0)
    { // All spawned processes
        if (all == 0)
        {
            printf("--- NO PROCESSES SPAWNED ---\n");
            return;
        }
        printf("\nList of all processes spawned from this shell:\n");
        for (int i = 0; i < all; i++)
        {
            printf("%d. Command: %15s PID: %d\n", (i + 1), all_proc[i].name, all_proc[i].pid);
        }
    }
    else
        getContext(list, len);
}
void tok_exec(char str[PATH_MAX])
{ // to get the arguments for exec and run inbuilt commands
    char *d = " \n";
    char *d1 = ";";
    char *p;
    char *p1;
    char *token;
    char *token1;
    token1 = strtok_r(str, d1, &p1);
    while (token1 != NULL)
    {
        char *args[50];
        int c = 0;
        token = strtok_r(token1, d, &p);
        while (token != NULL)
        {
            args[c++] = token;
            token = strtok_r(NULL, d, &p);
        }
        token1 = strtok_r(NULL, d1, &p1);
        usr_def_execute(args, c);
    }
}
void exec_from_hist(int idx) // execute a command from the maintained history
{
    checkrunning();
    char *cmd = malloc(sizeof(char) * 100);
    char cmd2[100];
    char *args[50];
    strcpy(cmd, History[idx].full);
    strcpy(cmd2, cmd);
    int id_for_hist = 0;
    parse(cmd, args);
    if (strcmp(args[0], "STOP") == 0)
    {
        if (args[1] == NULL)
        {
            printf("Exiting normally, Goodbye!\n");
            exit(0);
        }
        else
            printf("Error: Invalid Command\n");
    }
    else if (strcmp(args[0], "HISTORY") == 0)
    {
        if (args[1] == NULL)
        {
            printf("Error: Invalid Command");
            return;
        }

        if ((strcmp(args[1], "BRIEF") == 0) && (args[2] == NULL))
            show_history(0);

        else if ((strcmp(args[1], "FULL") == 0) && (args[2] == NULL))
            show_history(1);

        else
            printf("Error: Invalid Command\n");
    }
    else if (isdigit(*(args[0] + 4)) && args[1] == NULL)
    {
        int idx = atoi(args[0] + 4) - 1;
        if (idx == -1)
        {
            printf("Error: Invalid Command: Command number does not exist\n");
            return;
        }
        int k = MAX(h_size - idx, 0);
        int j = MIN(h_size, k + idx);
        int a = 1;
        for (int i = k; i < j; ++i)
        {
            if (History[i].full && *(History[i].full) && History[i].full[strlen(History[i].full) - 1] == '\n')
                printf("%d. %s", (a++), History[i].full);
            else
                printf("%d. %s\n", (a++), History[i].full);
        }
    }
    else if (isdigit(*(args[0] + 5)) && (*args[0] == '!') && args[1] == NULL)
    {
        id_for_hist = atoi(args[0] + 5) - 1;
        if ((id_for_hist > h_size) || (id_for_hist < 0))
        {
            printf("Error: Invalid Command: Command number does not exist\n");
        }
        else
        {
            exec_from_hist(id_for_hist);
        }
    }
    else if (strcmp(args[0], "EXEC") == 0)
    {
        addHistory(cmd2);
        if (args[1] == NULL)
            printf("Error: Invalid Command\n");
        else
        {
            char **cmd_args = &args[1];
            if (isdigit(**cmd_args) && args[2] == NULL)
            {
                int idx = atoi(args[1]);
                if ((idx > h_size) || (idx <= 0))
                {
                    printf("Error: Invalid Command; Command number does not exist\n");
                    return;
                }
                printf("\nExecuting Command: %s\n", History[idx - 1].full);
                exec_from_hist(idx - 1);
            }
            else if (isalpha(**cmd_args))
            {
                printf("\nExecuting Command: %s\n", &cmd2[5]);
                tok_exec(&cmd2[5]);
            }
            else
            {
                printf("Error: Invalid Command\n");
            }
        }
    }
    else
    {
        tok_exec(cmd2);
        printf("\n");
    }
    free(cmd);
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
    char *args;
    int id_for_hist = 0;
    while (1)
    {
        checkrunning();
        char *cmd = malloc(sizeof(char) * 100);
        char cmd2[100];
        char *args[50];
        getcwd(shellsWD, sizeof(shellsWD));
        char *cwd1 = shellsWD;
        if (strncmp(currentWD, shellsWD, strlen(currentWD)) == 0)
            cwd1 = replace(shellsWD, currentWD, "");
        // PROMPT
        printf("<%s@%s:~%s>", userName, host, cwd1);
        fgets(cmd, 100, stdin);
        strcpy(cmd2, cmd);
        parse(cmd, args);
        if (args[0] == NULL)
            continue;
        if (strcmp(args[0], "STOP") == 0)
        { // CAUGHT STOP CMD
            if (args[1] == NULL)
            {
                int status;
                for (int i = 0; i < bg; i++) // KILLING ALL STOPPED BACKGROUND PROCESSES
                {
                    if (waitpid(bg_proc[i].pid, &status, WNOHANG) != 0)
                    {
                        continue;
                    }
                    else
                    {
                        kill(bg_proc[i].pid, SIGKILL);
                    }
                }
                printf("Exiting normally, Bye!\n");
                exit(0);
            }
            else
                printf("Error: Invalid Command\n");
        }
        else if (strcmp(args[0], "HISTORY") == 0)
        { // CAUGHT COMMAND HISTORY
            if (args[1] == NULL)
            {
                printf("Error: Invalid Command");
                continue;
            }
            if ((strcmp(args[1], "BRIEF") == 0) && (args[2] == NULL))
                show_history(0); // FLAG=0 FOR BRIEF HISTORY

            else if ((strcmp(args[1], "FULL") == 0) && (args[2] == NULL))
                show_history(1); // FLAG =1 FOR FULL HISTORY

            else
                printf("Error: Invalid Command\n");
            addHistory(cmd2);
        }
        else if (isdigit(*(args[0] + 4)) && (*args[0] == 'h') && (*(args[0] + 1) == 'i') && (*(args[0] + 2) == 's') && (*(args[0] + 3) == 't') && args[1] == NULL)
        { // CAUGHT COMMAND ----- HISTN
            int idx = atoi(args[0] + 4) - 1;
            if (idx == -1)
            {
                printf("Error: Invalid Command: Command number does not exist\n");
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
            addHistory(cmd2);
        }
        else if (isdigit(*(args[0] + 5)) && (*(args[0] + 1) == 'h') && (*(args[0] + 2) == 'i') && (*(args[0] + 3) == 's') && (*(args[0] + 4) == 't') && (*args[0] == '!') && args[1] == NULL)
        { // CAUGHT COMMAND ----- !HISTN
            id_for_hist = atoi(args[0] + 5) - 1;
            if ((id_for_hist > h_size) || (id_for_hist < 0))
            {
                printf("Error: Invalid Command: Command number does not exist\n");
            }
            else
            {
                exec_from_hist(id_for_hist); // recursive function to execute commands from history
            }
            addHistory(cmd2);
        }
        else if (strcmp(args[0], "EXEC") == 0)
        { // CAUGHT COMMAND EXEC -- <CMD_NAME/CMD_IDX>
            addHistory(cmd2);
            if (args[1] == NULL)
                printf("Error: Invalid Command\n");
            else
            {
                char **cmd_args = &args[1];
                if (isdigit(**cmd_args) && args[2] == NULL) // EXEC N(idx)
                {
                    int idx = atoi(args[1]);
                    if ((idx > h_size) || (idx <= 0))
                    {
                        printf("Error: Invalid Command; Command number does not exist\n");
                        continue;
                    }
                    printf("\nExecuting Command: %s\n", History[idx - 1].full);
                    exec_from_hist(idx - 1);
                }
                else if (isalpha(**cmd_args)) // EXEC cmd
                {
                    printf("\nExecuting Command: %s\n", &cmd2[5]);
                    tok_exec(&cmd2[5]);
                    addHistory(&cmd2[5]);
                }
                else
                {
                    printf("Error: Invalid Command\n");
                }
            }
        }
        else
        { // ALL OTHER CASES (inbuilt, terminal AND pid commands)
            addHistory(cmd2);
            tok_exec(cmd2);
            printf("\n");
        }
        free(cmd); // freeing memory
    }
    return 0;
}
