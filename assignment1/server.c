// Server side C/C++ program to demonstrate Socket programming

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <pwd.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/wait.h>

#define PORT 80
int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[102] = {0};
    char *hello = "Hello from server";
    struct passwd *pd;

    printf("execve=0x%p\n", execve);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Attaching socket to port 80
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    // Forcefully attaching socket to the port 80
    if (bind(server_fd, (struct sockaddr *)&address,
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    // get "nobody" info
    pd = getpwnam("nobody");
    if (pd == NULL) {
        printf("Cannot find UID for name nobody\n"); 
        exit(EXIT_FAILURE);
    }
    printf("nobody uid: %d\n", pd->pw_uid);

    // fork before listening
    pid_t pid = fork();
    if (pid < 0) { /* error occurred */
        printf("\nFork failed. \n");
   	    return -1; 
    }
    // run under forked process
    if (pid == 0)
    {
        //set child process privilege to nobody
        setuid(pd->pw_uid);
        // verify current uid
        uid_t uid = getuid();
        printf("Listening with uid: %d\n", uid);
        if (listen(server_fd, 3) < 0)
        {
            perror("listen");
            exit(EXIT_FAILURE);
        }
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                        (socklen_t*)&addrlen))<0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        valread = read( new_socket , buffer, 1024);
        printf("%s\n",buffer );
        send(new_socket , hello , strlen(hello) , 0 );
        printf("Hello message sent\n");
        return 0;
    }
    else
    {
        // master wait for child process to complete
        wait(NULL);
        printf("Child Complete\n");
    }
}
