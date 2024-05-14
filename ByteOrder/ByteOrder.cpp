#include <stdio.h>
#include <winsock2.h> // Windows 소켓 프로그래밍을 위한 헤더 파일
#include "Common.h"

int main(int argc, char* argv[])
{
    // 윈속 초기화
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    u_short x1 = 0x1234;
    u_long  y1 = 0x12345678;
    u_short x2;
    u_long  y2;

    // 호스트 바이트 -> 네트워크 바이트
    printf("[호스트 바이트 -> 네트워크 바이트]\n");
    printf("%#x -> %#x\n", x1, x2 = htons(x1));
    printf("%#x -> %#x\n", y1, y2 = htonl(y1));

    // 네트워크 바이트 -> 호스트 바이트
    printf("\n[네트워크 바이트 -> 호스트 바이트]\n");
    printf("%#x -> %#x\n", x2, ntohs(x2));
    printf("%#x -> %#x\n", y2, ntohl(y2));

    // 잘못된 사용 예
    printf("\n[잘못된 사용 예]\n");
    printf("%#x -> %#x\n", x1, htonl(x1)); // 올바르지 않은 사용입니다.

    // 윈속 종료
    WSACleanup();
    return 0;
}
