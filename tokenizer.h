#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_HEX_CHARS 2
#define MAX_OCT_CHARS 3

struct TokenizerT_;
typedef struct TokenizerT_ TokenizerT;
TokenizerT *TKCreate(char *separators, char *ts);
void TKDestroy(TokenizerT *tk);
char *TKGetNextToken(TokenizerT *tk);
