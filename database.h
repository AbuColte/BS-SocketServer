#include <stdio.h>
#include <string.h>

#define MAX_PAIRS 100
#define MAX_KEY_LEN 50
#define MAX_VALUE_LEN 100
typedef struct {
    char key[MAX_KEY_LEN];
    char value[MAX_VALUE_LEN];
} KeyValuePair;

KeyValuePair store[MAX_PAIRS];
int storeSize = 0;

void trim_newline(char *str) {
    str[strcspn(str, "\r\n")] = 0;
}

int put(const char *key, const char *value) {
    // check if
    for (int i = 0; i < storeSize; ++i) {
        if (strcmp(store[i].key, key) == 0) {
            strncpy(store[i].value, value, MAX_VALUE_LEN);
            return 2;
        }
    }

    // Add new key and val
    if (storeSize < MAX_PAIRS) {
        strncpy(store[storeSize].key, key, MAX_KEY_LEN);
        strncpy(store[storeSize].value, value, MAX_VALUE_LEN);
        storeSize++;
    } else {
        printf("Store full!\n");
    }
}

// Get value by key
int get(const char *key) {
    char tempString[1024];
    printf("GET Function accessed \n");
    for (int i = 0; i < strlen(key) - 1; ++i) {
        printf("%c\n", key[i]);
        tempString[i] = key[i];
    }
    for (int i = 0; i < storeSize; i++) {
        if (strcmp(store[i].key, tempString) == 0) {
            printf("Found!\n");
            printf("Value : %s\n", store[i].value);
            printf("Key : %s\n", store[i].key);
            printf("StoreIndex : %d\n", i);
            return 1;
        }
    }
    printf("Not Found-\n");
    return 0;
}
// delete bny key
int del(const char *key) {
    char trimmedKey[MAX_KEY_LEN];
    strncpy(trimmedKey, key, MAX_KEY_LEN - 1);
    trimmedKey[MAX_KEY_LEN - 1] = '\0';
    trim_newline(trimmedKey);

    for (int i = 0; i < storeSize; i++) {
        if (strcmp(store[i].key, trimmedKey) == 0) {
            for (int j = i; j < storeSize - 1; j++) {
                store[j] = store[j + 1];
            }
            storeSize--;
            return 1;  // Deleted
        }
    }
    return 0;  // Not found
}
// typedef struct {
//     char key[MAX_KEY_LEN];
//     char value[MAX_VALUE_LEN];
// } KeyValuePair;
//
// KeyValuePair store[MAX_PAIRS];
// int storeSize = 0;
//
// // Set key-value
// inline void put(const char *key, const char *value) {
//     // Check if key already exists; if so, update
//     for (int i = 0; i < storeSize; ++i) {
//         if (strcmp(store[i].key, key) == 0) {
//             strncpy(store[i].value, value, MAX_VALUE_LEN);
//             return;
//         }
//     }
//
//     // Add new key-value pair
//     if (storeSize < MAX_PAIRS) {
//         strncpy(store[storeSize].key, key, MAX_KEY_LEN);
//         strncpy(store[storeSize].value, value, MAX_VALUE_LEN);
//         storeSize++;
//     } else {
//         printf("Store full!\n");
//     }
// }
//
// // Get value by key
// inline const char* get(const char *key) {
//     for (int i = 0; i < storeSize; ++i) {
//         if (strcmp(store[i].key, key) == 0) {
//             return store[i].value;
//         }
//     }
//     return NULL;
// }