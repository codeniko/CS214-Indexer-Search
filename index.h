#include <dirent.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>


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


static WordNode *head = NULL;
static char delims[66];

void printHelp();
int is_dir(char *path);
void clean();
void insertIntoLL(WordNode *node);
int processFile(char *path);
int writeToFile(char *file);
void processDir(char *path);
