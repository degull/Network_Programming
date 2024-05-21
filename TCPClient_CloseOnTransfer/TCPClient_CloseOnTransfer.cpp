#include <stdio.h>
#include <winsock2.h> // Windows ���� ���α׷����� ���� ��� ����
#include <ws2tcpip.h> // getaddrinfo �Լ��� ����ϱ� ���� ��� ����
#pragma comment(lib, "ws2_32.lib") // ��ũ�� ���̺귯�� ����

char* SERVERIP = (char*)"127.0.0.1";
#define SERVERPORT 9000
#define BUFSIZE    50

void err_quit(const char* msg) {
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

void err_display(const char* msg) {
    fprintf(stderr, "%s: %d\n", msg, WSAGetLastError());
}

int main(int argc, char* argv[]) {
    int retval;

    // ����� �μ��� ������ IP �ּҷ� ���
    if (argc > 1) SERVERIP = argv[1];

    // ���� �ʱ�ȭ
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    // connect() ȣ�⿡ ����� ����
    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    inet_pton(AF_INET, SERVERIP, &serveraddr.sin_addr);
    serveraddr.sin_port = htons(SERVERPORT);

    // ������ ��ſ� ����� ����
    char buf[BUFSIZE];
    const char* testdata[] = {
        "�ȳ��ϼ���",
        "�ݰ�����",
        "���õ��� �� �̾߱Ⱑ ���� �� ���׿�",
        "���� �׷��׿�",
    };
    int len;

    // ������ ������ ���
    for (int i = 0; i < 4; i++) {
        // ���� ����
        SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == INVALID_SOCKET) err_quit("socket()");

        // connect()
        retval = connect(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
        if (retval == SOCKET_ERROR) err_quit("connect()");

        // ������ �Է�(�ùķ��̼�)
        len = (int)strlen(testdata[i]);
        strncpy_s(buf, sizeof(buf), testdata[i], len);

        // ������ ������
        retval = send(sock, buf, len, 0);
        if (retval == SOCKET_ERROR) {
            err_display("send()");
            break;
        }
        printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\n", retval);

        // ���� �ݱ�
        closesocket(sock);
    }

    // ���� ����
    WSACleanup();
    return 0;
}
