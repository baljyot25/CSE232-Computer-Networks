#include <iostream>
#include <thread>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include<sched.h>
#include <signal.h>
#include <fstream>
#include <string>
#include <dirent.h>
#include <cctype>
#include<vector>
#include <sstream>
#include <algorithm>
using namespace std;
int server_fd;

bool is_num(string &s) {
    for (char i : s) {
        if (!isdigit(i)) return 0;
    }
    return 1;
}

vector<string> split(string & s)
{
    vector<string> ans;
    stringstream ss(s); 
    string token;
     while (ss >> token) {
        ans.push_back(token); 
    }

    return ans;

}

bool cmp(vector<string> & a, vector<string> & b)
{
    int a1= stoi(a[2])+stoi(a[3]);
    int b1= stoi(b[2])+stoi(b[3]);
    return a1>b1;

}

void handle_client(int new_socket) {
    DIR *proc_dir;
    struct dirent *entry;

    // Open the /proc directory
    proc_dir = opendir("/proc");
    if (proc_dir == nullptr) {
        perror("opendir");
        return;
    }

    // Iterate over the entries in /proc
    vector<vector<string >> proc_info;
    while ((entry = readdir(proc_dir)) != nullptr) {
        string direc_name = entry->d_name;
        // cout<<direc_name<<endl;

        // Check if the directory name is a PID (i.e., all digits)
        if (is_num(direc_name)) {
            // cout<<"Yes"<<endl;
            // Construct the path to the /proc/[pid]/stat file
            string path = "/proc/" + direc_name + "/stat";

            // // Open the stat file
            ifstream stats(path);
            if (stats.is_open()) {
                string line;
                getline(stats, line);//only 1 line to read

                // Print the content of the stat file
                // cout <<line << endl;

                vector<string> line_array=split(line);

                // for (auto i : line_array) cout<<i<<endl;
                // cout<<endl;
                // cout<<line_array[0]<<" "<<line_array[1]<<" "<<stoi(line_array[13])<<" "<<line_array[14]<<endl; //13 -user , 14- kernel
                proc_info.push_back({line_array[0], line_array[1],line_array[13], line_array[14]});

                // Close the file
                stats.close();
                // break;
            }
        }

    }

    // Close the /proc directory
    closedir(proc_dir);

    partial_sort(proc_info.begin(), proc_info.begin()+2,proc_info.end(),cmp);
    for(int i=0 ;i<2;i++)
    {
        for (auto  j : proc_info[i])
        {
            cout<<j<<" ";
        }
        cout<<endl;
    }


    int cpu=sched_getcpu();

    char buffer[1024] = {0};
    const char *hello = "Hello from server";

    read(new_socket, buffer, 1024);
    std::cout << "Message received: " << buffer << "running on "<<cpu<<std::endl;
    send(new_socket, hello, strlen(hello), 0);
    std::cout << "Hello message sent" << std::endl;
    close(new_socket);
}


void signal_handler(int signum) {
    std::cout << "\nCaught signal " << signum << ", closing server socket..." << std::endl;
    close(server_fd); // Close the server socket properly
    exit(signum);     // Exit the program
}
   
int main() {
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    signal(SIGINT, signal_handler); 

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Socket failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8005);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        std::cerr << "Listen failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    while (true) {
        int new_socket;
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            std::cerr << "Accept failed" << std::endl;
            exit(EXIT_FAILURE);
        }
        std::thread t(handle_client, new_socket);
        t.detach();
    }

    close(server_fd);
    return 0;
}