#if defined(_WIN32) /*for windows base platform*/
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else /*for linux based platform*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#endif

/*defining some macros, which abstract out some of the difference between the
Berkeley socket(linux base platform) and Winsock(windows base platform) APIs*/
#if defined(_WIN32)
#define ISVALIDSOCKET(s) ((s) != INVALID_SOCKET)
#define CLOSESOCKET(s) closesocket(s)
#define GETSOCKETERRNO() (WSAGetLastError())
#else
#define ISVALIDSOCKET(s) ((s) >= 0)
#define CLOSESOCKET(s) close(s)
#define SOCKET int
#define GETSOCKETERRNO() (errno)
#endif

/*C headers*/
#include <stdio.h>
#include <string.h>
#include <time.h>



int main() {
#if defined(_WIN32)/*if Windows base platform*/
    WSADATA d;
    if (WSAStartup(MAKEWORD(2, 2), &d)) {
        fprintf(stderr, "Failed to initialize.\n");
    return 1;
    }
#endif


    printf("Configuring local address...\n");
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints)); /*to have zeroed out hints*/
    hints.ai_family = AF_INET; /*looking for an IPv4 address*/
    hints.ai_socktype = SOCK_STREAM; /*using TCP.*/
    hints.ai_flags = AI_PASSIVE; /*asking getaddrinfo() to set up the address, so we listen on any available network interface*/

    /*Once hints is set up properly, we declare a pointer to a struct addrinfo structure,
    which holds the return information from getaddrinfo().*/
    struct addrinfo *bind_address;
    getaddrinfo(0, "8080", &hints, &bind_address);

    /*creating socket*/
    printf("Creating socket...\n");
    SOCKET socket_listen;
    socket_listen = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol);

    /*checking wether socket() was successful*/
    if (!ISVALIDSOCKET(socket_listen)) {
        fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    /*as the socket has been created successfully, we can call bind() to associate it with our
    address from getaddrinfo()*/
    printf("Binding socket to local address...\n");
    if (bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen)) {
        fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO()); /*if fails print error number*/
        return 1;
    }
    
    freeaddrinfo(bind_address); /*to release the address memory.*/

    /*Once the socket has been created and bound to a local address, we can cause it to start
    listening for connections with the listen() function:*/
    printf("Listening...\n");
    if (listen(socket_listen, 10) < 0) { /*10 means how many connections it is allowed to queue up*/
        fprintf(stderr, "listen() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    /*After the socket has begun listening for connections, we can accept any incoming
    connection with the accept() function*/
    printf("Waiting for connection...\n");
    struct sockaddr_storage client_address; /* to store the address info for the connecting client*/
    socklen_t client_len = sizeof(client_address);
    SOCKET socket_client = accept(socket_listen, (struct sockaddr*) &client_address, &client_len);
    /*checking for errors by detecting if client_socket is a valid socket*/
    if (!ISVALIDSOCKET(socket_client)) {
        fprintf(stderr, "accept() failed. (%d)\n", GETSOCKETERRNO());
    return 1;
    }

    /*a TCP connection has been established to a remote client, printing the
    client's address to the console*/
    printf("Client is connected... ");
    char address_buffer[100];
    getnameinfo((struct sockaddr*)&client_address, client_len, address_buffer, sizeof(address_buffer), 0, 0, NI_NUMERICHOST);/*NI_NUMERICHOST flag 
                                                                                specifies that we want to see the hostname as an IP address*/
    printf("%s\n", address_buffer);

    /*As we are programming a web server, we expect the client to
    send us an HTTP request.*/
    printf("Reading request...\n");
    char request[1024];
    int bytes_received = recv(socket_client, request, 1024, 0);
    printf("Received %d bytes.\n", bytes_received);

    /*printint the browser's request to the console*/
    printf("%.*s", bytes_received, request);

    /*Asthe web browser has sent its request, we can send our response back:*/
    printf("Sending response...\n");
    const char*response = 
    "HTTP/1.1 200 OK\r\n"
    "Connection: close\r\n"
    "Content-Type: text/plain\r\n\r\n"
    "Local time is: ";
    int bytes_sent = send(socket_client, response, strlen(response), 0);
    printf("Sen t %d of %d bytes.\n", bytes_sent, (int)strlen(response));

    /*After the HTTP header and the beginning of our message is sent, we can send the actual time.*/
    time_t timer;
    time(&timer);
    char *time_msg = ctime(&timer);
    bytes_sent = send(socket_client, time_msg, strlen(time_msg), 0);
    printf("Sent %d of %d bytes.\n", bytes_sent, (int)strlen(time_msg));

    /*closing the client connection*/
    printf("Closing connection...\n");
    CLOSESOCKET(socket_client);

    /*closing socket*/
    printf("Closing listening socket...\n");
    CLOSESOCKET(socket_listen);
#if defined(_WIN32)
    WSACleanup();
#endif
    printf("Finished.\n");
    return 0;
}
