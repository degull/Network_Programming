#include <stdio.h>
#include <winsock2.h> // Windows ���� ���α׷����� ���� ��� ����
#include "Common.h"

int main(int argc, char* argv[])
{
    // ���� �ʱ�ȭ
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    u_short x1 = 0x1234;
    u_long  y1 = 0x12345678;
    u_short x2;
    u_long  y2;

    // ȣ��Ʈ ����Ʈ -> ��Ʈ��ũ ����Ʈ
    printf("[ȣ��Ʈ ����Ʈ -> ��Ʈ��ũ ����Ʈ]\n");
    printf("%#x -> %#x\n", x1, x2 = htons(x1));
    printf("%#x -> %#x\n", y1, y2 = htonl(y1));

    // ��Ʈ��ũ ����Ʈ -> ȣ��Ʈ ����Ʈ
    printf("\n[��Ʈ��ũ ����Ʈ -> ȣ��Ʈ ����Ʈ]\n");
    printf("%#x -> %#x\n", x2, ntohs(x2));
    printf("%#x -> %#x\n", y2, ntohl(y2));

    // �߸��� ��� ��
    printf("\n[�߸��� ��� ��]\n");
    printf("%#x -> %#x\n", x1, htonl(x1)); // �ùٸ��� ���� ����Դϴ�.

    // ���� ����
    WSACleanup();
    return 0;
}