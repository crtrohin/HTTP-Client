#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "parson.h"
#include "requests.h"
#include "client_utils.h"

char *host_ip = "34.241.4.235";

void register_command(int sockfd, char **cookies) {
    if (cookies[0] != NULL) {
        puts("You have to log out first!");
    } else {
        char *url = "/api/v1/tema/auth/register";
        char *content_type = "application/json";

        char *username = malloc(LINELEN * sizeof(char));
        char *password = malloc(LINELEN * sizeof(char));

        printf("username=");
        fgets(username, LINELEN - 1, stdin);
        username[strlen(username) - 1] = '\0';

        printf("password=");
        fgets(password, LINELEN - 1, stdin);
        password[strlen(password) - 1] = '\0';

        int spaceCheck = strchr(username, ' ') != NULL || strchr(password, ' ') != NULL;;

        if (spaceCheck) {
            puts("Invalid credentials! Should not contain space character! Try again.");
        } else {

            JSON_Value *root_value = json_value_init_object();
            JSON_Object *root_object = json_value_get_object(root_value);

            json_object_set_string(root_object, "username", username);
            json_object_set_string(root_object, "password", password);
            
            char *serialized_string = NULL;
            serialized_string = json_serialize_to_string_pretty(root_value);

            char *message = compute_post_request(host_ip, url, content_type, serialized_string, NULL, 0, NULL);
            send_to_server(sockfd, message);

            free(username);
            free(password);

            json_free_serialized_string(serialized_string);
            json_value_free(root_value);

            char *response = receive_from_server(sockfd);

            root_value = json_parse_string(basic_extract_json_response(response));
            root_object = json_value_get_object(root_value);
            char *error_message = (char*)json_object_get_string(root_object, "error");

            if (error_message != NULL) {
                puts(error_message);
            } else {
                puts("Your account has been successfully created!");
            }

            free(message);
            free(response);
        }
    }
}

char **login_command(int sockfd, char **cookies) {
    if (cookies[0] != NULL) {
        puts("You have to log out first!");
    } else {
        char *url = "/api/v1/tema/auth/login";
        char *content_type = "application/json";

        char *username = malloc(LINELEN * sizeof(char));
        char *password = malloc(LINELEN * sizeof(char));

        printf("username=");
        fgets(username, LINELEN - 1, stdin);
        username[strlen(username) - 1] = '\0';

        printf("password=");
        fgets(password, LINELEN - 1, stdin);
        password[strlen(password) - 1] = '\0';

        int spaceCheck = strchr(username, ' ') != NULL || strchr(password, ' ') != NULL;;

        if (spaceCheck) {
            puts("Invalid credentials! Should not contain space character! Try again.");
        } else {
            JSON_Value *root_value = json_value_init_object();
            JSON_Object *root_object = json_value_get_object(root_value);

            json_object_set_string(root_object, "username", username);
            json_object_set_string(root_object, "password", password);
            
            char *serialized_string = NULL;
            serialized_string = json_serialize_to_string_pretty(root_value);

            char *message = compute_post_request(host_ip, url, content_type, serialized_string, NULL, 0, NULL);
            send_to_server(sockfd, message);

            free(username);
            free(password);

            json_free_serialized_string(serialized_string);
            json_value_free(root_value);

            char *response = receive_from_server(sockfd);

            root_value = json_parse_string(basic_extract_json_response(response));
            root_object = json_value_get_object(root_value);
            char *error_message = (char *)json_object_get_string(root_object, "error");

            if (error_message != NULL) {
                puts(error_message);
                free(error_message);
            } else {
                cookies[0] = strstr(response, "Set-Cookie: ");
                cookies[0] = strtok(cookies[0], ";: ");
                cookies[0] = strtok(NULL,";: ");

                puts("You successfully logged in!");
            }

            free(message);
            free(response);
        }
    }

    return cookies;
}

