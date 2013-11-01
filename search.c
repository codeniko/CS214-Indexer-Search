#include "search.h"

//Print usage of program if not invoked correctly.
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
	{ //loop through all words in search query, sorted ASC
		for ( ; wcur != NULL; wcur = wcur->next)
		{ //loop through all words indexed, note* sorted ASC
			int cmp = strcmp(wcur->word, swcur->word);
			if (cmp > 0) //indexed word is > search word, stop search of word
				break;
			else if (cmp == 0) //word found, add to a LL that will be returned
			{
				FileNode *fcur = NULL;
				for (fcur = wcur->fnHead; fcur != NULL; fcur = fcur->next)
				{
					SearchWordNode *pnode = (SearchWordNode *) malloc(sizeof(SearchWordNode));
					pnode->word = fcur->file; //not duplicated, reusing address
					pnode->next = NULL;
					insertIntoLL(&printHead, pnode, FREE_STRDUP_NO);
				}
			}
		}
	}
	return printHead;
}

SearchWordNode *queryAnd(WordNode *head, SearchWordNode *sWord)
{
	if (head == NULL) {
		fprintf(stderr, "ERROR: No Index data read into memory.\n");
		return NULL;
	}
	if (sWord == NULL)
		return NULL;

	SearchWordNode *printHead = NULL;
	WordNode *wcur = head;
	for ( ; wcur != NULL; wcur = wcur->next)
	{ //loop through all words indexed, note* sorted ASC
		int cmp = strcmp(wcur->word, sWord->word);
		if (cmp > 0) //indexed word is > search word, stop search of word
			return NULL;
		else if (cmp == 0) //word found, add to a LL that will be returned
		{
			//Get all files that contain this word
			FileNode *fcur = NULL;
			for (fcur = wcur->fnHead; fcur != NULL; fcur = fcur->next)
			{
				SearchWordNode *pnode = (SearchWordNode *) malloc(sizeof(SearchWordNode));
				pnode->word = fcur->file; //not duplicated, reusing address
				pnode->next = NULL;
				insertIntoLL(&printHead, pnode, FREE_STRDUP_NO);
			}

			//Recursive call to search for files containing the next word in query
			SearchWordNode *printHead2 = queryAnd(wcur, sWord->next);
			if (printHead2 == NULL)
				return printHead;
			else
			{
				//go through printHead and printHead2 and create a LL subset that only contains files in both
				SearchWordNode *p1 = NULL; //local file nodes
				SearchWordNode *p2 = NULL; //recursively returned file nodes
				SearchWordNode *p3 = NULL; //returned subset
				for ( p1 = printHead; p1 != NULL; p1 = p1->next)
				{
					for (p2 = printHead2 ; p2 != NULL; p2 = p2->next)
					{
						//check if word is in both
						if (strcmp(p1->word, p2->word) == 0) {
							SearchWordNode *pnode = (SearchWordNode *) malloc(sizeof(SearchWordNode));
							pnode->word = p1->word;
							pnode->next = NULL;
							insertIntoLL(&p3,pnode, FREE_STRDUP_NO);
						}
					}
				}

				//subset is created, free printHead and printHead2 LLs, return p3
				while(printHead != NULL)
				{
					SearchWordNode *temp = printHead->next;
					free(printHead);
					printHead = temp;
				}
				while(printHead2 != NULL)
				{
					SearchWordNode *temp = printHead2->next;
					free(printHead2);
					printHead2 = temp;
				}
				return p3;
			}

		}
	}
	return NULL;
}

void insertIntoLL(SearchWordNode **head, SearchWordNode *node, int freeStrdup)
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
		if (cmp == 0) {
			if (freeStrdup == FREE_STRDUP_YES)
				free(node->word);
			free(node);
			return; // duplicate was found, ignore it
		}
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
			insertIntoLL(&sWordHead, sWordNode, FREE_STRDUP_YES);
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
