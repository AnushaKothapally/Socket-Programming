
/*-------------------------------------------------------------------------------------------------------
*   Program Title: Text-based SMTP client.                                                              *
*   Author       : Anusha Kothapally                                                                    *
*   Class        : CSCI8210 , Fall 2017                                                                 *
*   Purpose      : Send an email from client.                                                        *
* -------------------------------------------------------------------------------------------------------
*/
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<stdio.h>
#include<unistd.h>

#define SERVERPORT 25
#define MAX_MSG 100
  
char quit[50]= "QUIT\r\n"; 
char data[50]="DATA\n";

char buf[2000];
char buf2[2000];
int sd, rc, i;
int len;

/*-------------------------------------------------------------------------------------------------------
    function        : main
    Input Parameters: 
                    argc: size of the commandline arguments
                    argv: an array containing hostname, port number, debugging option
    Output          : It will send an email to the provided Email Address. 

*--------------------------------------------------------------------------------------------------------
*/
int main(int argc, char *argv[]) {
	struct sockaddr_in localAddr, servAddr;
	struct hostent *h;
	if (argc < 3) {                                            // check the adequate commanline arguments provided or not
		printf("Usage: %s server <server> <port#> <data> \n", argv[0]);
		printf("or: %s <server> <data> \n", argv[0]);
		exit(1);
	}
	h = gethostbyname(argv[1]);                                // retrive the host information corresponding to hostname.
	if (h == NULL) {
		printf("%s: unknown host '%s' \n", argv[0], argv[1]);
		exit(1);
	}
	
	servAddr.sin_family = h->h_addrtype;
	memcpy((char *)&servAddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
	if (argc == 4)
		servAddr.sin_port = htons(atoi(argv[2]));
	else
		servAddr.sin_port = htons(SERVERPORT);



	sd = socket(AF_INET, SOCK_STREAM, 0);                         // creating socket
	printf("Socket descriptor\n");
	if (sd < 0) {
		perror("Cannot open socket");
		exit(1);
	}



	rc = connect(sd, (struct sockaddr *) &servAddr, sizeof(servAddr));     // Initiating connection to server
	if (rc < 0) {
		perror("Cannot connect\n");
		exit(1);
	}
    
    printf("Connected to server\n");
    read(sd, buf, 2000);
    
    if((argc==4&& (strcmp(argv[3],"1")==0))||(argc==3&& (strcmp(argv[2],"1")==0))){
    printf("Message from server:");
    puts(buf);
    }
    memset(buf, 0, sizeof(buf));
    

    char helo[]= "HELO gmail\n";
    sendmail_write(sd,helo);                                        // introduce ourselves 
    printf("Sending Message %s",helo);
    read(sd, buf, 2000);                                            //Read reply 
    if((argc==4&& (strcmp(argv[3],"1")==0))||(argc==3&& (strcmp(argv[2],"1")==0))){
    printf("Message from server:");
    puts(buf);
    }
    memset(buf, 0, sizeof(buf));
    
    char mailFrom[50];
    char mf[100]="MAIL FROM: ";                                      // Reading I/P Mail From
    printf("Please Enter Mail From:");
    scanf("%s",mailFrom);
    strcat(mf, "<");
    strcat(mf, mailFrom);
    strcat(mf, ">");
    strcat(mf,"\n");
   

    
   
    memset(buf, 0, sizeof(buf));
    printf("Sending mailfrom %s",mf);
    sendmail_write(sd,mf);                                         // Shake hand message
    read(sd, buf, 2000);
    if((argc==4&& (strcmp(argv[3],"1")==0))||(argc==3&& (strcmp(argv[2],"1")==0))){
    printf("Message from server:");
    puts(buf);
    }
    
  
    memset(buf, 0, sizeof(buf));
    
    char mailTo[50];
    char mt[100]="RCPT TO: ";                                       // Reading I/P Mail TO        
    printf("Please Enter Mail To:");
    scanf("%s",mailTo);
    strcat(mt, "<");
    strcat(mt, mailTo);
    strcat(mt, ">");
    strcat(mt,"\n");
   
    
    sendmail_write(sd,mt);                                          // Shake hand message
    printf("Sending Mail To %s",mt);
    read(sd, buf, 2000);
    if((argc==4&& (strcmp(argv[3],"1")==0))||(argc==3&& (strcmp(argv[2],"1")==0))){
    printf("Message from server:");
    puts(buf);
    }
    memset(buf, 0, sizeof(buf));
    
    
    char subject[50];
    char sub[100]="Subject: ";
    printf("Please Enter subject:");
    scanf("%s",subject);                                            // Reading I/P for Subject
    strcat(sub, subject);
    strcat(sub, "\n");
   
    
    printf("Sending data %s",data);                                 
    sendmail_write(sd,data);                                        // Sending data, body to follow
    read(sd, buf, 2000);
    if((argc==4&& (strcmp(argv[3],"1")==0))||(argc==3&& (strcmp(argv[2],"1")==0))){
    printf("Message from server:");
    puts(buf);
    }
    memset(buf, 0, sizeof(buf));

    
    char mff[100]="From: ";
    char mtt[100]="To: ";
    strcat(mailFrom,"\r\n");
    strcat(mailTo,"\r\n");
    strcat(mff, mailFrom);
    strcat(mtt, mailTo);
    char msgLine[500]="";
    int b;
    printf("Sending message %s\n",mtt);
    b=sendmail_write(sd,mtt);
    
    printf("Sending message %s\n",mff);
    b=sendmail_write(sd,mff);
    printf("Sending message %s\n\n",sub); 
    sendmail_write(sd,sub);                                         // Sending header of the mail

    
    char str2[10]=".\r\n";
    char msg[50];
    printf("Enter your message now.\n");
    scanf("%s",msg);
    strcat(msg,"\r\n");
    sendmail_write(sd,msg);
    printf("Sending message %s",msg);
     fflush (stdin);
    int count=1;     
    while(strcmp(msg,str2)!=0){                                     // send messages until condition met.
        memset(msg, 0, sizeof(msg));
        scanf("%s",msg);
        strcat(msg,"\r\n");
        memset(buf, 0, sizeof(buf));
        if(strcmp(msg,str2)!=0){
        b=sendmail_write(sd,msg);
        }
        printf("Sending message %s",msg); 
        count++;
        if(count==10)  //Maximum num of messages.
            break;
        
}
    
   // char msg1[20]=".\r\n";                                          //End of the message
    b=sendmail_write(sd,msg);
    read(sd, buf, 2000);
    if((argc==4&& (strcmp(argv[3],"1")==0))||(argc==3&& (strcmp(argv[2],"1")==0))){
    printf("Message from server:");
    puts(buf);
    }
    memset(buf, 0, sizeof(buf));
    sendmail_write(sd, quit);
    printf("Sending message QUIT\n");                               //send quit
    read(sd, buf, 2000);
    if((argc==4&& (strcmp(argv[3],"1")==0))||(argc==3&& (strcmp(argv[2],"1")==0))){
    printf("Message from server:");
    puts(buf);
    }
    printf("Your Message hase been sent.\n");
	close(sd);                                                      //Close socket and finish
	return 0;

}
/*-------------------------------------------------------------------------------------------------------
    function        : sendmail_write
    Input Parameters: 
                    sock: Socket Descriptor
                    str : message which need to be send 
    Output          : Message sent to server.
*--------------------------------------------------------------------------------------------------------
*/
 void sendmail_write(int  sock, char *str) {  
    write(sock, str, strlen(str));    
}
