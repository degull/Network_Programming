#include <stdio.h>
#include <winsock2.h> // Windows ���� ���α׷����� ���� ��� ����
#include <ws2tcpip.h> // getaddrinfo �Լ��� ����ϱ� ���� ��� ����
#include "Common.h"

#define TESTNAME "www.google.com"

// ������ �̸� -> IPv4 �ּ�
bool GetIPAddr(const char* name, struct in_addr* addr)
{
    struct addrinfo* result = NULL;
    struct addrinfo* ptr = NULL;
    struct addrinfo hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // �־��� ������ �̸��� ���� �ּ� ���� ��������
    int ret = getaddrinfo(name, NULL, &hints, &result);
    if (ret != 0) {
        fprintf(stderr, "getaddrinfo failed: %d\n", ret);
        return false;
    }

    // ù ��° ��� ���
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        if (ptr->ai_family == AF_INET) {
            struct sockaddr_in* sockaddr_ipv4 = (struct sockaddr_in*)ptr->ai_addr;
            memcpy(addr, &sockaddr_ipv4->sin_addr, sizeof(struct in_addr));
            break;
        }
    }

    freeaddrinfo(result);
    return true;
}

// IPv4 �ּ� -> ������ �̸�
bool GetDomainName(struct in_addr addr, char* name, int namelen)
{
    struct sockaddr_in sockaddr_ipv4;
    sockaddr_ipv4.sin_family = AF_INET;
    sockaddr_ipv4.sin_port = htons(80);
    sockaddr_ipv4.sin_addr = addr;

    char hostname[NI_MAXHOST];
    int ret = getnameinfo((struct sockaddr*)&sockaddr_ipv4, sizeof(sockaddr_ipv4), hostname, NI_MAXHOST, NULL, 0, 0);
    if (ret != 0) {
        fprintf(stderr, "getnameinfo failed: %d\n", ret);
        return false;
    }

    // 'strncpy' �Լ��� 'strncpy_s' �Լ��� ����
    if (strncpy_s(name, namelen, hostname, _TRUNCATE) != 0) {
        fprintf(stderr, "strncpy_s failed\n");
        return false;
    }

    return true;
}

int main(int argc, char* argv[])
{
    // ���� �ʱ�ȭ
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    printf("������ �̸�(��ȯ ��) = %s\n", TESTNAME);

    // ������ �̸� -> IP �ּ�
    struct in_addr addr;
    if (GetIPAddr(TESTNAME, &addr)) {
        // �����̸� ��� ���
        char str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &addr, str, sizeof(str));
        printf("IP �ּ�(��ȯ ��) = %s\n", str);

        // IP �ּ� -> ������ �̸�
        char name[NI_MAXHOST];
        if (GetDomainName(addr, name, sizeof(name))) {
            // �����̸� ��� ���
            printf("������ �̸�(�ٽ� ��ȯ ��) = %s\n", name);
        }
    }

    // ���� ����
    WSACleanup();
    return 0;
}
