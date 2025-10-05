#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXARGS 10

typedef enum {EXEC = 0, REDIR =1 , PIPE = 2, SEQ = 3, BG = 4 } cmdtype_t;

typedef struct{
	cmdtype_t type;
} node;

typedef struct{
	cmdtype_t type;
	node *left;
	node *right;
} pipeNode;

typedef struct{
	cmdtype_t type;
	node *left;
	node *right;
} seqNode;

typedef struct{
	cmdtype_t type;
	node *child;
} bgNode;

typedef struct{
	cmdtype_t type;
	int fd;
	int mode;
	node *child;
	char *file;
	char *efile; 
} redirNode;


typedef struct{
	cmdtype_t type;
	char *argv[MAXARGS];
	char *eargv[MAXARGS];

}execNode;



node *
execNode_INIT(){
	execNode *curr;
	curr = malloc(sizeof(execNode));
	memset(curr,0,sizeof(execNode));
	curr->type = EXEC;
	return (node *) curr;
}



int
gettoken(char **start, char *end, char **tok_start, char **tok_end ){

	char *dummy = *start;
	int tok = 'x';
	//check if string ended
	if (dummy >= end){
		return 'z';
	}
	//push pointer up till empty string
	while(dummy < end && *dummy != ' ' ){
		dummy++;
	}
	//update the start and end of the word by updating **
	if(tok_start){
		*tok_start = *start;
		*tok_end = dummy;
	}
	//push till next token
	while(*dummy == ' '){
		dummy++;
	}
	*start = dummy;
	return tok;
}


node*
parsExec(char *s, size_t size){
	int tok;
	char *end = s+size;
	char *tok_start, *tok_end;
	int counter = 0;
	execNode *curr = (execNode *) execNode_INIT();
	while((tok = gettoken(&s,end,&tok_start,&tok_end)) == 'x'){
		curr->argv[counter] =  tok_start;
		curr->eargv[counter] = tok_end;
		counter++;
	}
	curr->argv[counter] = 0;
	curr->eargv[counter] = 0;
	return (node *)curr;
}
void
parsExecNullTerminate(execNode* curr){
	for(int i=0; curr->eargv[i]; i++){

		printf("%d\n", i);
		printf("%c", *(curr->argv[i]));
		*curr->eargv[i] = 0;
	}
}
void
printNode(execNode* curr){
	for(int i=0; curr->argv[i]; i++){

	}
}

int
main(int argc, char *argv[]){
	while(1){
		char *buff;
		size_t cap;
		printf("mySh>>" );
		ssize_t n = getline(&buff, &cap, stdin);
		node *curr = parsExec(buff,n);
		printf("-------------\n");
		parsExecNullTerminate((execNode *) curr);
		printNode((execNode *) curr);
	}
}
