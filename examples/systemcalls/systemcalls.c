// examples/systemcalls/systemcalls.c

#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>

/**
 * Führt einen Shell-Befehl mittels system() aus.
 */
bool do_system(const char *cmd)
{
    if (cmd == NULL) {
        return false;
    }

    int ret = system(cmd);
    if (ret == -1) {
        return false;
    }
    // system() gibt den Beendigungsstatus zurück in Form (exit_status << 8), ggf weitere bits
    // Hier prüfen wir, ob der Prozess normal beendet wurde und mit Status 0
    return WIFEXITED(ret) && WEXITSTATUS(ret) == 0;
}

/**
 * Führt execv aus. args[...] muss count Argumente enthalten und args[count] == NULL setzen.
 */
bool do_exec(int count, char *args[])
{
    if (count < 1 || args == NULL) {
        return false;
    }

    // args muss NULL-terminiert sein
    // Stelle sicher, dass args[count] == NULL
    args[count] = NULL;

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        return false;
    }
    if (pid == 0) {
        // Kindprozess
        execv(args[0], args);
        perror("execv failed");
        exit(EXIT_FAILURE);
    } else {
        // Elternprozess
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid failed");
            return false;
        }
        return WIFEXITED(status) && WEXITSTATUS(status) == 0;
    }
}

/**
 * Führt execv aus und leitet stdout auf outputfile um. args muss count Argumente + NULL-Terminator haben.
 */
bool do_exec_redirect(const char *outputfile, int count, char *args[])
{
    if (outputfile == NULL || count < 1 || args == NULL) {
        return false;
    }

    args[count] = NULL;  // NULL-terminierung

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        return false;
    }
    if (pid == 0) {
        // Kindprozess
        int fd = open(outputfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
            perror("open failed");
            exit(EXIT_FAILURE);
        }

        if (dup2(fd, STDOUT_FILENO) < 0) {
            perror("dup2 failed");
            close(fd);
            exit(EXIT_FAILURE);
        }
        close(fd);

        execv(args[0], args);
        perror("execv failed");
        exit(EXIT_FAILURE);
    } else {
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid failed");
            return false;
        }
        return WIFEXITED(status) && WEXITSTATUS(status) == 0;
    }
}

