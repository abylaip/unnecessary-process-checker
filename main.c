#include <stdio.h>
#include <stdlib.h>
#include <sys/sysctl.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/proc_info.h>
#include <libproc.h>
#include "hash-table.h"

/*
    - I have used this function to check current running processes and 
    compare them with data.txt, if we have some process that is not in data.txt then print it
    It receives:
        1. size of buffer for all processes
        2. list of all processes
*/
void checkUnnecessaryProcesses(size_t size, struct kinfo_proc *procList) {
    FILE *file = fopen("data.txt", "r");
    if (!file) return;
    int procCount = (int)(size / sizeof(struct kinfo_proc));
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    HashMap *map = create_hashmap(procCount);

    // first we got all processes from data.txt and inserted into hashmap
    while ((read = getline(&line, &len, file)) != -1) {
        if (line[read - 1] == '\n') {  // remove `\n`
            line[read - 1] = '\0';
        }
        
        insert_hashmap(map, line, strdup("1"));
    }

    // we are going to get all current processes and check if we have them in hashmap
    for (int i = 0; i < procCount; i++) {
        char *value = get_hashmap(map, procList[i].kp_proc.p_comm);
        // if we dont have then print it
        if (value == NULL || strcmp(value, "1") != 0) {
            printf("Unnecesary process: %s\n", procList[i].kp_proc.p_comm);
        }
    }

    free_hashmap(map);
    if (line != NULL) {
        free(line);
    }
    fclose(file);
}

/* 
    - I have used this function to write all running processes into data.txt file
    It receives:
        1. size of buffer for all processes
        2. list of all processes
*/
void writeProcessNames(size_t size, struct kinfo_proc *procList) {
    FILE *file = fopen("data.txt", "w");
    if (!file) return;
    int procCount = (int)(size / sizeof(struct kinfo_proc));
    for (int i = 0; i < procCount; i++) {
        fprintf(file, "%s\n", procList[i].kp_proc.p_comm);
    }
    fclose(file);
}


void listProcesses() {
    int mib[3] = { CTL_KERN, KERN_PROC, KERN_PROC_ALL };
    size_t size;
    
    if (sysctl(mib, 3, NULL, &size, NULL, 0) == -1) {
        perror("sysctl error");
        return;
    }

    struct kinfo_proc *procList = malloc(size);
    if (!procList) {
        perror("malloc error");
        return;
    }

    if (sysctl(mib, 3, procList, &size, NULL, 0) == -1) {
        perror("sysctl error");
        free(procList);
        return;
    }

    // writeProcessNames(size, procList); // call if need to update data.txt
    checkUnnecessaryProcesses(size, procList);

    free(procList);
}

// main function
int main() {
    listProcesses();
    return 0;
}