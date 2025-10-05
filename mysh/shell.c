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
gettoken(char **start, char *end, char *tok_start, char *tok_end ){

	char *dummy = *start;
	if(**start == EOF){
		return 'z';
	}
	printf("checkpoint2");

	int tok = 'x';
	while(dummy < end && strcmp(dummy, " ") != 0){
		dummy++;
	}
	printf("checkpoint3");

	if(tok_start){
		tok_start = *start;
	}
	printf("checkpoint4");

	if(tok_end){
		tok_end = dummy;
	}
	printf("checkpoint5");

	while(strcmp(dummy," ") == 0){
		dummy++;
	}
	printf("checkpoint6");

	*start = dummy; 
	printf("\n %c \n", tok);
	return tok;


}


node*
parsExec(char *s, size_t size){
	int tok;
	char *end = s+size;
	char *tok_start, *tok_end;
	node *curr = execNode_INIT();
	printf("checkpoint1");
	while((tok = gettoken(&s,end,tok_start,tok_end)) == 'x'){
		printf("HIIII");
	}

}

int
main(int argc, char *argv[]){
	while(1){
		char *buff;
		size_t size;
		printf("mySh>>" );
		getline(&buff, &size, stdin);
		parsExec(buff,size);

	}
}
