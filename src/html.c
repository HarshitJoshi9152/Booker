#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "html.h"

const char* void_tags[] = {
	"area",
	"base",
	"basefont",
	"bgsound",
	"br",
	"col",
	"command",
	"embed",
	"frame",
	"hr",
	"image",
	"img",
	"input",
	"isindex",
	"keygen",
	"link",
	"menuitem",
	"meta",
	"nextid",
	"param",
	"source",
	"track",
	"wbr"
};
const void_tags_len = sizeof(void_tags)/sizeof(void_tags[0]);

const u64 MAX_BLOCK_COUNT = 10;

// <parsing html>

char *alloc_string(char *str, u64 len) {
	char *p = malloc(len + 1);
	strncpy(p, str, len);
	p[len] = (char)NULL;
	return p;
}

bool is_void_tag(char *tag_name)
{
	for (u64 i = 0; i < void_tags_len; ++i)
	{
		if (!strcmp(tag_name, void_tags[i])) {
			return true;
		}
	}
	return false;
}

// tag starts at char '<' && char *tagEnd should point to '>'
NODE* parse_tag_rec(const char* tag, char** tagEnd)
{
	char* cursor = tag;
	// get the opening_tag_name first
	if (*cursor != '<') {
		fprintf(stderr, "Invalid Start of Tag, '<' expected %s:%d", __FILE__, __LINE__);
		exit(1);
	}

	// *cursor == '<'
	cursor += 1; 							// to get started on the tagname;
	char *name_start = cursor;
	u64 name_len = 0;

	while(*cursor != '>') {  // not parsing attributes currently
		name_len += 1;
		cursor++;
	}

	char *opening_tag_name = alloc_string(name_start, name_len);

	// determine using a hash_table if its a singleton and return immediately if it is .
	if (is_void_tag(opening_tag_name))
	{
		NODE *n = alloc_node(opening_tag_name, NULL, NULL, NULL);
		*tagEnd = cursor;
		return n;
	}

	// *cursor == '>'
	const char const *addr_val_start = cursor;
	bool tagClosed = false;

	BLOCK **blocks = malloc(MAX_BLOCK_COUNT * sizeof(BLOCK*));
	u64 blocks_count = 0;

	u64 children_count = 0;

	do {

		char *val_start = ++cursor; // *cursor == '>' thats why we increment by 1;
		u64 val_len = 0;

		while(*cursor != '<') {
			val_len++;
			cursor++;
		}

		// value block allocation.
		char* value =  alloc_string(val_start, val_len);
		BLOCK *value_block = alloc_block(STRING, (union type_val){value});
		blocks[blocks_count++] = value_block;

		// CHILD TAG OR ENDING TAG STARTS
		// *cursor == '<'
		char *addr_tag_start = cursor++;
		bool is_closing_tag = false;

		while(*cursor == '/' || isspace(*cursor))
		{
			if (*cursor == '/') is_closing_tag = true;
			cursor++;
		}

		// *cursor == '(first alpha char !)';
		char *name_start = cursor; // first char of tagname
		u64 name_len = 0;

		while(*cursor != '>') {  // not parsing attributes currently
			name_len += 1;
			cursor++;
		}
		// *cursor == '>' now, which is exactly what we need it to be for going in the next iteration of the loop.
		// todo: remove
		// printf("\nCALLED %p %c %s %d\n", cursor, *cursor, closing_tag_name, is_closing_tag);

		// compare the $opening_tag_name with this tagname.
		if (!is_closing_tag)
		{
			// then it must be an opening tag !
			char *addr_child_tag_end;
			NODE *child_tag = parse_tag_rec(addr_tag_start, &addr_child_tag_end);
			cursor = addr_child_tag_end;	// jumping to the end of the childtag !

			BLOCK *node_block = alloc_block(TAG, (union type_val){child_tag});
			blocks[blocks_count++] = node_block;

			children_count++;

		}
		else
		{
			// CLOSING/VOID tag !
			char *closing_tag_name = alloc_string(name_start, name_len);

			// check if its actually the matching CLOSING TAG for the opening tag.
			if (!strcmp(closing_tag_name, opening_tag_name))
			{
				tagClosed = true;
				free(closing_tag_name);
			}
			else
			{
				// VOID TAG
				bool is_void = is_void_tag(closing_tag_name);
				// todo: much better to alloc/make the tag here only ! coz its a singelton with '/'
				// and we will have to work again to seperate '/' and other ws chars from it !

				if (is_void_tag) {
					NODE *child_tag = alloc_node(closing_tag_name, NULL, NULL, NULL); // closing_tag_name NEVER GOT ALLOCED !
					BLOCK *node_block = alloc_block(TAG, (union type_val){child_tag});

					blocks[blocks_count++] = node_block;
					children_count++;
				} else {
					// it is not a void tag but has a '/' inside it ???
				}
			}
		}

	} while(!tagClosed);

	*tagEnd = cursor; // setting pos to last value of cursor. +1 maybe ?

	NODE *parsed_tag = alloc_node_from_blocks(opening_tag_name, blocks, blocks_count);
	return parsed_tag;

}

