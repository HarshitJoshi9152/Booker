
#define HT_SIZE 100

typedef struct entry_s {
	char *key;
	char *value;
} entry_t;

typedef struct hashtable_s {
	uint64_t size;
	struct entry_s **entries;
} hashtable_t;

void test_hash();

bool ht_init();
bool ht_free();

entry_t *alloc_entry(char *key, char *value); // should i make it private by not including it in the header ??
bool free_entry(entry_t *entry);

uint64_t ht_hash(char* key);

entry_t *ht_insert(char *key, char *value);
bool ht_remove(char *key);
char *ht_get(char *key);