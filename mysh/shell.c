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

//INITIALIZERS
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

node *
redirNode_INIT(){
	redirNode *curr;
	curr = malloc(sizeof(redirNode));
	memset(curr,0,sizeof(redirNode));
	curr->type = REDIR;
	return (node *) curr;
}
//PROCESSORS
int 
peak(char *s, char *end){
	while(s<end && isspace(*s)) s++;
	if(s == end) return 0;
	if(*s == '|') return '|';
	if(*s == '<') return '<';
	if(*s == '>'){
		if(s+1<end && *(s+1) == '>'){return '+';}
		else{return '>';}
	}
	else return 'x';
}

int
gettoken(char **start, char *end, char **tok_start, char **tok_end ){
	char *dummy = *start;
	int tok = 'x';
	if (dummy >= end){return 0;}
	if(*dummy == '|'){tok = '|';}
	if(*dummy == '<'){tok = '<';}
	if(*dummy == '>'){
		if(dummy + 1 < end && *(dummy +1) == '>'){tok = '+';}
		else{tok =  '>';}
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

//PARSERS

node *
parseRedir(node *child, char **s, char *end){
	int tok;
	char *tok_start, *tok_end;
	redirNode *curr = (redirNode *)redirNode_INIT();
	tok = gettoken(s,end,0,0);

	if(tok == '<'){
		curr->fd = STDIN_FILENO;
		curr->mode = O_RDONLY;
	}
	else if(tok == '>'){
		curr -> fd = STDOUT_FILENO;
		curr->mode = O_WRONLY | O_CREAT | O_TRUNC;
	}
	else if(tok == '+'){
		curr->fd = STDOUT_FILENO;
		curr->mode =O_WRONLY | O_CREAT | O_APPEND;
	}
	else{
		free(curr);
		exit(1);
	}
	
	if((tok = gettoken(s,end,&tok_start,&tok_end)) != 'x'){

		exit(1);
	}
	curr->efile = tok_end;
	curr->file = tok_start;
	if(peak(*s,end) == 0){
		curr->child = child;
		return (node *) curr;
	}
	if(peak(*s,end) == '>' || peak(*s,end) == '<' || peak(*s,end) == '+' ){
			curr->child = (node *) parseRedir(child,s,end);
	}
	else{
		curr->child = child;
	}
	return (node *)curr;	
}


node*
parsExec(char **s, char *end){
	int tok;
	char *tok_start, *tok_end;
	int counter = 0;
	execNode *curr = (execNode *) execNode_INIT();
	while( peak(*s,end) == 'x'){
		if((tok = gettoken(s,end,&tok_start,&tok_end)) == 0) break;
		curr->argv[counter] =  tok_start;
		curr->eargv[counter] = tok_end;
		counter++;
	}
	curr->argv[counter] = 0;
	curr->eargv[counter] = 0;
	if((peak(*s,end) == '<') || (peak(*s,end) == '+') ||(peak(*s,end) == '>')){
		return parseRedir((node *)curr,s,end);
	}
	return (node *)curr;
}


node *
parsePipe(char *s, char *end){
	// printf("PARSING PIPE\n");
	node *exec1 = parsExec(&s,end);
	if(peak(s,end) == '|'){
		gettoken(&s,end,0,0);
		pipeNode *curr = (pipeNode *) pipeNode_INIT();
		curr->left = exec1;
		curr->right = parsePipe(s,end);
		return (node *) curr;
	}
	return exec1;

}

//executers
void
exec(node* curr){
	if(curr->type == PIPE){
		pid_t pid1, pid2;
		int fd[2];
		pipe(fd);
		if((pid1 = fork())<0){printf("panic1");exit(1);}
		if(pid1 == 0){

			if(dup2(fd[1],STDOUT_FILENO ) < 0 ) {printf("panic2");exit(1);};
			close(fd[0]);
			close(fd[1]);
			exec(((pipeNode *)curr)->left);
			printf("panic3");
			exit(1);
		}
		if((pid2 = fork())<0){printf("panic4");exit(1);}
		if(pid2 == 0){
			if(dup2(fd[0],STDIN_FILENO)<0){printf("panic5");exit(1);};
			close(fd[1]);
			close(fd[0]);
			exec(((pipeNode *)curr)->right);
			exit(1);
		}
		close(fd[1]);
		close(fd[0]);
		wait(NULL);
		wait(NULL);
	}
	if(curr->type == REDIR){

		int fdOld = ((redirNode *) curr) ->fd;
		int mode = ((redirNode *) curr) -> mode;
		node *child = ((redirNode *) curr) -> child;
		char *file = ((redirNode *) curr) -> file;
		int fd = open(file,mode,S_IRWXU);
		printf("fdOld: %d, mode: %d, fileName: %s\n", fdOld,mode,file);
		dup2(fd,fdOld);
		exec(child);
	}
	if(curr->type == EXEC){
		execvp((((execNode *)curr)->argv[0]), ((execNode *)curr)->argv);
		printf("BAS SYNTAX");exit(0);
	}

}



void
printTree(node *curr){
	if(curr->type == REDIR){
		*(((redirNode *) curr)->efile) = '\0';
		printf("%s\n", ((redirNode *) curr)->file);
		printTree((((redirNode *) curr)->child));
	}
	else if(curr->type == PIPE){
		printf("|\n");
		printTree(((pipeNode *) curr) ->left);
		printTree(((pipeNode *) curr) ->right);
	}
	else{
		parsExecNullTerminate((execNode *) curr);
		for(int i = 0; ((execNode *) curr)->argv[i];i++) printf("%s\n", ((execNode *) curr)->argv[i]);
	}
}
void 
printExec(execNode *curr){
	for(int i = 0; curr->argv[i] ; i++) printf("i = %d, value: %s\n", i, curr->argv[i]);
}

int
main(int argc, char *argv[]){
		// char *buff = NULL;
		// size_t cap = 0;
		// pid_t pid;
		// printf("mySh>>" );
		// ssize_t n = getline(&buff, &cap, stdin);
		// char *end = buff + n;
		// node *root = parsePipe(buff,end);
		// printTree(root);
		// exec(root);

	
	while(1){
		char *buff = NULL;
		size_t cap = 0;
		pid_t pid;
		printf("mySh>>" );
		ssize_t n = getline(&buff, &cap, stdin);
		char *end = buff + n;
		if((pid = fork()) < 0){printf("panik7");exit(1);}
		if(pid == 0){
			node *root = parsePipe(buff,end);
			printTree(root);
			exec(root);
		}
		wait(NULL);
		

	}
}
