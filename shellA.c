//Yemane Berhane lab03A
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAXLINE 40 /* 40 chars per line, per command, should be enough. */

/** The setup() routine reads in the next command line string storing it in the input buffer.
The line is separated into distinct tokens using whitespace as delimiters.  Setup also 
modifies the args parameter so that it holds points to the null-terminated strings  which 
are the tokens in the most recent user command line as well as a NULL pointer, indicating the
end of the argument list, which comes after the string pointers that have been assigned to
args. ***/

/*
construct linked list to store history of input
*/
struct node{
    int index;
    char argsBuff[MAXLINE];
    struct node *next;
};
/*
get command input from history using prefix
*/
struct node* getCommand(char inBuff[],struct node **start){

    struct node *current=*start;
    struct node *result=NULL;
    int         length=strlen(inBuff);
    char        tempHost[MAXLINE];

    //check through the linked list if passed argument exit and return the result
    while(current!=NULL){                           

        strcpy(tempHost,current->argsBuff);
        tempHost[length]='\0';

        if(!strcmp(tempHost,inBuff)){
            result=current;
        }
        current=current->next;
    }     

    return result;
    
}
/*
delete command from history after been used
*/
void deleteCommand(struct node *temp)
{
    struct node *nextnode=temp->next;

    temp->next=temp->next->next;
    free(nextnode);

}
struct node* get(char inBuff[],struct node **start){

    struct node *current=*start;

    /*iterate the linked list to find node by its command value*/
    while(current->next!=NULL){                        
        if(!strcmp(current->next->argsBuff,inBuff)){
            break;
        }
        current=current->next;
    }    

    return current;  
}

void setup(char inputBuff[], char *args[],int *background, char inputCommand[])
{
    int length,  /* Num characters in the command line */
        i,       /* Index for inputBuff arrray          */
        j,       /* Where to place the next parameter into args[] */
        start;   /* Beginning of next command parameter */

    /* Read what the user enters */
    length = read(STDIN_FILENO, inputBuff, MAXLINE);  
    strcpy(inputCommand, inputBuff);
    j = 0;
    start = -1;

    if (length == 0)
        exit(0);            /* Cntrl-d was entered, end of user command stream */

    if (length < 0){
        perror("error reading command");
    exit(-1);           /* Terminate with error code of -1 */
    }
    
    /* Examine every character in the input buffer */
    for (i = 0; i < length; i++) {
 
        switch (inputBuff[i]){
        case ' ':
        case '\t' :          /* Argument separators */

            if(start != -1){
                args[j] = &inputBuff[start];    /* Set up pointer */
                j++;
            }

            inputBuff[i] = '\0'; /* Add a null char; make a C string */
            start = -1;
            break;

        case '\n':             /* Final char examined */
            if (start != -1){
                args[j] = &inputBuff[start];     
                j++;
            }

            inputBuff[i] = '\0';
            inputCommand[i] = '\0';
            args[j] = NULL; /* No more arguments to this command */
            break;

        case '&':
            *background = 1;
            inputBuff[i] = '\0';
            break;
            
        default :             /* Some other character */
            if (start == -1)
                start = i;
    }
 
    }    
    args[j] = NULL; /* Just in case the input line was > 40 */
} 
int getIndex(int index,struct node **start,char inBuff[] ){

    //check if user input for selection is greater than 0
    if(index<=0){
        printf("r has to be greater than 0: %d\n",index);   
        return 0;
    }

    //get the position of selected command from the linked list
    struct node *current=*start;
    while(current->next!=NULL){ 

        if(current->next->index==index){
            break;
        }
        current=current->next;
    }
    if(current->next!=NULL) {
        strcpy(inBuff,current->next->argsBuff);
        return 1;
    }else{
        printf("%d is not in history buffer\n",index);
        return 0;
    }
}
int  getHistory(char* inputBuff, char *args[])
{
    if(inputBuff==NULL){
        printf("Cannot find that command in history\n");
        return 1;
    }
   
    int i=0,       /* Index for inputBuff arrray          */
        j,       /* Where to place the next parameter into args[] */
        start;   /* Beginning of next command parameter */
   
    j = 0;
    start = -1;
    /* Examine every character in the input buffer */
    int inputlength=strlen(inputBuff);
    for (;i<inputlength+1;) {
        switch (inputBuff[i]){
        case ' ':
        case '\t' :          /* Argument separators */

            if(start != -1){
                args[j] = &inputBuff[start];    /* Set up pointer */
                j++;
            }
            inputBuff[i] = '\0'; /* Add a null char; make a C string */
            start = -1;
            break;
        case '\0':
            if(start != -1){
                args[j] = &inputBuff[start];    /* Set up pointer */
                j++;
            }
            inputBuff[i] = '\0'; /* Add a null char; make a C string */
            start = -1;
            break;

        case '&':
            inputBuff[i] = '\0';
            break;
            
        default :             /* Some other character */
            if (start == -1)
                start = i;
    }
    i++;
 
    } 
    if (start != -1){
        args[j] = &inputBuff[start];     
    }else{
        args[j]=NULL;
    }

    return 0;
}
/*
get the input argument and store it in history linked list to be reuse
*/
void addCommand(int no,char inBuff[],struct node *start,int* historySize,int displayHistory){
    //check if exit in history then update
        struct node *temp=get(inBuff,&start);    
        if(temp->next!=NULL){                       
            deleteCommand(temp);
            *historySize-=1;
        }
        while(*historySize >=displayHistory){                    
            deleteCommand(start);
            *historySize-=1;
        }
        // if doesn't exit in history create new node 
        struct node *current=start;                
        struct node *newnode=(struct node *) malloc(sizeof(struct node));
        newnode->index=no;
        strcpy(newnode->argsBuff, inBuff);
        newnode->next=NULL;
        //update the linked list 
        while(current->next!=NULL){                
            current=current->next;
        }
        current->next=newnode;
        *historySize+=1;
        
}
void printHistory(struct node *start){
    //print upto 5 command to console excluding history or h 
    while(start!=NULL){ 
        if(start->argsBuff[0]!='h'){           
        printf("%d  %s\n",start->index,start->argsBuff);
        }
        start=start->next;
    }
}

