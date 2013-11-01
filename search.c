#include "search.h"

void printHelp()
{
	fprintf(stdout, "Usage: search <inverted-index file name>\n");
}

void clean(WordNode *head)
{
	WordNode *cur = head;
	while (cur != NULL)
	{
		WordNode *temp = cur;
		free(temp->word);

		FileNode *fcur = temp->fnHead;
		while (fcur != NULL)
		{
			FileNode *ftemp = fcur;
			free(ftemp->file);
			fcur = ftemp->next;
			free(ftemp);
		}
		cur = temp->next;
		free(temp);
	}
}

WordNode *readIndexFile(char *path)
{
	FILE *file = fopen(path, "r");
	if (file == NULL) {
		fprintf(stderr, "ERROR: unable to read from file: '%s'", path);
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

SearchWordNode *queryOr(WordNode *head, SearchWordNode *sWordHead)
{
	if (head == NULL) {
		fprintf(stderr, "ERROR: No Index data read into memory.\n");
		return NULL;
	}

	SearchWordNode *printHead = NULL;
	SearchWordNode *swcur = NULL;
	WordNode *wcur = head;
	for (swcur = sWordHead; swcur != NULL; swcur = swcur->next)
	{
		for ( ; wcur != NULL; wcur = wcur->next)
		{ //loop through all words indexed, note* sorted ASC
			int cmp = strcmp(wcur->word, swcur->word);
			if (cmp > 1) //indexed word is > search word, stop search of word
				break;
			else if (cmp == 0) //word found, print all files containing word
			{
				FileNode *fcur = NULL;
				for (fcur = wcur->fnHead; fcur != NULL; fcur = fcur->next)
				{
					SearchWordNode *pnode = (SearchWordNode *) malloc(sizeof(SearchWordNode));
					pnode->word = fcur->file; //not duplicated, reusing address
					pnode->next = NULL;
					insertIntoLL(&printHead, pnode);
				}
			}
		}
	}
	return printHead;
}

SearchWordNode *queryAnd(WordNode *head, SearchWordNode *sWordHead)
{
	if (head == NULL) {
		fprintf(stderr, "ERROR: No Index data read into memory.\n");
		return NULL;
	}
	return NULL;
}

void insertIntoLL(SearchWordNode **head, SearchWordNode *node)
{
	//first insert, set node as head
	if (*head == NULL) {
		*head = node;
		return;
	}

	SearchWordNode *prev = NULL;
	SearchWordNode *cur = *head;
	while (cur != NULL)
	{
		int cmp = strcmp(cur->word, node->word);
		if (cmp == 0)
			return; // duplicate was found, ignore it
		else if (cmp > 0) 
		{ //word is past the sorted location, so add word inbetween
			if (prev == NULL) { // new node to add needs to be head
				node->next = *head;
				*head = node;
			} else {
				node->next = cur;
				prev->next = node;
			}
			return; //added, return from function
		}

		prev = cur;
		cur = cur->next;
	}
	//if reach this point, word is NOT in list and needs to be added to the end.
	prev->next = node;
}
/*
int writeToFile(WordNode *head, char *file)
{
	WordNode *cur = head;
	if (cur == NULL) {
		printf("There is nothing to write to '%s'", file);
		return 1;
	}
	FILE *fd = fopen(file, "w");
	if (fd == NULL) {
		fprintf(stderr, "ERROR opening file '%s' to write.", file);
		return 0;
	}
	while (cur != NULL)
	{
		fprintf(fd, "<list> %s\n", cur->word);
		int count = 1;
		FileNode *fcur = cur->fnHead;
		while (fcur != NULL)
		{
			if (count == 1) {
				fprintf(fd, "%s %d", fcur->file, fcur->count);
			} else {
				fprintf(fd, " %s %d", fcur->file, fcur->count);
			}
			fcur = fcur->next;
			if (count++ == 5 && fcur != NULL) {
				fprintf(fd, "\n");
				count = 1;
			}
		}
		fprintf(fd, "\n</list>\n");
		cur = cur->next;
	}

	fclose(fd);
	return 1;
}
*/
int main(int argc, char **argv)
{
	if (argc != 2) {
		printHelp();
		return 1;
	}

	WordNode *head = NULL;

	if ( (head = readIndexFile(argv[1])) == NULL) {
		return 1;
	}

	char buffer[BUFFER_SIZE];
	char *token = NULL;

	while(1)
	{
		if (fgets(buffer, BUFFER_SIZE, stdin) == NULL)
			break;
		if ( (token = strtok(buffer, " ")) == NULL)
			continue; //empty line

		int query;
		if (strncmp(token, "sa", 2) == 0)
			query = QUERY_AND;
		else if (strncmp(token, "so", 2) == 0)
			query = QUERY_OR;
		else if (strncmp(token, "q", 1) == 0)
			break;
		else {
			fprintf(stderr, "Invalid command. Possible commands are sa, so, and q\n");
			continue;
		}

		//Read all terms to search for in our query and place in Linked List
		SearchWordNode *sWordHead = NULL;
		while ( (token = strtok(NULL, " ")) != NULL )
		{
			SearchWordNode *sWordNode = (SearchWordNode *) malloc(sizeof(SearchWordNode));
			int i;
			int len = strlen(token);
			for (i = 0; i < len; i++) //convert to lower case
				token[i] = tolower(token[i]);
			if (token[len-1] == '\n') //remove trailing newline
				token[len-1] = '\0';

			sWordNode->word = strdup(token);
			sWordNode->next = NULL;
			insertIntoLL(&sWordHead, sWordNode);
		}

		//Use our Index data and find files that satisfy query
		SearchWordNode *printHead = NULL;
		if (query == QUERY_OR)
			printHead = queryOr(head, sWordHead);
		else if (query == QUERY_AND)
			printHead = queryAnd(head, sWordHead);

		if (printHead == NULL)
			printf("No files were found containing the search terms.\n");
		else 
		{
			SearchWordNode *printCur = NULL;
			//output file paths matching query
			for ( printCur = printHead; printCur != NULL; printCur = printCur->next)
			{
				if (printCur == printHead) //first output, so no comma
					printf("%s", printCur->word);
				else
					printf(", %s", printCur->word);
			}
			printf("\n");

			//free the structs containing output
			while (printHead != NULL)
			{
				SearchWordNode *temp = printHead;
				printHead = printHead->next;
				free(temp);
			}
		}

		//Free Linked List full of search terms for query
		while (sWordHead != NULL)
		{
			SearchWordNode *temp = sWordHead;
			sWordHead = sWordHead->next;
			free(temp->word);
			free(temp);
		}
	}

	clean(head);
	return 0;
}
