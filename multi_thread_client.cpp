#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sched.h> 
#include <arpa/inet.h>
#include <thread>
#include <vector>
// #include <bits/stdc++.h>

using namespace std;

// need to do task set 
void client_task(int i) {
    int sock = 0;
    cout<<"I am thread "<<i<<endl;
    int cpu = sched_getcpu();
    std::cout << "Thread " << i << " is running on CPU: " << cpu << std::endl;
    struct sockaddr_in serv_addr;
    // const char *hello = "Hello from client"+to_string(i);
    string message = "Hello from client " + to_string(i);
    const char *hello = message.c_str();
    char buffer[1024] = {0};
    // cout<<hello<<endl;

    std::thread::id this_id = std::this_thread::get_id();

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Thread " << this_id << ": Socket creation error" << std::endl;
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8005);

    if (inet_pton(AF_INET, "192.168.1.15", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Thread " << this_id << ": Invalid address/Address not supported" << std::endl;
        return;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Thread " << this_id << ": Connection Failed" << std::endl;
        return;
    }

    send(sock, hello, strlen(hello), 0);
    std::cout << "Thread " << this_id << ": Hello message sent" << std::endl;
    read(sock, buffer, 1024);
    std::cout << "Thread " << this_id << ": Message received: " << buffer << std::endl;

    close(sock);
}

int main(int argc , char * argv[]) {

    cout<<argc<<endl;
    int number_of_clients=2;//default
    if (argc>1)
    {
        number_of_clients=stoi(argv[1]);
    }
    cout<<number_of_clients<<endl;

    vector<thread> threads;

    for(int i =0 ; i<number_of_clients;i++)
    {

        threads.push_back(thread(client_task,i));

    }
    for(auto &i :  threads)
    {
        i.join();
    }
    

    return 0;
}