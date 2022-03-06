#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "html.h"

typedef enum _BUFFER_TYPE {
	TAGNAME,
	ATTRIB,
	VALUE,
} BUFFER_TYPE;

// <parsing html>

/*
HTMLDoc parseHtml(const char *raw_html) {

	HTMLDoc document = {0};
	NODE root = *(document.root);

	const u64 length = strlen(raw_html);

	char current_buffer[100] = {0};
	BUFFER_TYPE current_buffer_type = TAGNAME;
	u64 count = 0;

	for (u64 i = strchr(raw_html, '<'); i < length; ++i)
	{
		// we need to ignore the whitespaces " \n\t\r" etc;
		char c = raw_html[i];
		
		switch (c)
		{
			case '<':
				// change current_buffer to tag buffer ? and make new tag;
				// tag_recording_starts
				break;
			
			case '>':
				// tag recording stops
				break;
			
			default:
				break;
		}

		// todo: change this line
		current_buffer[count++] = c;
	}

	return document;
}

// starting char must be <
NODE parse_tag(const char* html)
{
	if (*html != '<') return (NODE){0};
	
	NODE tag = {0}; // nah use alloc_node;

	char tagName[100] = "";
	int word_len = wordEnd(html++, tagName);
	printf("word: %s\n", tagName);
	
	html += word_len;

	// next char
	int over = 0;
	while(*(html) != '>')
	{
		html++;
		if(over++ > 1000) {
			fprintf(stderr, "[ERROR]: Unending pool, threshhold passed %s:%d\n", __FILE__, __LINE__);
			exit(1);
		}
	}



	// tag.value
	return tag;
}
*/

char *alloc_string(char *str, u64 len) {
	char *p = malloc(len + 1);
	strncpy(p, str, len);
	p[len] = (char)NULL;
	return p;
}

NODE parse_tag_rec(const char* tag, char** tagEnd)
{
	// get the name first
	if (*tag != '<') {
		fprintf(stderr, "Invalid Start of Tag, '<' expected %s:%d", __FILE__, __LINE__);
		exit(1);
	}

	// *tag == '<'
	tag += 1; 							// to get started on the tagname;
	char *name_start = tag;
	u64 name_len = 0;

	while(*tag != '>') {  // not parsing attributes currently
		name_len += 1;
		tag++;
	}

	char *name = alloc_string(name_start, name_len);

	// determine using a hash_table if its a single use tag or a tag that gets used in pair.

	// *tag == '>'
	const char const *addr_val_start = tag;
	bool tagClosed = false;
	
	const u64 MAX_BLOCK_COUNT = 10;
	BLOCK **blocks = malloc(MAX_BLOCK_COUNT * sizeof(BLOCK*));
	u64 blocks_count = 0;

	u64 children_count = 0;

	do {

		char *val_start = ++tag; // *tag == '>' thats why we increment by 1;
		u64 val_len = 0;

		while(*tag != '<') {
			val_len++;
			tag++;
		}

		// value block allocation.
		BLOCK *value_block = malloc(sizeof(BLOCK));
		value_block->type = STRING;
		value_block->value.string = alloc_string(val_start, val_len);;

		// adding the block to blocks array
		blocks[blocks_count++] = value_block;


		// PARSING FOUND TAG : value found partially, child tag or ending tag starts
		// *tag == '<'
		char *addr_tag_start = tag++;
		char *name_start = tag; // t = start of tagName '<' ;
		u64 name_len = 0;
		bool is_closing_tag = false;

		while(*tag != '>') {  // not parsing attributes currently
			if (*tag == '/') is_closing_tag = true;
			name_len += 1;
			tag++;
		}

		char *closing_tag_name = alloc_string(name_start, name_len);

		// compare the $opening_tag_name with this tagname.
		if (!is_closing_tag)
		{
			// then it must be an opening tag !
			char *addr_child_tag_end;
			NODE child_tag = parse_tag_rec(addr_tag_start, &addr_child_tag_end);
			tag = addr_child_tag_end;

			NODE *child_node = malloc(sizeof(NODE));
			memcpy(child_node, &child_tag, sizeof(child_tag));
			

			BLOCK *node_block = malloc(sizeof(BLOCK));
			node_block->type = TAG;
			node_block->value.tag = child_node;

			blocks[blocks_count++] = node_block;

			children_count++;

		} else {
			// check if name equals opening_tag
			tagClosed = true;
			// NAH SCRATCH THAT if not check if its a single occurence tag; THIS CHECK WILL BE DONE IN THE RECURSIVE CALL.
			// if not raise an error or ignore ig.
		}

	} while(!tagClosed);

	*tagEnd = tag; // setting pos to last value of tag. +1 maybe ?


	// char* accumulated_value; // loop over all blocks to get accumulated values of both, make a new function for that
	// char* accumulated_children;

	// NODE parsed_tag = {
	// 	.tagName = name,
	// 	.childrenCount = children_count,
	// 	.value = accumulated_value,
	// 	.children = accumulated_children,
	// 	.blocks = blocks,
	// 	.blocks_count = blocks_count
	// };
	// return parsed_tag;

}
// </parsing html>

NODE* alloc_node(char* tagName,	char* value, uint64_t childrenCount, NODE **children)
{
	NODE *node = malloc(sizeof(NODE));
	node->tagName = tagName;
	node->value = value;
	node->childrenCount = childrenCount;
	node->children = children;
	return node;
}

void free_nodes_rec(NODE *node)
{
	free(node->tagName);
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
}
