#include "index.h"
#include "tokenizer.h"

/*
Print the usage of the program in case there is invalid input.
*/
void printHelp()
{
	printf("Usage: index <inverted-index file name> <directory or file name>\n");
}

/*
Check if argument path is a directory.
RETURN: 1 if directory, 0 otherwise (file)
*/
int is_dir(char *path) {
	struct stat buf;
	stat(path, &buf);
	return S_ISDIR(buf.st_mode);
}

/*
Free all memory that the program used up.
*/
void clean()
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

/*
Insert a node into a WordNode Linked List in the correct location. Everything is inserted already in ascending order.
*/
void insertIntoLL(WordNode *node)
{
	//first insert, set node as head
	if (head == NULL) {
		head = node;
		return;
	}

	WordNode *prev = NULL;
	WordNode *cur = head;
	while (cur != NULL)
	{
		int cmp = strcmp(cur->word, node->word);
		if (cmp > 0) 
		{ //word is past the sorted location, so add word inbetween
			if (prev == NULL) { // new node to add needs to be head
				node->next = head;
				head = node;
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
Parse through a file's contents, gather tokens, and add tokens to our WordNode LL
RETURNS: 1 on success, 0 on failure
*/
int processFile(char *path)
{
	char *buffer = NULL;	
	FILE *filestream = fopen(path, "r");
	if (filestream == NULL)
	{
		fprintf(stderr, "ERROR: unable to read from file: '%s'", path);
		perror("");
		return 0;
	}
	fseek(filestream, 0, SEEK_END);
	long fsize = ftell(filestream);
	fseek(filestream, 0, SEEK_SET);
	buffer = malloc(fsize+1);
	if (buffer == NULL) {
		perror("ERROR: Unable to malloc memory space");
		fclose(filestream);
		return 0;
	}
	if (fread(buffer, fsize, 1, filestream) == 0) {
		fprintf(stderr, "ERROR: Unable to read from file '%s'\n", path);
		free(buffer);
		fclose(filestream);
		return 0;
	}
	buffer[fsize] = '\0';
	fclose(filestream);

	TokenizerT *tokenizer = TKCreate(delims, buffer);
	if (tokenizer == NULL) {
		fprintf(stderr, "Error: unable to create tokenizer\n");
		return 0;
	}

	char * token = NULL;
	while((token = TKGetNextToken(tokenizer)) != NULL) 
	{
		//for each character in token, convert it to lowercase
		int i;
		for (i = 0; i < strlen(token); i++) {
			token[i] = tolower(token[i]);
		}
		WordNode *wnode = NULL;
		FileNode *fnode = NULL;

		//traverse LL and see if word exists
		if ( head != NULL )
		{
			WordNode *cur = head;
			while (cur != NULL)
			{
				if (strcmp(cur->word, token) == 0) { //word exists
					wnode = cur;
					break;
				}
				cur = cur->next;
			}
		}

		//token is new, add it
		if ( head == NULL || wnode == NULL)
		{
			wnode = (WordNode *) malloc(sizeof(WordNode));
			fnode = (FileNode *) malloc(sizeof(FileNode));
			fnode->count = 1;
			fnode->next = NULL;
			fnode->file = strdup(path);
			wnode->word = strdup(token);
			wnode->fnHead = fnode;
			wnode->next = NULL;
			insertIntoLL(wnode);
		} else 
		{ // token already exists
			//check if file node already exists for this word.
			FileNode *prev = NULL;
			FileNode *fcur = wnode->fnHead;
			FileNode *new = NULL; 
			int added = 0; //boolean
			while (fcur != NULL)
			{
				int cmp = strcmp(path, fcur->file);
				if (cmp == 0) { //file node exists for this word, increment count
					(fcur->count)++;
					added = 1;
					break;
				} else if (cmp > 0) 
				{ //word is past the sorted location, so file for word doesnt exist
					new = (FileNode *) malloc(sizeof(FileNode));
					new->file = strdup(path);
					new->count = 1;
					new->next = fcur;
					if (prev == NULL) { // new node to add needs to be head
						wnode->fnHead = new;
					} else {
						prev->next = new;
					}
					added = 1;
					break;
				}

				prev = fcur;
				fcur = fcur->next;
			}
			//if reached this point and not added, inserted node needs to go at end of LL
			if (added == 0)
			{
				new = (FileNode *) malloc(sizeof(FileNode));
				new->file = strdup(path);
				new->count = 1;
				new->next = NULL;
				prev->next = new;
			}
		}
		free(token);
	}

	TKDestroy(tokenizer);
	free(buffer);
	
	return 1;
}

/*
Traverse the entire WordNode LL and write to file
RETURNS: 1 on success, 0 on failure
*/
int writeToFile(char *file)
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

/*
Parse through a file's contents, gather tokens, and add tokens to our WordNode LL
RETURNS: 1 on success, 0 on failure
*/
void processDir(char *path)
{
	DIR *dirFD;
	struct dirent *ent;
	if ((dirFD = opendir(path)) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir(dirFD)) != NULL) 
		{
			if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
				continue;
			}

			int lendir = strlen(path); //length of path
			int lenfile = strlen(ent->d_name); //length of filename
			char buffer[lendir+lenfile+1]; //buffer concatenating path and filename
			memcpy(buffer, path, lendir);
			if (path[lendir-1] == '/') {
				memcpy(buffer+lendir, ent->d_name, lenfile+1);
			} else {
				buffer[lendir] = '/';
				memcpy(buffer+lendir+1, ent->d_name, lenfile+1);
			}

			if (is_dir(buffer) == 1)
			{
				processDir(buffer); //recursive call to read inner dir
			} else {
				processFile(buffer); 
			}
		}
		closedir(dirFD);
	} else {
		/* could not open directory */
		perror ("Could not open directory");
		return;
	}
}


int main(int argc, char **argv)
{
	char *writePath = argv[1];
	char *path = argv[2];

	//create delimiter string for tokenizer, all ascii minus alphanumerical chars
	int offset = 0; //offset to insert character into buffer
	int i;
	for (i = 1; i <= 47; i++) {
		delims[offset++] = i;
	}
	for (i = 58; i <= 64; i++) {
		delims[offset++] = i;
	}
	for (i = 91; i <= 96; i++) {
		delims[offset++] = i;
	}
	for (i = 123; i <= 127; i++) {
		delims[offset++] = i;
	}
	delims[offset] = '\0';


	if (argc != 3)
	{ //invalid input
		printHelp();
		return 1;
	}

	if (is_dir(path) == 1)
	{ //input is directory
		processDir(path);
	} else
	{ //input is file
		processFile(path);
	}
	
	int ret;
	ret = writeToFile(writePath);
	clean();
	if (ret == 1) {
		return 0; // succesfully written to output file
	} else {
		return 1; // failed to write to output file
	}
}