char *enter_library_command(int sockfd, char **cookies, char *token) {
    if (cookies[0] == NULL) {
        puts("You are not logged in!");
        return NULL;
    } else if (token != NULL) {
        puts("You already entered library!");
        return token;
    } else {
        char *url = "/api/v1/tema/library/access";

        char *message = compute_get_request(host_ip, url, NULL, cookies, 1, NULL);
        send_to_server(sockfd, message);

        char *response = receive_from_server(sockfd);

        JSON_Value *root_value = json_parse_string(basic_extract_json_response(response));
        JSON_Object *root_object = json_value_get_object(root_value);
        
        char *error_message = (char *)json_object_get_string(root_object, "error");

        if (error_message != NULL) {
            puts(error_message);
            free(error_message);
            return NULL;
        } else {
            token = (char *)json_object_get_string(root_object, "token");
            puts("Successfully entered library!");
            return token;
        }

        free(message);
        free(response);
    }
}

void get_books_command(int sockfd, char **cookies, char *token) {
    if (cookies[0] == NULL) {
        puts("You are not logged in!");
    } else if (token == NULL) {
        puts("You don't have access to library!");
    } else {
        char *url = "/api/v1/tema/library/books";

        char *message = compute_get_request(host_ip, url, NULL, NULL, 0, token);
        send_to_server(sockfd, message);

        char *response = receive_from_server(sockfd);

        JSON_Value *root_value = json_parse_string(basic_extract_json_response(response));
        JSON_Object *root_object = json_object(root_value);

        char *error_message = (char *)json_object_get_string(root_object, "error");

        if (error_message != NULL) {
            puts(error_message);
            free(error_message);
        } else {
            JSON_Value *root = json_parse_string(strchr(response, '['));
            JSON_Array *books = json_value_get_array(root);
            if (json_array_get_count(books) <= 0) {
                puts("No books in the library! Start adding!");
            } else {
                for (int i = 0; i < json_array_get_count(books); i++) {
                    fprintf(stdout, "Book nr. %d: \n", i + 1);
                    JSON_Object *book = json_array_get_object(books, i);
                    fprintf(stdout, "\tID: %.0f\n",json_object_get_number(book, "id"));
                    fprintf(stdout, "\tTitle: %s\n", json_object_get_string(book, "title"));
                }
            }
        }
        json_value_free(root_value);
        free(message);
        free(response);
    }
}

void get_book_command(int sockfd, char **cookies, char *token) {
    if (cookies[0] == NULL) {
        puts("You are not logged in!");
    } else if (token == NULL) {
        puts("You don't have access to library!");
    } else {
        char *id = malloc(LINELEN * sizeof(char));
        printf("id=");
        fgets(id, LINELEN - 1, stdin);
        id[strlen(id) - 1] = '\0';

        char *url = malloc(LINELEN * sizeof(char));
        memset(url, 0, LINELEN);
        sprintf(url, "/api/v1/tema/library/books/%s",id);

        char *message = compute_get_request(host_ip, url, NULL, NULL, 0, token);

        send_to_server(sockfd, message);
        char *response = receive_from_server(sockfd);

        JSON_Value *root_value = json_parse_string(basic_extract_json_response(response));
        JSON_Object *root_object = json_object(root_value);

        char *error_message = (char *)json_object_get_string(root_object, "error");

        if (error_message != NULL) {
            puts(error_message);
        } else {
            JSON_Object *book = root_object;
            fprintf(stdout, "\tID: %.0f\n",json_object_get_number(book, "id"));
            fprintf(stdout, "\tTitle: %s\n", json_object_get_string(book, "title"));
            fprintf(stdout, "\tAuthor: %s\n",json_object_get_string(book, "author"));
            fprintf(stdout, "\tPublisher: %s\n", json_object_get_string(book, "publisher"));
            fprintf(stdout, "\tGenre: %s\n", json_object_get_string(book, "genre"));
            fprintf(stdout, "\tPage Count: %.0f\n",json_object_get_number(book, "page_count"));
        }

        free(message);
        free(response);
    }
}

