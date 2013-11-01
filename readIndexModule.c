#include "search.h"

WordNode *readIndexFile(char *path)
{
	FILE *file = fopen(path, "r");
	if (file == NULL) {
		fprintf(stderr, "ERROR: unable to read from file: '%s' :", path);
		perror("");
		return NULL;
	}

	char buffer[BUFFER_SIZE];
	char *token = NULL;
	WordNode *head = NULL;
	WordNode *wnode = NULL;
	WordNode *tail = NULL;
	FileNode *ftail = NULL;
	while (fgets(buffer, BUFFER_SIZE, file))
	{
		token = strtok(buffer, " ");
		if (token == NULL) { //if a line is empty
			fprintf(stderr, "ERROR: Unexpected token, skipping line\n");
			continue;
		}

		if (strncmp(token, "</list>", 7) == 0)
		{ //end reading file list for a current word
			wnode = NULL;
			ftail = NULL;
		}
		else if (wnode != NULL)
		{ //reading file list for a current word
			do {
				FileNode *fnode = (FileNode *) malloc(sizeof(FileNode));
				int lastI = strlen(token) - 1;
				if (token[lastI] == '\n') //remove trailing newline
					token[lastI] = '\0';
				fnode->file = strdup(token);
				fnode->count = atoi(strtok(NULL, " "));
				fnode->next = NULL;
				if (ftail == NULL) {
					ftail = fnode;
					wnode->fnHead = fnode;
				} else {
					ftail->next = fnode;
					ftail = fnode;
				}
			} while ( (token = strtok(NULL, " ")) != NULL );
		}
		else if (strncmp(token, "<list>", 6) == 0 && (token = strtok(NULL, " ")) != NULL)
		{ //create new WordNode 
			wnode = (WordNode *) malloc(sizeof(WordNode));
			int lastI = strlen(token) - 1;
			if (token[lastI] == '\n') //remove trailing newline
				token[lastI] = '\0';
			wnode->word = strdup(token);
			wnode->next = NULL;
			wnode->fnHead = NULL;
			if (head == NULL) {
				head = wnode;
				tail = wnode;
			} else {
				tail->next = wnode;
				tail = wnode;
			}
		}
	}

	fclose(file);
	return head;
}
