#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXARGS 10

typedef enum {EXEC = 0, REDIR =1 , PIPE = 2, SEQ = 3, BG = 4 } cmdtype_t;
typedef enum{INPUT = 0, OUTPU =1} redir_t;



typedef struct{
	cmdtype_t type;
} node;

typedef struct{
	cmdtype_t type;
	char *argv[MAXARGS];
	char *eargv[MAXARGS];

}execNode;

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
} BG;

typedef struct{
	cmdtype_t;
	
}

char
getToken(char **ps, char *es, ){
	
}


node*
parser(char *buff, size_t size){
	char *s = buff;
	char *es = buff + size; 
	getToken(&s,es);
};

int
main(int argc, char *argv[]){
	while(1){

		char *buff;
		size_t size;
		printf("mySh>>" );
		getline(&buff, &size, stdin);
		parser(*buff,size)

	}
}
