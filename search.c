#include "search.h"

void printHelp()
{
	fprintf(stdout, "Usage: search <inverted-index file name>\n");
}

void clean()
{

}

WordNode *readIndexFile(char *path)
{
	FILE *file = fopen(argv[1], "r");
	if (filestream == NULL)
	{
		fprintf(stderr, "ERROR: unable to read from file: '%s'", path);
		perror("");
		return 0;
	}

}

int main(int argc, char **argv)
{
	if (argc != 2) {
		printHelp();
		return 1;
	}

	readIndexFile(argv[1]);

	char buffer[BUFFER_SIZE];
	char *token = NULL;

	while(1)
	{
		if (fgets(buffer, BUFFER_SIZE, stdin) == NULL)
			break;
		if (strtok(buffer, " ", &token) == NULL)
			continue; //empty line

		int query;
		if (strncmp(token, "sa", 2))
			query = QUERY_AND;
		else if (strncmp(token, "so", 2))
			query = QUERY_OR;
		else if (strncmp(token, "q", 1))
			break;
		else {
			fprintf(stderr, "Invalid command. Possible commands are sa, so, and q\n");
			continue;
		}

		//Read all terms to search for in our query and place in Linked List
		SearchWordNode *sWordHead = NULL;
		SearchWordNode *sWordTail = NULL;
		while ( strtok(NULL, " ", &token) != NULL )
		{
			SearchWordNode sWordNode = (SearchWordNode *) malloc(sizeof(SearchWordNode));
			sWordNode->word = strdup(token);
			sWordNode->next = NULL;
			if (sWordHead == NULL) {
				sWordHead = sWordNode;
				sWordTail = sWordNode;
			} else {
				sWordTail->next = sWordNode;
				sWordTail = sWordNode;
			}
		}

		//Use our Index data and find files that satisfy query
		

		//Free Linked List full of search terms for query
		while (sWordHead != NULL)
		{
			SearchWordNode *temp = sWordHead;
			sWordHead = sWordHead->next;
			free(temp->word);
			free(temp);
		}
	}

	clean();
	return 0;
}
