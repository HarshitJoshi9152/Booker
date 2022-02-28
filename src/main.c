#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>


// #define PATH "C:\\Users\\Harshit\\code\\projects\\Booker\\res\\bookmarks.html"
#define PATH "/mnt/c/Users/Harshit/code/projects/Booker/res/bookmarks.html"


char* readFile(const char *path)
{
	FILE *fd = fopen(path, "r");
	if (fd == NULL)
	{
		fprintf(stderr, "Error opening file: %s\n", strerror( errno ));
		exit(EXIT_FAILURE);
	}
	
	fseek(fd, 0, SEEK_END);
	uint64_t size = ftell(fd);
	rewind(fd);

	printf("size is %ld\n", size);

	char *contents = malloc(size * sizeof(char) + 1); // 1 for NULL char
	fread(contents, sizeof(char), size, fd);
	contents[size] = 0;

	fclose(fd);
	return contents;
}


int main(int argc, char *argv[])
{
	printf("arg count is %d\n", argc);
	char* file_contents = readFile(PATH);
	printf("---------------FILE-START------------\n");
	printf("%s\n", file_contents);
	printf("---------------FILE-END------------\n");
	free(file_contents);
	return EXIT_SUCCESS;
}