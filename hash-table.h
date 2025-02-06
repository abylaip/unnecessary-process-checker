#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 16

typedef struct Entry {
    char *key;
    char *value;
    struct Entry *next;
} Entry;

typedef struct HashMap {
    Entry **buckets;
    int capacity;  
    int size;      
} HashMap;

unsigned int hash(const char *key) {
    unsigned int hash = 5381;
    int c;

    while ((c = *key++))
        hash = ((hash << 5) + hash) + c;

    return hash;
}

HashMap *create_hashmap(int capacity) {
    if (capacity <= 0) {
        capacity = INITIAL_CAPACITY; 
    }

    HashMap *map = (HashMap *)malloc(sizeof(HashMap));
    if (map == NULL) {
        perror("malloc failed");
        exit(1);
    }

    map->capacity = capacity;
    map->size = 0;
    map->buckets = (Entry **)calloc(capacity, sizeof(Entry *));
    if (map->buckets == NULL) {
        perror("calloc failed");
        free(map);
        exit(1);
    }

    return map;
}

void resize_hashmap(HashMap *map) {
    int new_capacity = map->capacity * 2;
    Entry **new_buckets = (Entry **)calloc(new_capacity, sizeof(Entry *));
    if (new_buckets == NULL) {
        perror("calloc failed during resizing");
        exit(1);
    }

    for (int i = 0; i < map->capacity; i++) {
        Entry *current = map->buckets[i];
        while (current != NULL) {
            unsigned int new_index = hash(current->key) % new_capacity;
            Entry *next = current->next;
            current->next = new_buckets[new_index];
            new_buckets[new_index] = current;
            current = next;
        }
    }

    free(map->buckets);
    map->buckets = new_buckets;
    map->capacity = new_capacity;
}

void insert_hashmap(HashMap *map, const char *key, const char *value) {
    if (map->size >= map->capacity * 0.75) {
        resize_hashmap(map);
    }

    unsigned int index = hash(key) % map->capacity;
    Entry *current = map->buckets[index];

    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            free(current->value);
            current->value = strdup(value);
            return;
        }
        current = current->next;
    }

    Entry *new_entry = (Entry *)malloc(sizeof(Entry));
    if (new_entry == NULL) {
        perror("malloc failed");
        exit(1);
    }

    new_entry->key = strdup(key);
    new_entry->value = strdup(value);
    new_entry->next = map->buckets[index];
    map->buckets[index] = new_entry;
    map->size++;
}

char *get_hashmap(HashMap *map, const char *key) {
    unsigned int index = hash(key) % map->capacity;

    Entry *current = map->buckets[index];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
        current = current->next;
    }

    return NULL;
}

void delete_hashmap(HashMap *map, const char *key) {
    unsigned int index = hash(key) % map->capacity;

    Entry *current = map->buckets[index];
    Entry *prev = NULL;

    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            if (prev == NULL) {
                map->buckets[index] = current->next;
            } else {
                prev->next = current->next;
            }
            free(current->key);
            free(current->value);
            free(current);
            map->size--;
            return;
        }
        prev = current;
        current = current->next;
    }
}

void free_hashmap(HashMap *map) {
    for (int i = 0; i < map->capacity; i++) {
        Entry *current = map->buckets[i];
        while (current != NULL) {
            Entry *next = current->next;
            free(current->key);
            free(current->value);
            free(current);
            current = next;
        }
    }
    free(map->buckets);
    free(map);
}

void print_hashmap(HashMap *map) {
    for (int i = 0; i < map->capacity; i++) {
        Entry *current = map->buckets[i];
        while (current != NULL) {
            printf("Key: %s, Value: %s\n", current->key, current->value);
            current = current->next;
        }
    }
}

char **get_all_values(HashMap *map, int *num_values) {
    char **values = NULL;
    *num_values = 0;

    for (int i = 0; i < map->capacity; i++) {
        Entry *current = map->buckets[i];
        while (current != NULL) {
            values = realloc(values, (*num_values + 1) * sizeof(char *));
            if (values == NULL) {
                perror("realloc failed");
                exit(1);
            }
            values[*num_values] = strdup(current->value);
            (*num_values)++;
            current = current->next;
        }
    }

    return values;
}

#endif 
