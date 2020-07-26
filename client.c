#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <termios.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

#ifndef STRING
#define STRING
#define RED   "\e[31;1m"
#define GRN   "\e[32;1m"
#define YEL   "\e[33;1m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"
void str_trim_lf (char*, int);
void str_overwrite_stdout();

#endif
#ifndef PROTO
#define PROTO

#define LENGTH_NAME 31
#define LENGTH_MSG 101
#define LENGTH_SEND 201
#define LENGTH_BIG 6000
#endif
// Global variables
volatile sig_atomic_t flag = 0;
int sockfd = 0;
char nickname[LENGTH_NAME] = {};
char checkname[LENGTH_NAME] = {};
char password[LENGTH_NAME] = {};
char checkpassword[LENGTH_NAME] = {};
char req_acc[]="REQACCESS ";
char* found;
// Functions
void str_trim_lf (char* arr, int length) {
    int i;
    for (i = 0; i < length; i++) { // trim \n
        if (arr[i] == '\n') {
            arr[i] = '\0';
            break;
        }
    }
}
int getch() {
    struct termios oldtc, newtc;
    int ch;
    tcgetattr(STDIN_FILENO, &oldtc);
    newtc = oldtc;
    newtc.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newtc);
    ch=getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldtc);
    return ch;
}
void str_overwrite_stdout() {
    printf("\r%s", "> ");
    fflush(stdout);
}
void catch_ctrl_c_and_exit(int sig) {
    flag = 1;
}

void recv_msg_handler() {
char commandb[50];
    char command[LENGTH_SEND];
	char commandcopy[LENGTH_SEND];
    char receiveMessage[LENGTH_SEND] = {};
    char receiveMessage_bkp[LENGTH_SEND] = {};
    int accessc = 0;
    while (1) {
	int usernm_start_index=0; 
/**/	int usernm_end_index=0;
/**/	char req_usernm[20]={};
	char chk[1]={};
/**/	int i=0,j=0,k=0;
        int receive = recv(sockfd, receiveMessage, LENGTH_SEND, 0);
	strcpy(receiveMessage_bkp, receiveMessage);
        if (receive > 0) {//point start
            printf("\r%s\n", receiveMessage);
		if(strstr(receiveMessage_bkp, req_acc)!=NULL){
			found = strstr(receiveMessage_bkp, req_acc);
			if	(found!= NULL){
				usernm_start_index = (found - *(&receiveMessage_bkp))+strlen(req_acc); 
			}
				i= usernm_start_index;
				while(receiveMessage_bkp[i]!=' '){
					i++;
				}	
				usernm_end_index=i-1;
				i=usernm_start_index;
				
				while(1){
					if(i>usernm_end_index) break;
					req_usernm[j]=receiveMessage_bkp[i];
					j++;
					i++;
				}
//				printf("'%s'",req_usernm); 
/**/			
			i++;
			for(;i<strlen(receiveMessage_bkp);i++){
				command[k]=receiveMessage_bkp[i];
				k++;
			}
//str_trim_lf(command, LENGTH_SEND);
memset(commandb, 0, 50);
memset(commandcopy, 0, 50);
			sprintf(commandcopy, "%s > test.txt",command);
			//str_trim_lf(commandcopy, LENGTH_SEND);
			printf("Requesting access from '%d' Command: '%s'\n",sockfd,commandcopy);
			printf("Would you like to proceed with the certain request(Y/N)?\n");

		/*while(1){
			
			if(chk == 'Y') {printf("Command Output will be sent shortly!\n"); accessc++;break;}
			else if(chk == 'N') {printf("Access has beed denied successfully!\n"); break;}
			else printf("Input only 'Y' or 'N'\n");
		}	*/
		if(accessc == 0){//start point of system procedure

strcpy(commandb, commandcopy);
		system(commandb);

memset(commandb, 0, 50);
memset(commandcopy, 0, 50);
memset(command, 0, 50);

	}//end point of system procedure		
}


            str_overwrite_stdout();
        }//point end
	 else if (receive == 0) {
            break;
        } else { 
            // -1 
        }
    }

/*   strcpy(commandb, commandcopy);
   system(command);*/
}

void send_msg_handler() {
    char message[LENGTH_MSG] = {};
    char commandresult[LENGTH_BIG];
    int rfd, na;
	
    while (1) {
        str_overwrite_stdout();
        while (fgets(message, LENGTH_MSG, stdin) != NULL) {
            str_trim_lf(message, LENGTH_MSG);//we need some counter to don't delete the new line from the result of command (Req access) Because the result can have a lot of lines
            if (strlen(message) == 0) {
                str_overwrite_stdout();
            } else {
                break;
            }
        }
        send(sockfd, message, LENGTH_MSG, 0);
        if (strcmp(message, "exit") == 0) {
            break;
        }
	else if (strcmp(message, "yes") == 0){
	rfd = open("test.txt", O_RDONLY);
                  if(rfd == -1) {
                  perror("Error in opening file");
                  exit(1);
                  }
                  na = read(rfd, commandresult, LENGTH_BIG);
                  //bufmes[na]='\0';
                  close(rfd);
	send(sockfd, commandresult, LENGTH_BIG, 0);
memset(commandresult, 0, 6001);
}
	else if(strstr(message, "fuck")!=NULL || strstr(message, "shit")!=NULL || strstr(message, "asshole")!=NULL || strstr(message, "pussy")!=NULL){
	printf("Abusive/swear word has been detected! You have been kicked out!");break;}
    }
    catch_ctrl_c_and_exit(2);
}

