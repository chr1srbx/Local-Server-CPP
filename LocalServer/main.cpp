#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "jsonHandler.h"
#include "misc.h"

#pragma comment(lib, "Ws2_32.lib")

#define PORT 54000
#define BUFFER_SIZE 1024

int main() {
    try {
        WSADATA wsaData;
        int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (result != 0) {
            std::cerr << "WSAStartup failed: " << result << std::endl;
            return 1;
        }

        SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == INVALID_SOCKET) {
            std::cerr << "Socket creation failed!" << std::endl;
            WSACleanup();
            return 1;
        }

        sockaddr_in serverAddr{};
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(PORT);

        if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "Binding failed!" << std::endl;
            closesocket(serverSocket);
            WSACleanup();
            return 1;
        }

        if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
            std::cerr << "Listen failed!" << std::endl;
            closesocket(serverSocket);
            WSACleanup();
            return 1;
        }

        std::cout << "Server listening on port " << PORT << "...\n";

        while (true) {
            SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
            if (clientSocket == INVALID_SOCKET) {
                std::cerr << "Accept failed!" << std::endl;
                continue;
            }

            std::cout << "Client connected!\n";

            char buffer[BUFFER_SIZE];
            int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
            if (bytesReceived > 0) {
                buffer[bytesReceived] = '\0';
                std::cout << "Received from client: " << buffer << std::endl;
                std::string response = processRequest(buffer);

                // Send processed response to client
                send(clientSocket, response.c_str(), response.length(), 0);

                std::cout << "Sent to client: " << response << std::endl;
            }

            closesocket(clientSocket);
        }

        closesocket(serverSocket);
        WSACleanup();
    }
    catch (const std::exception& e) {
        std::cout << "Error occured" << e.what();
    }
    return 0;
}
