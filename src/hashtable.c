#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "hashtable.h"

hashtable_t *hashtable = {0};

// bool ht_init(hashtable_t *ht) {
bool ht_init() {
	hashtable = calloc(sizeof(hashtable_t), 1);
	hashtable->entries = calloc(sizeof(entry_t), HT_SIZE);
	// should bool mark any error ?
}

bool ht_free() {
	for (uint64_t i = 0; i < hashtable->size; ++i)
	{
		free_entry(hashtable->entries[i]);
	}
	free(hashtable->entries);
	free(hashtable);

	// should bool mark any error ?
}

entry_t *alloc_entry(char *key, char *value) {
	entry_t *entry = calloc(sizeof(entry_t), 1);
	entry->key = strdup(key);
	entry->value = strdup(value);
	return entry;
}

bool free_entry(entry_t *entry) {
	free(entry->key);
	free(entry->value);
	free(entry);
	// should bool mark any error ?
}

uint64_t ht_hash(char* key) {
	uint64_t hashval = 0;
	int i = 0;

	/* Convert our string to an integer */
	while( hashval < ULONG_MAX && i < strlen( key ) ) {
		hashval = hashval << 8;
		hashval += key[ i ];
		i++;
	}

	return hashval % HT_SIZE;
}

entry_t *ht_insert(char *key, char *value) {
	uint64_t index = ht_hash(key);
	entry_t *entry = alloc_entry(key, value);

	bool index_occupied = (ht_get(key) != (void*)NULL);

	if (index_occupied) {
		free(entry);
		return NULL;
	}

	hashtable->entries[index] = entry;

	return entry;
}

bool ht_remove(char *key) {
	uint64_t index = ht_hash(key);
	entry_t *entry_addr = hashtable->entries[index];
	
	if (entry_addr == NULL) return false;
	else {
		free_entry(entry_addr);
		hashtable->entries[index] = (entry_t*)NULL;
		return true;
	}
}

char *ht_get(char *key) {
	uint64_t index = ht_hash(key);
	entry_t *entry_addr = hashtable->entries[index];
	// we can have an array to store the allocated indexes/addresses to check if an index/address has been allocated
	return (entry_addr == NULL) ? NULL : entry_addr->value;
}
// when line 3 gets executed `entry_addr` is `0x8007b00` but

void test_hash() {
	printf("ncie !\n");
	ht_init();
	ht_insert("harshit", "is a good boy !\n");
	char *rtn = ht_get("harshit"); 		// this is an example where rtn is a bad var, coz the val its pointing to value gets changed in ht_remove
	bool removed = ht_remove("harshit");
	// rtn = ht_get("harshit");
	if (rtn == NULL || removed) {
		printf("The result was NULl !\n");
	} else {
		printf("%s", rtn);
	}
	ht_free();
}