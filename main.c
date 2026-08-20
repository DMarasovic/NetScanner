#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(){

    char ip[16];
    int port;
    int scan_result;

    printf("Unesi IP adresu:" );
    scanf("%15s", ip);
        
    do{
        printf("Unesi port: ");

        scan_result = scanf("%d", &port);

        if (scan_result != 1){
            printf("Port mora biti broj.\n");

            while (getchar() != '\n');
            continue;
        }

        if (port > 65535 || port < 1){
            printf("Netocno uneseni port.\n");
        }
    }
    while (scan_result != 1 || port > 65535 || port < 1 );

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd == -1){
        printf("Greska pri stvaranju socketa.\n");
        return 1;
    }

    struct sockaddr_in target = {0};
    
    target.sin_family = AF_INET;
    target.sin_port = htons(port);

    int inet = inet_pton(AF_INET, ip, &target.sin_addr);

    //Provjera inet_pton()
    if (inet == 0){
        printf("IP adresa nije valjana.\n");
        close(sockfd);
        return 1;
    }
    else if (inet == -1){
        printf("Greška!\n");
        close(sockfd);
        return 1;
    }
    
    int result = connect(
        sockfd,
        (struct sockaddr *)&target,
        sizeof(target)
    );

    if (result == 0){
        printf("Port je OPEN\n");
    }
    else{
        printf("Konekcija je neuspjela");
    }

    close(sockfd);

    return 0;
}