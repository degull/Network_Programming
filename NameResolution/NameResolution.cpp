#include <stdio.h>
#include <winsock2.h> // Windows 소켓 프로그래밍을 위한 헤더 파일
#include <ws2tcpip.h> // getaddrinfo 함수를 사용하기 위한 헤더 파일
#include "Common.h"

#define TESTNAME "www.google.com"

// 도메인 이름 -> IPv4 주소
bool GetIPAddr(const char* name, struct in_addr* addr)
{
    struct addrinfo* result = NULL;
    struct addrinfo* ptr = NULL;
    struct addrinfo hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // 주어진 도메인 이름에 대한 주소 정보 가져오기
    int ret = getaddrinfo(name, NULL, &hints, &result);
    if (ret != 0) {
        fprintf(stderr, "getaddrinfo failed: %d\n", ret);
        return false;
    }

    // 첫 번째 결과 사용
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

// IPv4 주소 -> 도메인 이름
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

    // 'strncpy' 함수를 'strncpy_s' 함수로 변경
    if (strncpy_s(name, namelen, hostname, _TRUNCATE) != 0) {
        fprintf(stderr, "strncpy_s failed\n");
        return false;
    }

    return true;
}

int main(int argc, char* argv[])
{
    // 윈속 초기화
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    printf("도메인 이름(변환 전) = %s\n", TESTNAME);

    // 도메인 이름 -> IP 주소
    struct in_addr addr;
    if (GetIPAddr(TESTNAME, &addr)) {
        // 성공이면 결과 출력
        char str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &addr, str, sizeof(str));
        printf("IP 주소(변환 후) = %s\n", str);

        // IP 주소 -> 도메인 이름
        char name[NI_MAXHOST];
        if (GetDomainName(addr, name, sizeof(name))) {
            // 성공이면 결과 출력
            printf("도메인 이름(다시 변환 후) = %s\n", name);
        }
    }

    // 윈속 종료
    WSACleanup();
    return 0;
}
