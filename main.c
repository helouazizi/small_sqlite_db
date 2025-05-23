#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db.h"

#define INPUT_SIZE 256

int main() {
    HashTable *db = create_table(128);
    char input[INPUT_SIZE];

    printf("Welcome to mydb > type commands like: SET key value, GET key, DEL key\n");

    while (1) {
        printf("> ");
        if (!fgets(input, INPUT_SIZE, stdin)) break;

        input[strcspn(input, "\n")] = 0; // remove newline

        char *cmd = strtok(input, " ");
        if (!cmd) continue;

        if (strcmp(cmd, "SET") == 0) {
            char *key = strtok(NULL, " ");
            char *value = strtok(NULL, "");
            if (key && value) {
                put(db, key, value);
                printf("OK\n");
            } else {
                printf("Usage: SET key value\n");
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char *key = strtok(NULL, " ");
            if (key) {
                char *value = get(db, key);
                if (value)
                    printf("%s\n", value);
                else
                    printf("NULL\n");
            } else {
                printf("Usage: GET key\n");
            }
        } else if (strcmp(cmd, "DEL") == 0) {
            char *key = strtok(NULL, " ");
            if (key) {
                deletee(db, key);
                printf("Deleted.\n");
            } else {
                printf("Usage: DEL key\n");
            }
        } else if (strcmp(cmd, "EXIT") == 0) {
            break;
        } else {
            printf("Unknown command. Try: SET, GET, DEL, EXIT\n");
        }
    }

    free_table(db);
    return 0;
}
