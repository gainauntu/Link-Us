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

#ifndef PROTO
#define PROTO
#define LENGTH_NAME 31
#define LENGTH_MSG 101
#define LENGTH_SEND 201
#define LENGTH_BIG 6000
#endif 
#ifndef LIST
#define LIST

typedef struct ClientNode {
    int data;
    struct ClientNode* prev;
    struct ClientNode* link;
    char ip[16];
    char name[31];
} ClientList;

ClientList *newNode(int sockfd, char* ip) {
    ClientList *np = (ClientList *)malloc( sizeof(ClientList) );
    np->data = sockfd;
    np->prev = NULL;
    np->link = NULL;
    strncpy(np->ip, ip, 16);
    strncpy(np->name, "NULL", 5);
    return np;
}
#endif
int server_sockfd = 0, client_sockfd = 0;
ClientList *root, *now;
void str_trim_lf (char* arr, int length) {
    int i;
    for (i = 0; i < length; i++) { // trim \n
        if (arr[i] == '\n') {
            arr[i] = '\0';
            break;
        }
    }
}
void catch_ctrl_c_and_exit(int sig) {
    ClientList *tmp;
    while (root != NULL) {
        printf("\nClose socketfd: %d\n", root->data);
        close(root->data); // close all socket include server_sockfd
        tmp = root;
        root = root->link;
        free(tmp);
    }
    printf("Bye\n");
    exit(EXIT_SUCCESS);
}

void send_to_all_clients(ClientList *np, char tmp_buffer[]) {
    ClientList *tmp = root->link;
    while (tmp != NULL) {
        if (np->data != tmp->data) { // all clients except itself.
            printf("Send to sockfd %d: \"%s\" \n", tmp->data, tmp_buffer);
            send(tmp->data, tmp_buffer, LENGTH_SEND, 0);
        }
        tmp = tmp->link;
    }
}
void send_to_particular_client(ClientList *np, char tmp_buffer[],char tmp_name[]) {
    ClientList *tmp = root->link;
    while (tmp != NULL) {
        if (strcmp(tmp->name,tmp_name)==0) { // all clients except itself.
            printf("Send to sockfd %d: \"%s\" \n", tmp->data, tmp_buffer);
            send(tmp->data, tmp_buffer, LENGTH_SEND, 0);
        }
        tmp = tmp->link;
    }
}

int usernm_verification(ClientList *np, char tmp_name[]) {
		int c=0;
		ClientList *tmp = root->link;
		while (tmp != NULL) {
			if (strcmp(tmp->name,tmp_name)==0) { // // only to the tmp client
				c++;
			}
			tmp = tmp->link;
		}
		return c;
}

