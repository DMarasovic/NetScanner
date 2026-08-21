#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <poll.h>

int main(){

    char ip[16];
    int port;
    int scan_result;

    printf("Unesi IP adresu: " );
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

    //No Blok Flag
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1){
        printf("Greska pri citanju flagova: %s\n", strerror(errno));
        close(sockfd);
        return 1;
    }

    int fcntl_result = fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
    if (fcntl_result == -1) {
    printf("Greska pri postavljanju O_NONBLOCK: %s\n", strerror(errno));
    close(sockfd);
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
    
    //Poll()
    struct pollfd pfd = {0};

    pfd.fd = sockfd;
    pfd.events = POLLOUT;

    int timeout = 1000;
    
    int result = connect(
        sockfd,
        (struct sockaddr *)&target,
        sizeof(target)
    );

    //Provjera connect()
    if (result == 0){
        printf("Port je OPEN\n");
    }

    else if (result == -1 && errno == EINPROGRESS){
        printf("Konekcija je u procesu...\n");

        int poll_result = poll(&pfd, 1, timeout);
        
        if (poll_result > 0){
            int socket_error = 0;
            socklen_t error_len = sizeof(socket_error);
            
            int get_result = getsockopt(
                sockfd, 
                SOL_SOCKET, 
                SO_ERROR,
                &socket_error,
                &error_len
            );

            if (get_result == -1){
                int error = errno;
                printf("Greska u getsockopt: %s\n", strerror(error));
            }
            else if (socket_error == 0){
                printf("Port je OPEN\n");
            }
            else if (socket_error == ECONNREFUSED){
                printf("Port je CLOSED\n");
            }
            else{
                printf("Connect greska: %s\n", strerror(socket_error));
            }
        }
        else if (poll_result == 0){
            printf("TIMEOUT\n");
        }
        else{
            int error = errno;
            printf("Poll greska: %s\n", strerror(error));
        }
    }

    else if(result == -1 && errno != EINPROGRESS){
        int error = errno;
        printf("Connect greska: %s\n", strerror(error));
        close(sockfd);
        return 1;
    }
    
    close(sockfd);
 
    return 0;
}