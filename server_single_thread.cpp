#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
using namespace std;

int main()
{
    int server_fd , new_socket;
    struct sockaddr_in socket_address;
    int addrlen = sizeof(socket_address);
    char buffer[1024] = {0};
    const char *hello = "Hello from server";
    // AF_INET for protocol family , in our case ipv4 .
    // SOCK_STREAM means TCP connection
    // 0 -  ip protocol
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) { // AF_INET for protocol family , in our case ipv4 . SOCK_STREAM means TCP connection
        std::cerr << "Socket failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    //    when a socket is created with socket(2), it exists in a name space (ad‐
    //    dress  family)  but  has no address assigned to it.  bind() assigns the
    //    address specified by addr to the socket referred to  by  the  file  de‐
    //    scriptor  sockfd.  addrlen specifies the size, in bytes, of the address
    //    structure pointed to by addr.  Traditionally, this operation is  called
    //    “assigning a name to a socket”. -MAN page of "bind"

    socket_address.sin_family = AF_INET; // ipv4 used
    socket_address.sin_addr.s_addr = INADDR_ANY; // this means server will bind any ip address sending request to the server
    socket_address.sin_port = htons(8002); //convert to big endian 

    if (bind(server_fd, (struct sockaddr *)&socket_address, sizeof(socket_address)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    // if (getsockname(server_fd, (struct sockaddr *)&socket_address, (socklen_t*)(&addrlen)) == -1) {
    //     perror("getsockname failed");
    //     exit(EXIT_FAILURE);
    // }

    // // Convert the IP address to a readable format
    // char *ip = inet_ntoa(socket_address.sin_addr);
    // printf("Server is running on IP: %s\n", ip);
    // printf("Port: %d\n", ntohs(socket_address.sin_port));

    if (listen(server_fd, 3) < 0) {
        std::cerr << "Listen failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    cout<<"listening"<<endl;
      if ((new_socket = accept(server_fd, (struct sockaddr *)&socket_address, (socklen_t*)&addrlen)) < 0) {
        std::cerr << "Accept failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    read(new_socket, buffer, 1024);
    std::cout << "Message received: " << buffer << std::endl;
    send(new_socket, hello, strlen(hello), 0);
    std::cout << "Hello message sent" << std::endl;

    close(new_socket);
    close(server_fd);

}