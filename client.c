#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"

int main(int argc, char *argv[]) {
    int sockfd;
    char *message;
    char *response;

    char buffer[BUFLEN];

    char *host_ip = "34.241.4.235";
    int portno = 8080;

    while (1) {
        sockfd = open_connection(host_ip, portno, AF_INET, SOCK_STREAM, 0);

        fgets(buffer, BUFLEN - 1, stdin);
        buffer[strlen(buffer) - 1] = '\0';

        if (!strcmp(buffer, "register")) {
            char *url = "/api/v1/tema/auth/register";
            char *content_type = "application/json";
            int body_data_fields_count = 2;
            char **body_data = malloc(body_data_fields_count * sizeof(char *));
            for (int i = 0; i < body_data_fields_count; i++) {
                body_data[i] = malloc(LINELEN * sizeof(char));
            }
            printf("username=");
            scanf("%s", body_data[0]);
            printf("password=");
            scanf("%s", body_data[1]);

            JSON_Value *root_value = json_value_init_object();
            JSON_Object *root_object = json_value_get_object(root_value);
            char *serialized_string = NULL;
            json_object_set_string(root_object, "username", body_data[0]);
            json_object_set_string(root_object, "password", body_data[1]);
            serialized_string = json_serialize_to_string_pretty(root_value);

            message = compute_post_request(host_ip, url, content_type, serialized_string, 2, NULL, 0);

            // puts(serialized_string);
            json_free_serialized_string(serialized_string);
            json_value_free(root_value);

            puts(message);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            puts(response);
            
        }

        if (!strcmp(buffer, "login")) {
            char *url = "/api/v1/tema/auth/login";
            char *content_type = "application/json";
        }

        if (!strcmp(buffer, "enter_library")) {
            char *url = "/api/v1/tema/library/access";
        }

        if (!strcmp(buffer, "get_books")) {
            char *url = "/api/v1/tema/library/books";
        }

        if (!strcmp(buffer, "get_book")) {
            char *url = "/api/v1/tema/library/books/:bookId.";
        }

        if (!strcmp(buffer, "add_book")) {
            char *url = "/api/v1/tema/library/books";
            char *content_type = "application/json";
        }

        if (!strcmp(buffer, "delete_book")) {
            char *url = "/api/v1/tema/library/books/:bookId.";
        }

        if (!strcmp(buffer, "logout")) {
            char *url = "/api/v1/tema/auth/logout";
        }

        if (!strcmp(buffer, "exit")) {
            break;
        }

        puts(buffer);
    }
    // char *message;
    // char *response;
    // int sockfd;
    // // Ex 1.1: GET dummy
    // // Ex 1.2: POST dummy and print response

    // // Ex 2: Login

    // sockfd = open_connection("127.0.0.1", 8080, AF_INET, SOCK_STREAM, 0);

    // char **form_data = calloc(2, sizeof(char *));
    // for (int i = 0; i < 2; i++)
    // {
    //     form_data[i] = calloc(LINELEN, sizeof(char));
    // }
    // strcpy(form_data[0], "username=student");
    // strcpy(form_data[1], "password=student");

    // message = compute_post_request("localhost", "/api/v1/auth/login", "application/x-www-form-urlencoded", form_data, 2, NULL, 1);
    // puts(message);
    // send_to_server(sockfd, message);
    // response = receive_from_server(sockfd);
    // puts(response);

    // // Ex 3: GET weather key
    // char **cookies = calloc(1, sizeof(char *));
    // cookies[0] = calloc(LINELEN, sizeof(char));

    // strcpy(cookies[0], "connect.sid=s%3Aoy5V6CLDYPGv8SqYq6yzvXL-BpCKiCC1.BEK0tJsEWKyuecGp7ADt%2FNheOkMF7Ov3EcngkQmgHnE");
    // message = compute_get_request("localhost", "/api/v1/weather/key", NULL, cookies, 1);
    // puts(message);
    // send_to_server(sockfd, message);
    // response = receive_from_server(sockfd);
    // puts(response);
    // close(sockfd);

    // // Ex 4: GET weather data from OpenWeather API
    // sockfd = open_connection("37.139.20.5", 80, AF_INET, SOCK_STREAM, 0);
    // message = compute_get_request("api.openweathermap.org", "/data/2.5/weather", "lat=44.7398&lon=22.2767&appid=b912dd495585fbf756dc6d8f415a7649", NULL, 0);
    // puts(message);
    // send_to_server(sockfd, message);
    // response = receive_from_server(sockfd);
    // puts(response);
    // close(sockfd);

    // // Ex 5: POST weather data for verification
    // // sockfd = open_connection("127.0.0.1", 8080, AF_INET, SOCK_STREAM, 0);
    // // char **json_data = calloc(1, sizeof(char *));
    // // json_data[0] = calloc(LINELEN, sizeof(char));
    // // strcpy(json_data[0], basic_extract_json_response(response));
    // // message = compute_post_request("localhost", "/api/v1/weather/44.7398/22.2767", "application/json", json_data, 1, cookies, 1);
    // // puts(message);
    // // send_to_server(sockfd, message);
    // // response = receive_from_server(sockfd);
    // // puts(response);
    // // close(sockfd);

    // // Ex 6: Logout

    // free(message);
    // free(response);

    return 0;
}
