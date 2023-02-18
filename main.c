#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080

// thread function
void *thread_func(void *arg) {
    int *num = (int *)arg;
    printf("Thread %d started\n", *num);
    sleep(1);
    printf("Thread %d finished\n", *num);
    return NULL;
}

int main() {
    // create a thread
    pthread_t thread1;
    int num1 = 1;
    if (pthread_create(&thread1, NULL, thread_func, &num1)) {
        perror("Error creating thread");
        return 1;
    }

    // create a process
    pid_t pid = fork();
    if (pid == -1) {
        perror("Error creating process");
        return 1;
    } else if (pid == 0) {
        // child process
        printf("Child process started\n");
        sleep(1);
        printf("Child process finished\n");
        exit(0);
    } else {
        // parent process
        printf("Parent process waiting for child\n");
        int status;
        wait(&status);
        printf("Parent process resumed\n");
    }

    // create a socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Error creating socket");
        return 1;
    }

    // connect to a server
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address");
        return 1;
    }
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        return 1;
    }
    printf("Connected to server\n");

    // send and receive data
    char *message = "Hello, server!";
    send(sockfd, message, strlen(message), 0);
    printf("Sent message: %s\n", message);
    char buffer[1024] = {0};
    int n = recv(sockfd, buffer, 1024, 0);
    printf("Received message: %s\n", buffer);

    // close the socket
    close(sockfd);

    // join the thread
    if (pthread_join(thread1, NULL)) {
        perror("Error joining thread");
        return 1;
    }

    return 0;
}
