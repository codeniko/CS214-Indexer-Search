#ifndef ___search_h___
#define ___search_h___

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>
#include <ctype.h>

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
	SearchWordNode *next;
};


void printHelp();
void clean(WordNode *head);
void insertIntoLL(SearchWordNode **head, SearchWordNode *node);
SearchWordNode *queryOr(WordNode *head, SearchWordNode *sWordHead);
SearchWordNode *queryAnd(WordNode *head, SearchWordNode *sWord);
WordNode *readIndexFile(char *path);

#endif
