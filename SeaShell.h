struct c_des // Struct for maintaining command HISTORY
{
    char *brief;
    char *full;
};
struct p_info // Struct for maintaining PID records
{
    char *name;
    int pid;
};

void exec_bat(char *file);

void master_execute(char *args[50], char *cmd2, int len, int firstCall);

static void pipelineio_exec(char *full, char ***cmd, char *ip, char *op);

void handle_sigint(int sig); // SIGNAL HANDLER FOR INTERRUPT HANDLING

int foreGroundProcess(char *line[50], int len);

int backGroundProcess(char *line[50], int len);

int getContext(char *line[50], int len); /*to get context of execution: foreground or background (&)*/

int parse(char *currentline, char *argv[]); /*generic routine to parse/tokenize the input command by user*/

void addHistory(char *entry); /* for adding record to history*/

void show_history(int flag); /* showing history --brief and --full */

char *replace(const char *s, const char *curr, const char *newW);

extern void checkrunning(); // checks for running child process