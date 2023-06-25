#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unordered_map>
#include <chrono>
#include <thread>

#define PORT 8080
#define LOCKOUT_THRESHOLD 3
#define LOCKOUT_TIME chrono::seconds(10)

using namespace std;

unordered_map<string, int> attempt_counts;

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in address {};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    cout << "Server started. Waiting for connections..." << endl;

    int new_socket = accept(server_fd, nullptr, nullptr);

    unordered_map<string, string> users {
        {"alice", "password123"},
        {"bob", "qwerty"},
        {"charlie", "admin"}
    };

    char buffer[1024] = {0};

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        read(new_socket, buffer, 1024);
        string received = string(buffer);
        string::size_type separator = received.find(":");
        if (separator == string::npos) {
            send(new_socket, "incorrect", 9, 0);
            continue;
        }
        string username = received.substr(0, separator);
        string guess = received.substr(separator + 1);
        auto user = users.find(username);
        if (user == users.end() || guess != user->second) {
            attempt_counts[username]++;
            if (attempt_counts[username] >= LOCKOUT_THRESHOLD) {
                this_thread::sleep_for(LOCKOUT_TIME);
                attempt_counts[username] = 0;
            }
            send(new_socket, "incorrect", 9, 0);
        } else {
            attempt_counts[username] = 0;
            send(new_socket, "correct", 7, 0);
            users.erase(user);
        }
        if (users.empty()) {
            break;
        }
    }

    cout << "All passwords guessed correctly." << endl;
    return 0;
}