void client_handler(void *p_client) {
    int leave_flag = 0, sendr = 0;
    char nickname[LENGTH_NAME] = {};
    char reservname[LENGTH_NAME] ={};
    char recv_buffer[LENGTH_MSG] = {};
    char recv_big[LENGTH_BIG] = {};
    char recv_buffer_bkp[LENGTH_MSG] = {};
    char send_buffer[LENGTH_SEND] = {};
    ClientList *np = (ClientList *)p_client;
	char req_acc[]="REQACCESS ";
	char* found;
    // Naming
    if (recv(np->data, nickname, LENGTH_NAME, 0) <= 0 || strlen(nickname) < 2 || strlen(nickname) >= LENGTH_NAME-1) {
        printf("%s didn't input name.\n", np->ip);
        leave_flag = 1;
    } else {
        strncpy(np->name, nickname, LENGTH_NAME);
        printf("%s(%s)(%d) joined the chatroom.\n", np->name, np->ip, np->data);
        sprintf(send_buffer, "%s(%s) joined the chatroom.", np->name, np->ip);
        send_to_all_clients(np, send_buffer);
	}

    // Conversation
    while (1) {
/**/	int usernm_start_index=0; 
/**/	int usernm_end_index=0;
/**/	char req_usernm[20]={};
/**/	int i=0,j=0;
	
        if (leave_flag) {
            break;
        }
        int receive = recv(np->data, recv_buffer, LENGTH_MSG, 0);
//checking message in requestaccess function ->>
/*new idea of checking for abusive language(oath) with function strstr()
*/ 
/**/	strcpy(recv_buffer_bkp, recv_buffer);
 
        if (receive > 0) {
            if (strlen(recv_buffer) == 0) {
                continue;
            }
	
	else if(strstr(recv_buffer_bkp, req_acc)!=NULL){
			
			found = strstr(recv_buffer_bkp, req_acc);
			if	(found!= NULL){
				usernm_start_index = (found - *(&recv_buffer_bkp))+strlen(req_acc); 
			}
				i= usernm_start_index;
				while(recv_buffer_bkp[i]!=' '){
					i++;
				}	
				usernm_end_index=i-1;
				i=usernm_start_index;
				
				while(1){
					if(i>usernm_end_index) break;
					req_usernm[j]=recv_buffer_bkp[i];
					j++;
					i++;
				}
}str_trim_lf(req_usernm, LENGTH_MSG);printf("'%s'",req_usernm);
/**/	sprintf(send_buffer, "%s：%s", np->name, recv_buffer);	
        } else if (receive == 0 || strcmp(recv_buffer, "exit") == 0) {
            printf("%s(%s)(%d) left the chatroom.\n", np->name, np->ip, np->data);
            sprintf(send_buffer, "%s(%s) left the chatroom.", np->name, np->ip);
            leave_flag = 1;
	}else if (receive == 0 || strstr(recv_buffer_bkp, "fuck")!=NULL || strstr(recv_buffer_bkp, "shit")!=NULL || strstr(recv_buffer_bkp, "asshole")!=NULL || strstr(recv_buffer_bkp, "pussy")!=NULL) {
            send_to_particular_client(np, "Abusive/swear word has been detected! You have been kicked out!",np->name);
        }/* else if (receive == 0 || strcmp(recv_buffer, "y") == 0) {
		strcpy(recv_big, recv_buffer);
		send_to_particular_client(np, send_buffer,req_usernm);
            
	}*/else {
            printf("Fatal Error: -1\n");
            leave_flag = 1;
        }
        if(strstr(recv_buffer_bkp, req_acc)!=NULL) {
		send_to_particular_client(np, send_buffer,req_usernm); 
	if(usernm_verification(np, req_usernm)==0)
		send_to_particular_client(np, "The requested user couldn't be detected!",np->name); 
	}
/**/	else send_to_all_clients(np, send_buffer);
	
    }

    // Remove Node
    close(np->data);
    if (np == now) { // remove an edge node
        now = np->prev;
        now->link = NULL;
    } else { // remove a middle node
        np->prev->link = np->link;
        np->link->prev = np->prev;
    }
    free(np);
}

int main()
{
    signal(SIGINT, catch_ctrl_c_and_exit);

    // Create socket
    server_sockfd = socket(AF_INET , SOCK_STREAM , 0);
    if (server_sockfd == -1) {
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
    server_info.sin_addr.s_addr = inet_addr("127.0.0.1");//INADDR_ANY;//
    server_info.sin_port = htons(8888);

    // Bind and Listen
    bind(server_sockfd, (struct sockaddr *)&server_info, s_addrlen);
    listen(server_sockfd, 5);

    // Print Server IP
    getsockname(server_sockfd, (struct sockaddr*) &server_info, (socklen_t*) &s_addrlen);
    printf("Start Server on: %s:%d\n", inet_ntoa(server_info.sin_addr), ntohs(server_info.sin_port));

    // Initial linked list for clients
    root = newNode(server_sockfd, inet_ntoa(server_info.sin_addr));
    now = root;

    while (1) {
        client_sockfd = accept(server_sockfd, (struct sockaddr*) &client_info, (socklen_t*) &c_addrlen);

        // Print Client IP
        getpeername(client_sockfd, (struct sockaddr*) &client_info, (socklen_t*) &c_addrlen);
        printf("Client %s:%d come in.\n", inet_ntoa(client_info.sin_addr), ntohs(client_info.sin_port));

        // Append linked list for clients
        ClientList *c = newNode(client_sockfd, inet_ntoa(client_info.sin_addr));
        c->prev = now;
        now->link = c;

        now = c;

        pthread_t id;
        if (pthread_create(&id, NULL, (void *)client_handler, (void *)c) != 0) {
            perror("Create pthread error!\n");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
