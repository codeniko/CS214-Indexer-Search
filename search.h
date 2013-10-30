#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>

#define BUFFER_SIZE	2000
#define QUERY_AND	0
#define QUERY_OR	1

typedef struct FileNode FileNode;
struct FileNode
{
	char *file;
	int count;
	FileNode *next;
};

typedef struct WordNode WordNode;
struct WordNode
{
	char *word;
	WordNode *next;
	FileNode *fnHead;
};

typedef struct SearchWordNode SearchWordNode;
struct SearchWordNode
{
	char *word;
	SearchWordNode next;
};

static WordNode *head = NULL;


void printHelp();
void clean();
WordNode *readIndexFile(char *path);
