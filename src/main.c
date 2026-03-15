#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    // Flush after every printf
    setbuf(stdout, NULL);

    char *builtin_commands[] = {"echo", "exit", "type", NULL};
    while (1) {
        printf("$ ");

        char command[256];
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "exit") == 0)
            break;
        else if (strncmp(command, "echo ", 5) == 0) {
            printf("%s\n", command + 5);
        } else if (strncmp(command, "type ", 5) == 0) {
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
                continue;
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
        } else {
            printf("%s: command not found\n", command);
        }
    }

    return 0;
}
