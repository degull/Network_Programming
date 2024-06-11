#include <stdio.h>
#include <windows.h>

#define MAXCNT 100000000
int count = 0;
CRITICAL_SECTION mutex;

DWORD WINAPI MyThread1(LPVOID arg)
{
    for (int i = 0; i < MAXCNT; i++) {
        EnterCriticalSection(&mutex);
        count += 2;
        LeaveCriticalSection(&mutex);
    }
    return 0;
}

DWORD WINAPI MyThread2(LPVOID arg)
{
    for (int i = 0; i < MAXCNT; i++) {
        EnterCriticalSection(&mutex);
        count -= 2;
        LeaveCriticalSection(&mutex);
    }
    return 0;
}

int main(int argc, char* argv[])
{
    // Critical Section 초기화
    InitializeCriticalSection(&mutex);

    // 스레드 두 개 생성
    HANDLE hThread[2];
    DWORD dwThreadId;
    hThread[0] = CreateThread(NULL, 0, MyThread1, NULL, 0, &dwThreadId);
    hThread[1] = CreateThread(NULL, 0, MyThread2, NULL, 0, &dwThreadId);

    // 스레드 두 개 종료 대기
    WaitForMultipleObjects(2, hThread, TRUE, INFINITE);

    // Critical Section 삭제
    DeleteCriticalSection(&mutex);

    // 결과 출력
    printf("count = %d\n", count);

    // 스레드 핸들 닫기
    CloseHandle(hThread[0]);
    CloseHandle(hThread[1]);

    return 0;
}
