#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
            char *type = command + 5;

            int found = 0;
            for (int i = 0; builtin_commands[i] != NULL; ++i) {
                if (strcmp(builtin_commands[i], type) == 0) {
                    found = 1;
                    break;
                }
            }

            if (found)
                printf("%s is a shell builtin\n", type);
            else
                printf("%s: not found\n", type);
        } else {
            printf("%s: command not found\n", command);
        }
    }

    return 0;
}