NODE *alloc_node_from_blocks(char* tagName, BLOCK **blocks, u64 blocks_count)
{
	u64 children_n = 0;
	u64 val_len = 0;

	// determining the length of val string and number of children nodes.
	for (u64 block_i = 0; block_i < blocks_count; ++block_i)
	{
		BLOCK *block = blocks[block_i];
		enum VAL_TYPE type = block->type;

		switch (type)
		{
			case TAG:
				children_n++;
				break;

			case STRING:
				val_len += strlen(block->value.string);
				break;
			
			default:
				break;
		}
	}

	// can i do ...
	// *node = (NODE){.tagName, ...}; // ???

	char *val = calloc(val_len + 1, sizeof(char)); // 1 for NULL char;
	NODE **children = malloc(sizeof(NODE*) * children_n);
	u64 children_i = 0;

	for (u64 block_i = 0; block_i < blocks_count; ++block_i)
	{
		BLOCK *block = blocks[block_i];
		enum VAL_TYPE type = block->type;

		switch (type)
		{
			case TAG:
				children[children_i] = block->value.tag;
				break;

			case STRING:
				strcat(val, block->value.string);
				break;
			
			default:
				break;
		}
	}

	NODE *node = malloc(sizeof(NODE));
	node->tagName = tagName;
	node->blocks = blocks;
	node->blocks_count = blocks_count;
	node->childrenCount = children_n;
	node->value = val;
	node->children = children;

	return node;
}

BLOCK *alloc_block(enum VAL_TYPE type, union type_val value) {
	BLOCK *block = malloc(sizeof(BLOCK));
	block->type = type;
	if (type == STRING) block->value.string = value.string;
	if (type == TAG) block->value.tag = value.tag;
	return block;
}

// </parsing html>

NODE* alloc_node(char* tagName,	char* value, uint64_t childrenCount, NODE **children)
{
	NODE *node = malloc(sizeof(NODE));
	node->tagName = tagName;
	node->value = value;
	node->childrenCount = childrenCount;
	node->children = children;
	node->blocks_count = 0;
	node->blocks = NULL;
	return node;
}

void free_nodes_rec(NODE *node)
{
	// printf("Why does it fail !\n");
	free(node->tagName);
	// printf("It doesnt !\n");
	if (node->value != NULL) {
		free(node->value);
	}

	// depth first ?
	if (node->children != NULL) {
		for(u64 i = 0; i < node->childrenCount; ++i)
		{
			free_nodes_rec(node->children[i]);
		}
		free(node->children);		// i know these are NULLS !
	}

	if (node->blocks != NULL) {
		for (u64 i = 0; i < node->blocks_count; ++i)
		{
			if (node->blocks[i]->type == STRING) free(node->blocks[i]->value.string);
			// the .tag in type TAG will free themselves in the last free(node) call.
			free(node->blocks[i]);
		}
		free(node->blocks);		// i know these are NULLS !
	}

	free(node);
}

void free_HTML(HTMLDoc *doc)
{
	NODE *root = doc->root;
	// free nodes recursively;
	free_nodes_rec(root);
	free(root);
}

void print_node(NODE *node)
{
	printf("_node {\n");
	printf("\telmType: %s\n", node->tagName);
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

		if (strlen(childrenTagsString) + strlen(child->tagName) + strlen(delm) + 1 <= sizeof(childrenTagsString))
		{
			strcat(childrenTagsString, child->tagName);

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
			fprintf(stderr, "\tstrlen(child->tagName) : %ld\n", strlen(child->tagName));
			fprintf(stderr, "\tstrlen(delm) : %ld\n", strlen(delm));
			fprintf(stderr, "\tsizeof(childrenTagsString) : %ld\n", sizeof(childrenTagsString));
			fprintf(stderr, "\x1b[0m");
			break;
		}		
	}
	
	printf("\tchildren: [%s]\n", childrenTagsString);
	printf("};\n");

}


void nodes_test()
{

	HTMLDoc doc = {0};
	doc.root = malloc(sizeof(NODE));
	doc.root->tagName = "h2";
	doc.root->value = "structures dont get converted to pointers !";
	doc.root->childrenCount = 4;

	NODE **children = malloc(sizeof(NODE) * 3);

	children[0] = &(NODE){"n", "helloworld"}; // works | compound literal
	children[1] = &(NODE){"n2", "helloworld"};
	children[2] = &(NODE){"n1", "helloworld"};

	NODE *new_children[] = {
		&(NODE){.tagName="blackJack",.value="ncie des yo",.childrenCount=0, .children=NULL},
		&(NODE){.tagName="blackJack",.value="ncie des yo",.childrenCount=0, .children=NULL},
		&(NODE){.tagName="blackJack",.value="ncie des yo",.childrenCount=0, .children=NULL},
		&(NODE){.tagName="blackJack",.value="ncie des yo",.childrenCount=0, .children=NULL}
	};

	doc.root->children = new_children;
	print_node(doc.root);
	free(children);
	// free_HTML(&doc); // only use when root and all nodes .tagName and .value are pointers to dynamically allocated memory locations,
									 // and this clears the all document NODES recursively.
	free(doc.root);
}

void test() {
	const char* demo_html = "\t<!DOCTYPE html> \n\
	<html>\n\
	<body>\n\
	\n\
	<h1>My First Heading</h1>\n\
	<p>My first paragraph.</p>\n\
	\n\
	</body>\n\
	</html>";

	printf("%s\n", demo_html);

	// HTMLDoc doc = parseHtml(demo_html);

	// print_node(doc.root);
	// bool h = is_void_tag("hr");
	// printf("is VOID : %d\n", h);

	char *p;
	NODE *html = parse_tag_rec("<h1>Hello <p> this si the ultimate test ! </br>  </p>World !</h1>", &p);

	print_node(html);
	print_node(html->children[0]->children[0]);
	free_nodes_rec(html);
}
