#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_ARGS 100

void parse_command(char *command, char **args) {
    char *token = strtok(command, " ");

    int i = 0;
    while (token != NULL && i < MAX_ARGS - 1) {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;
}

void execute_command(char **args) {
    pid_t child = fork();

    if (child == 0) {
        execvp(args[0], args);
        printf("%s: command not found\n", args[0]);
        exit(EXIT_FAILURE);
    } else if (child > 0) {
        wait(NULL);
    }
}

int handle_builtin(char *command, char **builtin_commands) {
    if (strcmp(command, "exit") == 0)
        exit(EXIT_SUCCESS);

    if (strncmp(command, "echo ", 5) == 0) {
        printf("%s\n", command + 5);
        return 1;
    }

    if (strncmp(command, "type ", 5) == 0) {
        char *type_cmd = command + 5;

        int is_builtin = 0;
        for (int i = 0; builtin_commands[i] != NULL; ++i) {
            if (strcmp(builtin_commands[i], type_cmd) == 0) {
                is_builtin = 1;
                break;
            }
        }

        if (is_builtin) {
            printf("%s is a shell builtin\n", type_cmd);
            return 1;
        }

        char full_path[256];
        int found = 0;

        char *path_env = getenv("PATH");
        if (path_env != NULL) {
            char *path_copy = strdup(path_env);
            char *dir = strtok(path_copy, ":");

            while (dir != NULL) {
                snprintf(full_path, sizeof(full_path), "%s/%s", dir, type_cmd);

                if (access(full_path, X_OK) == 0) {
                    found = 1;
                    printf("%s is %s\n", type_cmd, full_path);
                    break;
                }
                dir = strtok(NULL, ":");
            }
            free(path_copy);
        }

        if (!found)
            printf("%s: not found\n", type_cmd);

        return 1;
    }

    return 0;
}

int main(int argc, char *argv[]) {
    // Flush after every printf
    setbuf(stdout, NULL);

    char *builtin_commands[] = {"echo", "exit", "type", NULL};
    while (1) {
        printf("$ ");
        char command[256];
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = '\0';

        if (handle_builtin(command, builtin_commands))
            continue;

        char *args[MAX_ARGS];
        parse_command(command, args);
        execute_command(args);
    }

    return 0;
}
