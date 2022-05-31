#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "client_utils.h"
#include "helpers.h"
#include "requests.h"
#include "parson.h"

int main(int argc, char *argv[]) {
    int sockfd;

    char buffer[BUFLEN];

    char *host_ip = "34.241.4.235";
    int portno = 8080;

    char **cookies = malloc(sizeof(char *));
    cookies[0] = NULL;

    char *token = NULL;

    while (1) {
        sockfd = open_connection(host_ip, portno, AF_INET, SOCK_STREAM, 0);

        fgets(buffer, BUFLEN - 1, stdin);
        buffer[strlen(buffer) - 1] = '\0';

        if (!strcmp(buffer, "register")) {
            register_command(sockfd, cookies);
        }

        else if (!strcmp(buffer, "login")) {
            cookies = login_command(sockfd, cookies);
        }

        else if (!strcmp(buffer, "enter_library")) {
            token = enter_library_command(sockfd, cookies, token);
        }

        else if (!strcmp(buffer, "get_books")) {
            get_books_command(sockfd, cookies, token);
        }

        else if (!strcmp(buffer, "get_book")) {
            get_book_command(sockfd, cookies, token);
        }

        else if (!strcmp(buffer, "add_book")) {
            add_book_command(sockfd, cookies, token);
        }

        else if (!strcmp(buffer, "delete_book")) {
            delete_book_command(sockfd, cookies, token);
        }

        else if (!strcmp(buffer, "logout")) {
            logout_command(sockfd, cookies, token);
        }

        else if (!strcmp(buffer, "exit")) {
            puts("You exited!");
            close_connection(sockfd);
            break;
        } 
        else {
            puts("Invalid command!");
        }
    }

    return 0;
}
