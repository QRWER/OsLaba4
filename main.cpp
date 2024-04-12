#pragma comment(lib, "ws2_32.lib")
#include <iostream>
#include <stack>
#include<winsock2.h>
#pragma warning(disable: 4996)

bool printCausedBy(int Result, const char* nameOfOper) {
    if (!Result) {
        std::cerr << "Connection closed.\n";
        return false;
    }
    else if (Result < 0) {
        std::cout << nameOfOper;
        std::cerr << " failed: " << WSAGetLastError() << '\n';
        return false;
    }
    return true;
}

void Error(std::string msg, SOCKET listen){
    std::cout << msg << WSAGetLastError() << '\n';
    closesocket(listen);
    WSACleanup();
}

void SocketError(SOCKET newConnection){
    if (closesocket(newConnection) == SOCKET_ERROR)
        std::cerr << "Failed to terminate connection.\n Error code: " << WSAGetLastError();
}

std::string CheckCorrect(std::string str) {
    std::stack<char> st;
    for (int i = 0; i < str.size(); i++) {
        switch (str[i]) {
            case '{':
                st.push('}');
                break;
            case '[':
                st.push(']');
                break;
            case '(':
                st.push(')');
                break;
            case '<':
                st.push('>');
                break;
            default:
                if (st.empty() || str[i] != st.top())
                    return "False input\n";
                else
                    st.pop();
        }
    }
    return const_cast<char *>(st.empty() ? "True input\n" : "False input\n");
}

int main() {
    WSAData wsaData{};
    WORD DLLVersion = MAKEWORD(2, 1);
    if (WSAStartup(DLLVersion, &wsaData) != 0) { // проверка подключения
        std::cerr << "Error: failed to link library.\n";
        return 1;
    }
    SOCKADDR_IN addr;
    int sizeOfAddr = sizeof(addr);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(1111);
    addr.sin_family = AF_INET;
    SOCKET sListen = socket(AF_INET, SOCK_STREAM, 0); //сокет для прослушивания порта
    if (bind(sListen, (SOCKADDR*)&addr, sizeOfAddr) == SOCKET_ERROR) { //привязка адреса сокету
        Error("Error bind ", sListen);
        return 1;
    } //подключение прослушивания с максимальной очередью
    if (listen(sListen, SOMAXCONN) == SOCKET_ERROR) {
        Error("Listen failed;\n", sListen);
        return 1;
    }
    while(true) {
        SOCKET newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeOfAddr);
        if (!newConnection) { //проверяем, произошло ли соединение с клиентом
            std::cout << "Error in connect!\n";
            SocketError(sListen);
            return 1;
        }
        else {
            std::cout << "New client connected\n";
            char buffer[2048];
            while(printCausedBy(recv(newConnection, buffer, 2048, 0), "Recv")) {
                printCausedBy(send(newConnection, CheckCorrect(buffer).data(), 2048, 0), "Send");
            }
        }
        std::cout << ("Client disconnected\n");
        SocketError(newConnection);
    }
}