void add_book_command(int sockfd, char **cookies, char *token) {
    if (cookies[0] == NULL) {
        puts("You are not logged in!");
    } else if (token == NULL) {
        puts("You don't have access to library!");
    } else {
        char *url = "/api/v1/tema/library/books";
        char *content_type = "application/json";

        char *title = malloc(LINELEN * sizeof(char));
        char *author = malloc(LINELEN * sizeof(char));
        char *genre = malloc(LINELEN * sizeof(char));
        char *publisher = malloc(LINELEN * sizeof(char));
        char *page_count_string = malloc(LINELEN * sizeof(char));

        printf("title=");
        fgets(title, BUFLEN - 1, stdin);
        title[strlen(title) - 1] = '\0';

        printf("author=");
        fgets(author, BUFLEN - 1, stdin);
        author[strlen(author) - 1] = '\0';

        printf("genre=");
        fgets(genre, BUFLEN - 1, stdin);
        genre[strlen(genre) - 1] = '\0';

        printf("publisher=");
        fgets(publisher, BUFLEN - 1, stdin);
        publisher[strlen(publisher) - 1] = '\0';

        printf("page_count=");
        fgets(page_count_string, BUFLEN - 1, stdin);
        page_count_string[strlen(page_count_string) - 1] = '\0';

        char *string_part = NULL;
        int page_count = strtol(page_count_string, &string_part, 10);

        if (!strcmp(string_part, "")) {
            JSON_Value *root_value = json_value_init_object();
            JSON_Object *root_object = json_value_get_object(root_value);

            json_object_set_string(root_object, "title", title);
            json_object_set_string(root_object, "author", author);
            json_object_set_string(root_object, "genre", genre);
            json_object_set_number(root_object, "page_count", page_count);
            json_object_set_string(root_object, "publisher", publisher);

            char *serialized_string = NULL;
            serialized_string = json_serialize_to_string_pretty(root_value);

            char *message = compute_post_request(host_ip, url, content_type, serialized_string, NULL, 0, token);
            send_to_server(sockfd, message);

            json_free_serialized_string(serialized_string);
            json_value_free(root_value);

            char *response = receive_from_server(sockfd);

            root_value = json_parse_string(basic_extract_json_response(response));
            root_object = json_value_get_object(root_value);
            char *error_message = (char*)json_object_get_string(root_object, "error");

            if (error_message != NULL) {
                puts(error_message);
            } else {
                puts("The book has been added!");
            }

            free(message);
            free(response);

        } else {
            puts("Page count field must be a number!");
            puts("START AGAIN!");
        }

    }
}

void delete_book_command(int sockfd, char **cookies, char *token) {
    if (cookies[0] == NULL) {
        puts("You are not logged in!");
    } else if (token == NULL) {
        puts("You don't have access to library!");
    } else {
        char *id = malloc(LINELEN * sizeof(char));
        printf("id=");
        fgets(id, LINELEN - 1, stdin);
        id[strlen(id) - 1] = '\0';
        
        char *url = malloc(LINELEN * sizeof(char));
        memset(url, 0, LINELEN);
        sprintf(url, "/api/v1/tema/library/books/%s",id);

        char *message = compute_delete_request(host_ip, url, NULL, NULL, 0, token);
        send_to_server(sockfd, message);

        char *response = receive_from_server(sockfd);

        JSON_Value *root_value = json_parse_string(basic_extract_json_response(response));
        JSON_Object *root_object = json_value_get_object(root_value);
        
        char *error_message = (char *)json_object_get_string(root_object, "error");

        if (error_message != NULL) {
            puts(error_message);
        } else {
            puts("The book has been deleted!");
        }

        free(message);
        free(response);
    }
}

void logout_command(int sockfd, char **cookies, char *token) {
    if (cookies[0] == NULL) {
        puts("You want to log out, but you are not logged in!");
    } else {
        char *url = "/api/v1/tema/auth/logout";

        char *message = compute_get_request(host_ip, url, NULL, cookies, 1, NULL);
        send_to_server(sockfd, message);

        char *response = receive_from_server(sockfd);

        JSON_Value *root_value = json_parse_string(basic_extract_json_response(response));
        JSON_Object *root_object = json_value_get_object(root_value);
        
        char *error_message = (char *)json_object_get_string(root_object, "error");

        if (error_message != NULL) {
            puts(error_message);
        } else {
            puts("You have logged out!");
        }

        cookies[0] = NULL;
        token = NULL;

        free(message);
        free(response);
    }
}