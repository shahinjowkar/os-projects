#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>

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
	if (dummy >= end){
		return 'z';
	}
	if(*dummy == '|'){
		return '|';
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


node*
parsExec(char *s, size_t size){
	int tok;
	char *end = s+size;
	char *tok_start, *tok_end;
	int counter = 0;
	execNode *curr = (execNode *) execNode_INIT();
	while((tok = gettoken(&s,end,&tok_start,&tok_end)) == 'x'){
		printf("in parsExec : %c\n ", *s);	
		curr->argv[counter] =  tok_start;
		curr->eargv[counter] = tok_end;
		counter++;
	}
	curr->argv[counter] = 0;
	curr->eargv[counter] = 0;
	printf("s end of parsExec: %c\n ", *s);
	return (node *)curr;
}

int 
peak(char *s, size_t size){
	char *end = s + size;
	while(s<end && !isspace(*s)) s++;
	if(s == end) return 0;
	if(*s == '|') return '|';
	else return 'x';
}


node *
parsePipe(char *s, size_t size){
	printf("s before parExec(pipe): %c\n ", *s);
	node *exec1 = parsExec(s,size);
	printf("s after parsexec(pipe)%c\n", *s);
	if( peak(s, size) == '|'){
		printf("PIPE");
	}
	printf("NO PIPE");

}


void
parsExecNullTerminate(execNode* curr){
	for(int i=0; curr->eargv[i]; i++){
		*curr->eargv[i] = 0;
	}
}



void
exec(execNode* curr){
	// pid_t pid;
	// if((pid = fork()) < 0){printf("panik");exit(1)};
	// if(pid==0){
	execvp((curr->argv[0]), curr->argv);
	printf("BAS SYNTAX");exit(0);
	// }


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
		parsePipe(buff,n);
		// if((pid = fork()) < 0){printf("panik");exit(1);}
		// if(pid == 0){
		// 	exec((execNode *) curr);
		// }
		// wait(0);

	}
}
