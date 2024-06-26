#include <stdio.h>
#include <winsock2.h> // Windows 소켓 프로그래밍을 위한 헤더 파일
#include <ws2tcpip.h> // getaddrinfo 함수를 사용하기 위한 헤더 파일
#pragma comment(lib, "ws2_32.lib") // 링크할 라이브러리 지정

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

int main(int argc, char* argv[])
{
    int retval;

    // 명령행 인수가 있으면 IP 주소로 사용
    if (argc > 1) SERVERIP = argv[1];

    // 윈속 초기화
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    // 소켓 생성
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) err_quit("socket()");

    // connect()
    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    inet_pton(AF_INET, SERVERIP, &serveraddr.sin_addr);
    serveraddr.sin_port = htons(SERVERPORT);
    retval = connect(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) err_quit("connect()");

    // 데이터 통신에 사용할 변수
    char buf[BUFSIZE];
    const char* testdata[] = {
        "안녕하세요",
        "반가워요",
        "오늘따라 할 이야기가 많을 것 같네요",
        "저도 그렇네요",
    };
    int len;

    // 서버와 데이터 통신
    for (int i = 0; i < 4; i++) {
        // 데이터 입력(시뮬레이션)
        len = (int)strlen(testdata[i]);
        strncpy_s(buf, sizeof(buf), testdata[i], len);

        // 데이터 보내기(고정 길이)
        retval = send(sock, (char*)&len, sizeof(int), 0);
        if (retval == SOCKET_ERROR) {
            err_display("send()");
            break;
        }

        // 데이터 보내기(가변 길이)
        retval = send(sock, buf, len, 0);
        if (retval == SOCKET_ERROR) {
            err_display("send()");
            break;
        }
        printf("[TCP 클라이언트] %d+%d바이트를 보냈습니다.\n", (int)sizeof(int), retval);
    }

    // 소켓 닫기
    closesocket(sock);

    // 윈속 종료
    WSACleanup();
    return 0;
}
