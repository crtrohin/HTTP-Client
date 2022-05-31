#ifndef _CLIENT_UTILS_
#define _CLIENT_UTILS_

void register_command(int sockfd, char **cokies);
char **login_command(int sockfd, char **cookies);
char *enter_library_command(int sockfd, char **cookies, char *token);
void get_books_command(int sockfd, char **cookies, char *token);
void get_book_command(int sockfd, char **cookies, char *token);
void add_book_command(int sockfd, char **cookies, char *token);
void delete_book_command(int sockfd, char **cookies, char *token);
void logout_command(int sockfd, char **cookies, char *token);

#endif
