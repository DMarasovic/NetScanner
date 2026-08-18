#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(){

    char ip[16];
    int port;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd == -1){
        printf("Greska pri stvaranju socketa.\n");
        return 1;
    }

    struct sockaddr_in target;
    
    target.sin_family = AF_INET;
    target.sin_port = htons(port);

    inet_pton(AF_INET, ip, &target.sin_addr);

    return 0;
}