#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

extern char **environ;
static char **global_argv = NULL;

int setproctitle(const char *title)
{
    size_t size;
    int i;
    char *p;

    size = 0;
    for (i = 0; environ[i]; i++) {
        size += strlen(environ[i]) + 1;
    }

    p = malloc(sizeof(char) * size);
    if (NULL == p) {
        perror("malloc error");
        return 1;
    }

    for (i = 0; environ[i]; i++) {
        size = strlen(environ[i]) + 1;
        memcpy(p, environ[i], size);
        environ[i] = p;
        p += size;
    }

    strcpy(global_argv[0], "nginx: ");
    strcat(global_argv[0], title);
    return 0;
}

int main(int argc, char *argv[])
{
    pid_t pid;
    global_argv = argv;
    int i;

    setproctitle("master");

    pid = fork();
    if (pid < 0) {
        perror("fork error");
    } else if (pid == 0) {
	    for (i = 0; i < 10; i++) {
            pid = fork();
            if (pid < 0) {
                perror("fork error");
            } else if (pid == 0) {
                setproctitle("worker");
                while(1) {
                    sleep(1);
                }
            }
        }
        exit(0);
    }

    wait(NULL);

    while (1) {
        sleep(1);
    }

    return EXIT_SUCCESS;
}