int main(void)
{
    char inputBuff[MAXLINE]; /* Input buffer  to hold the command entered */
    char *args[MAXLINE/2+1];/* Command line arguments */
    int background;         /* Equals 1 if a command is followed by '&', else 0 */
    struct node *head = NULL; /* Initiiaze head of linked list*/
    char inputCommand[MAXLINE]; /* get inputCommand from user*/
    char *inputHistory[MAXLINE/2+1]; /*inputCommand line argument*/
    char *singleCommand;/*get sing input command*/
    int historySize =0; /* history size initiliaze*/
    int index = 1; /* get index of initiale position*/
    int displayHistory = 6; /* number of command from history will be display -1*/

    //initial head of the linked list node 
    head=(struct node *) malloc(sizeof(struct node));  

    while (1){                                          /* Program terminates normally inside setup */

    background = 0;

    printf("CSE2431Sh$");                               /* Shell prompt */
        fflush(0);
    setup(inputBuff, args, &background,inputCommand);          /* Get next command */
    
    if(inputBuff[0]=='\0'){                             /*if input is null, break */
        continue; 
    }

    if(!strcmp(args[0],"r")&&args[1]==NULL){            /*check if user call history before 5 command call*/
        printf("r with no history number\n");
        continue;
    }

                                                        
    if(!strcmp(args[0],"r")){                           /* r vallid call  */
        if((args[1][0]>='A')) { 
            int         startIndex=0;                       
            struct node *temp=NULL;  
            singleCommand = &inputCommand[startIndex];
            temp = getCommand(singleCommand,&head); 
            
            if(temp==NULL){
                printf("no command found in history\n");
                continue;
            }
            else{
                strcpy(inputCommand,temp->argsBuff);
            }
        }else if(getIndex(atoi(args[1]),&head,inputCommand)==0){
            continue;
        }
        
    }
   
    if((!strcmp(args[0],"r"))         
        ||(!strcmp(args[0],"rr")&&getIndex(index-1,&head,inputCommand)==1)
            ||(strcmp(args[0],"r")&&strcmp(args[0],"rr"))){
                addCommand(index,inputCommand,head,&historySize,displayHistory);
                index++;
    }
    
     pid_t child;
     child = fork();    /*fork another process*/
     
     if(child<0) {    /*if fork failed, exit the process*/
        fprintf(stderr,"Fork Failed");
        exit(-1);
     } else if(child==0) {  
 
        if(!strcmp(args[0],"history")||!strcmp(args[0],"h")) {       
            printHistory(head->next);
            exit(-1);

        }//print history are rr or r is request from user 
        else if((!strcmp(args[0],"r")||!strcmp(args[0],"rr"))&&!getHistory(inputCommand,inputHistory)) { 
            if(!strcmp(inputHistory[0],"history")||!strcmp(inputHistory[0],"h")){
                printHistory(head->next);
                exit(-1);

            }else if(execvp(inputHistory[0],inputHistory)){
                printf("execvp: No such file or directory \n");
                exit(-1);
            }

        } else if(strcmp(args[0],"sethistory")&&execvp(args[0],args)){   
            printf("execvp: No such file or directory \n");
            exit(-1);
        } 

    } else {
        int condition=0;
        /* parent will wait for the child process to complete*/
        if(background==0) {
            waitpid(child,&condition,0);  
        }
        if(condition==1){
            printf("error!");
        }
    }
    }
}
