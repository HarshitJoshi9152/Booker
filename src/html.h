#include <stdbool.h>

#define u64 uint64_t
// print string limit
#define PS_LIMIT 100


enum VAL_TYPE {
	TAG,
	STRING
};

struct _node; 	// thats the magic of forward declarations !
union type_val {
	struct _node *tag;
	char *string;
};
typedef struct _block {
	enum VAL_TYPE type;
	union type_val value;
} BLOCK;

typedef struct _node {
	char* tagName;
	// HashTable attributes
	char* value;
	u64 childrenCount;
	struct _node **children;		// is this a linked list ? nah it cant be a linked list.
	BLOCK **blocks;							// array of pointers to blocks; double pointer !
	u64 blocks_count;
} NODE;

// ig we have a slight data duplication problem with .value and .blocks[i].string

typedef struct _htmldoc {
	// uint64_t tagCount;
	NODE *root;
	char* title;
} HTMLDoc;


NODE *alloc_node(char* tagName,	char* value, uint64_t childrenCount, NODE **children);
NODE *alloc_node_from_blocks(char* tagName, BLOCK **blocks, u64 blocks_count);
void print_node(NODE *node);
void free_nodes_rec(NODE *node);

// HTMLDoc parseHtml(const char *raw_html);
void free_HTML(HTMLDoc *doc);

NODE *parse_tag_rec(const char* tag, char** tagEnd);
bool is_void_tag(char *tag_name);


BLOCK *alloc_block(enum VAL_TYPE type, union type_val value);

void nodes_test();
void test();
