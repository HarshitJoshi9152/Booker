#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#define u64 uint16_t
// print string limit
#define PS_LIMIT 100

struct Point
{
   int x, y;
};
  
// compoundLiterals // Utility function to print point
// compoundLiterals void printPoint(struct Point p)
// compoundLiterals {
// compoundLiterals    printf("%d, %d", p.x, p.y);
// compoundLiterals }

// BETTER to store elmType as char*;
// typedef enum _elms {
// 	h1,
// 	h2,
// 	h3,
// 	h4,
// 	h5,
// 	h6,
// } ElmType;

typedef struct _node {
	char* elmType;
	// HashTable attributes
	char* value;
	uint64_t childrenCount;
	struct _node **children;		// is this a linked list ? nah it cant be a linked list.
} NODE;

typedef struct _htmldoc {
	// uint64_t tagCount;
	NODE *root;
} HTMLDoc;

void print_node(NODE *node);
HTMLDoc parseHtml(const char *raw_html);

// HTMLDoc parseHtml(const char *raw_html) {

// 	HTMLDoc document = {0};

// 	const u64 length = strlen(raw_html);
// 	char current_buffer[100] = {0};
// 	u64 count = 0;

// 	for (u64 i = 0; i < length; ++i)
// 	{
// 		char c = raw_html[i];
// 		switch (c)
// 		{
// 		case '<':
// 			// change current_buffer to tag buffer ? and make new tag;

// 			break;
		
// 		default:
// 			break;
// 		}
// 		current_buffer[count++] = c;
// 	}

// 	return document;
// }

NODE* make_node(char* elmType,	char* value, uint64_t childrenCount, NODE **children)
{
	NODE *node = malloc(sizeof(NODE));
	node->elmType = elmType;
	node->value = value;
	node->childrenCount = childrenCount;
	node->children = children;
	return node;
}


void test()
{

	HTMLDoc doc = {0};
	doc.root = malloc(sizeof(NODE));
	doc.root->elmType = "h2";
	doc.root->value = "structures dont get converted to pointers !";
	doc.root->childrenCount = 3;

	NODE **children = malloc(sizeof(NODE) * 3);

	// ig i cant do children[0]={"n2", "helloworld"}; coz structs dont get converted to struct pointers (unnamed objects) automatically like string literals.
	NODE n2 = {"n2", "helloworld"};
	NODE n1 = {"n1", "helloworld"};
	NODE n = {"n", "helloworld"};
	// children[0] = &n;
	// children[0] = (NODE *){"n", "helloworld"}; // doesnt work
	children[0] = &(NODE){"n", "helloworld"}; // works | compound literal
	children[1] = &n1;
	children[2] = &n2;

	// NODE **choldren = { 		AH i got it i should have done NODE *children[] = {};
	// 	&(NODE){0}, 
	// 	&(NODE){0}, 
	// 	&(NODE){0}
	// };
	// NODE **choldren = (NODE *[]){0, 0, 0};

	// NODE *n99 = make_node("1", "helloworld$", 0, NULL);
	// NODE *n99 = {"1", "helloworld$", 0, NULL}; // doesnt work 
	NODE *n99 = &(NODE){"1", "helloworld$", 0, NULL}; // but this works // annonymois object
	NODE *n98 = make_node("2", "helloworld$", 0, NULL); // todo learn more about differences in auto, static, global
	NODE *n97 = make_node("3", "helloworld$", 0, NULL);

	char *name = "harshit joshi";

	NODE **choldren = (NODE *[]){n99, n98, n97}; // hmm this works;
	// printf("%s\n", n.value);
	// printf("%s\n", doc.root->value);

	NODE *new_children[] = {
		&(NODE){.elmType="blackJack",.value="ncie des yo",.childrenCount=0, .children=NULL},
		&(NODE){.elmType="blackJack",.value="ncie des yo",.childrenCount=0, .children=NULL},
		&(NODE){.elmType="blackJack",.value="ncie des yo",.childrenCount=0, .children=NULL},
		&(NODE){.elmType="blackJack",.value="ncie des yo",.childrenCount=0, .children=NULL},
	};

	doc.root->children = choldren;
	print_node(doc.root);

	// print_node(n99);
	// print_node(n98);
	// print_node(n97);
	// free(n99);
	free(children);

	// compoundLiterals printPoint((struct Point){2, 3});

	// struct Point p = {.x=9, .y=2};
	// struct Point s = p; // works
	// struct Point* listOfLists[] = {
	// 	&(struct Point){.x=90,.y=90},
	// 	&s,&p
	// };

	// printf("\n%d\n", listOfLists[0]->x);

	// char name[100] = "Harshit Joshi";
	// strcat(name, "is a very good boy\n");
	// printf(name);
	// char n[100] = "nice";
	// sprintf(n, "%d", 12);
	// printf("%s\n", n);
}


void free_nodes_rec(NODE *node)
{
	free(node->elmType);
	free(node->value);

	// depth first ?
	for(u64 i = 0; i < node->childrenCount; ++i)
	{
		free_nodes_rec(node->children[i]);
	}
}

void free_HTML(HTMLDoc *doc)
{
	NODE *root = doc->root;
	// free nodes recursively;
	free_nodes_rec(root);
}

void print_node(NODE *node)
{
	printf("_node {\n");
	printf("\telmType: %s\n", node->elmType);
	printf("\tvalue: %s\n", node->value);
	printf("\tcc: %ld\n", node->childrenCount);

	NODE **children = node->children;
	u64 childrenCount = node->childrenCount;

	if (children == NULL || childrenCount == (intptr_t)NULL) {
		// childrenCount = 0;
			printf("};\n");
		return;
	}

	char childrenTagsString[PS_LIMIT] = "";
	const char* delm = ", ";

	for(u64 i = 0; i < childrenCount; ++i)
	{
		NODE *child = children[i];

		if (strlen(childrenTagsString) + strlen(child->elmType) + strlen(delm) + 1 <= sizeof(childrenTagsString))
		{
			strcat(childrenTagsString, child->elmType);

			if (i != childrenCount - 1)
			{
				strcat(childrenTagsString, delm);
				// hmm so we dont technically need to include strlen(delm) in the last sizeof() calc.
			}
		}
		else {
			// Error Reporting
			fprintf(stderr, "\x1b[91m");
			fprintf(stderr,"[ERROR] Not enough space in childrenTagsString %s:%d\n", __FILE__, __LINE__);
			fprintf(stderr, "\tstrlen(childrenTagsString) : %ld\n", strlen(childrenTagsString));
			fprintf(stderr, "\tstrlen(child->elmType) : %ld\n", strlen(child->elmType));
			fprintf(stderr, "\tstrlen(delm) : %ld\n", strlen(delm));
			fprintf(stderr, "\tsizeof(childrenTagsString) : %ld\n", sizeof(childrenTagsString));
			fprintf(stderr, "\x1b[0m");
			break;
		}		
	}
	
	printf("\tchildren: [%s]\n", childrenTagsString);
	printf("};\n");

}