int main()
{
    signal(SIGINT, catch_ctrl_c_and_exit);
	int menu;
	int counter = 1;
	int breaker = 0;
	int connector = 0;
	int ch,i = 0;
	
    // Menu
	while(1){//start of menu
		if(connector > 0){
	printf(GRN"\t> %s is successfully logged-in.\n \tConnecting to the server...\n"RESET, nickname);
	sleep(2);
	break;}
	
	printf(YEL "\n\n\t************* "GRN"LINK_US"YEL " *************"RESET"\n\t*\t\tMENU\t\t  *\n");
	printf("\t* 1. Log-in\t\t\t  *\n\t* 2. Sign-up\t\t\t  *\n\t* 3. Exit  \t\t\t  *\n\t"YEL "***********************************"RED"\n\t>"RESET);
	scanf("%d",&menu);
	if(menu==1){//********************* 1st opt
	printf("\t\t*** Log-in ***\n\n");
	while(1){//start of username checking
	if(fgets(checkname, LENGTH_NAME, stdin) != NULL) {
        str_trim_lf(checkname, LENGTH_NAME);
	if (strlen(checkname) < 2 || strlen(checkname) >= LENGTH_NAME-1) {
        counter++;
	if(counter>2){
	printf(RED"\tIncorrect username!\n"RESET); breaker++;}
    }
	else if(strcmp(nickname,checkname) != 0){
	printf(RED"\tIncorrect username!\n"RESET);counter=1;breaker++;}
	else counter=0;
	if(counter==0){break;}
	if(breaker >= 3) break;
	}
	printf("\tUsername:");
   }//end of username checking
	while(1){//start of password checking
	if(breaker >= 3){breaker=0;printf(RED"\tIncorrect log-in limit (3) is exceeded!\n"RESET); break;}
	printf("\tPassword:");
	for (;;) {
        ch = getch();
	checkpassword[i]=ch;i++;
        if(ch == 10){i=0;break;}
        printf("*");
     }
	str_trim_lf(checkpassword, LENGTH_NAME);
	printf("\n");
	if (strcmp(password,checkpassword) != 0) {
        printf(RED"\t*Incorrect password!\n"RESET); breaker++;
		if(breaker >= 5){breaker=0; break;}
	}
	else {connector++;break;}
  }//end of password checking
}//end of 1st opt
	else if(menu==2){//********************* 2nd opt
	printf("\t\t*** Sign-up ***\n");
     	while(1){//username loop
	fgets(nickname, LENGTH_NAME, stdin);
        str_trim_lf(nickname, LENGTH_NAME);counter=1;
    	if (strlen(nickname) < 2 || strlen(nickname) >= LENGTH_NAME-1) {
        printf(RED"\t\n*Username must be more than one and less than thirty characters.\n"RESET);
        counter=0;
    }
	if(counter==0){printf("Please enter your username: ");continue;}
	else if(counter==1) break;
	}
    	while(1){//password loop
	printf("Please enter your password: ");
	for (;;) {
        ch = getch();
	password[i]=ch;i++;
        if(ch == 10){i=0;break;}
        printf("*");
     }
	str_trim_lf(password, LENGTH_NAME);
	printf("\n");
	if (strlen(password) < 4 || strlen(password) >= LENGTH_NAME-1) {
        printf(RED"\t\n*Password must be more than four and less than thirty characters.\n"RESET);
	}
	else {printf("Account is successfully created. Log-in to connect to the server\n");break;}
}
    }//end of 2nd opt
	else if(menu==3)exit(1);
	else{ printf("Incorrect option!\n");exit(1);}
}//end of menu
    // Create socket
    sockfd = socket(AF_INET , SOCK_STREAM , 0);
    if (sockfd == -1) {
        printf("Fail to create a socket.");
        exit(EXIT_FAILURE);
    }

    // Socket information
    struct sockaddr_in server_info, client_info;
    int s_addrlen = sizeof(server_info);
    int c_addrlen = sizeof(client_info);
    memset(&server_info, 0, s_addrlen);
    memset(&client_info, 0, c_addrlen);
    server_info.sin_family = AF_INET;
    server_info.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_info.sin_port = htons(8888);

    // Connecting to the Server
    int err = connect(sockfd, (struct sockaddr *)&server_info, s_addrlen);
    if (err == -1) {
        printf("Connection to Server error!\n");
        exit(EXIT_FAILURE);
    }
    
    // Names
    getsockname(sockfd, (struct sockaddr*) &client_info, (socklen_t*) &c_addrlen);
    getpeername(sockfd, (struct sockaddr*) &server_info, (socklen_t*) &s_addrlen);
    printf("Connected to Server: %s Port:%d\n", inet_ntoa(server_info.sin_addr), ntohs(server_info.sin_port));
    printf("Current user: %s %s:%d\n",nickname, inet_ntoa(client_info.sin_addr), ntohs(client_info.sin_port));
printf(GRN"There are two rules you need to abide by:\n");
printf(RED"1: Don't use abusive language!\n");
printf("2:Use this pattern for requesting access to different client!\n  REQACCESS username command\n"RESET);
    send(sockfd, nickname, LENGTH_NAME, 0);
    //Thread
    pthread_t send_msg_thread;
    if (pthread_create(&send_msg_thread, NULL, (void *) send_msg_handler, NULL) != 0) {
        printf ("Create pthread error!\n");
        exit(EXIT_FAILURE);
    }

    pthread_t recv_msg_thread;
    if (pthread_create(&recv_msg_thread, NULL, (void *) recv_msg_handler, NULL) != 0) {
        printf ("Create pthread error!\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        if(flag) {
            printf("\nBye\n");
            break;
        }
    }

    close(sockfd);
    return 0;
}
