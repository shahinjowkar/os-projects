#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>

#define MAXARGS 10

typedef enum {EXEC = 0, REDIR =1 , PIPE = 2, SEQ = 3, BG = 4 } cmdtype_t;

//structs
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

//initializers
node *
execNode_INIT(){
	execNode *curr;
	curr = malloc(sizeof(execNode));
	memset(curr,0,sizeof(execNode));
	curr->type = EXEC;
	return (node *) curr;
}

node *
pipeNode_INIT(){
	pipeNode *curr;
	curr = malloc(sizeof(pipeNode));
	memset(curr,0,sizeof(pipeNode));
	curr->type=PIPE;
	return (node *) curr;
}

int 
peak(char *s, size_t size){
	char *end = s + size;
	while(s<end && isspace(*s)) s++;
	if(s == end) return 0;
	if(*s == '|') return '|';
	else return 'x';
}

int
gettoken(char **start, char *end, char **tok_start, char **tok_end ){
	char *dummy = *start;
	int tok = 'x';
	if (dummy >= end){
		return 'z';
	}
	if(*dummy == '|'){
		tok = '|';
	}

	while (dummy < end && !isspace(*dummy)) dummy++; //get to end of word
	if(tok_start){
		*tok_start = *start;//set the start
		*tok_end = dummy;//set the end
	}
	while(dummy<end && isspace(*dummy)) dummy++;//get to next token
	*start = dummy;
	return tok;
}


void
parsExecNullTerminate(execNode* curr){
	for(int i=0; curr->eargv[i]; i++){
		*curr->eargv[i] = 0;
	}
}

//parsers
node*
parsExec(char **s, size_t size){
	int tok;
	char *end = *s+size;
	char *tok_start, *tok_end;
	int counter = 0;
	execNode *curr = (execNode *) execNode_INIT();
	while((tok = gettoken(s,end,&tok_start,&tok_end)) == 'x'){
		curr->argv[counter] =  tok_start;
		curr->eargv[counter] = tok_end;
		counter++;
	}
	curr->argv[counter] = 0;
	curr->eargv[counter] = 0;
	return (node *)curr;
}


node *
parsePipe(char *s, size_t size){

	node *exec1 = parsExec(&s,size);
	printf("peak in pipe: %c",peak(s, size) );
	if(peak(s, size) == '|'){
		printf("s before get token: %c", *s);
		gettoken(&s,s+size,0,0);
		printf("s after get token: %c", *s);
		pipeNode *curr = (pipeNode *) pipeNode_INIT();
		curr->left = exec1;
		// curr->right = parsePipe(s,size);
		// parsExecNullTerminate((execNode *) curr->left);
		// parsExecNullTerminate((execNode *) curr->right);
		// return (node *) curr;
	}
	return exec1;
	parsExecNullTerminate((execNode *) exec1);
}

//executers
void
exec(execNode* curr){
	// pid_t pid;
	// if((pid = fork()) < 0){printf("panik");exit(1)};
	// if(pid==0){
	execvp((curr->argv[0]), curr->argv);
	printf("BAS SYNTAX");exit(0);
	// }


}

void
printPipe(pipeNode *curr){
	printf("type: %d", curr->type);
}

int
main(int argc, char *argv[]){

	while(1){
		char *buff = NULL;
		size_t cap = 0;
		pid_t pid;
		printf("mySh>>" );
		ssize_t n = getline(&buff, &cap, stdin);
		// node *curr = parsExec(buff,n);
		// parsExecNullTerminate((execNode *) curr);
		printPipe((pipeNode *) parsePipe(buff,n));
		// if((pid = fork()) < 0){printf("panik");exit(1);}
		// if(pid == 0){
		// 	exec((execNode *) curr);
		// }
		// wait(0);

	}
}
