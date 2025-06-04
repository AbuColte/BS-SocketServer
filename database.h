#include <stdio.h>
#include <string.h>

#define MAX_PAIRS 100
#define MAX_KEY_LEN 50
#define MAX_VALUE_LEN 100

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