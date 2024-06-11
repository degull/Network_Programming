#include <stdio.h>
#include <windows.h>

#define BUFSIZE 10

CONDITION_VARIABLE writeCond;
CONDITION_VARIABLE readCond;
CRITICAL_SECTION writeMutex;
CRITICAL_SECTION readMutex;
int writeDone = 0;
int readDone = 0;
int buf[BUFSIZE];

DWORD WINAPI WriteThread(LPVOID arg)
{
    for (int k = 1; k <= 500; k++) {
        // 읽기 완료 대기
        EnterCriticalSection(&readMutex);
        while (readDone == 0)
            SleepConditionVariableCS(&readCond, &readMutex, INFINITE);
        readDone = 0;
        LeaveCriticalSection(&readMutex);

        // 공유 버퍼에 데이터 저장
        for (int i = 0; i < BUFSIZE; i++)
            buf[i] = k;

        // 쓰기 완료 알림
        EnterCriticalSection(&writeMutex);
        writeDone = 1;
        LeaveCriticalSection(&writeMutex);
        WakeConditionVariable(&writeCond);
    }
    return 0;
}

DWORD WINAPI ReadThread(LPVOID arg)
{
    while (1) {
        // 쓰기 완료 대기
        EnterCriticalSection(&writeMutex);
        while (writeDone == 0)
            SleepConditionVariableCS(&writeCond, &writeMutex, INFINITE);
        writeDone = 0;
        LeaveCriticalSection(&writeMutex);

        // 읽은 데이터 출력 후 버퍼를 0으로 초기화
        printf("Thread %4d:\t", GetCurrentThreadId());
        for (int i = 0; i < BUFSIZE; i++)
            printf("%3d ", buf[i]);
        printf("\n");
        memset(buf, 0, sizeof(buf));

        // 읽기 완료 알림
        EnterCriticalSection(&readMutex);
        readDone = 1;
        LeaveCriticalSection(&readMutex);
        WakeConditionVariable(&readCond);
    }
    return 0;
}

int main(int argc, char* argv[])
{
    // 조건 변수와 뮤텍스 초기화
    InitializeConditionVariable(&writeCond);
    InitializeConditionVariable(&readCond);
    InitializeCriticalSection(&writeMutex);
    InitializeCriticalSection(&readMutex);

    // 스레드 세 개 생성
    HANDLE hThread[3];
    DWORD dwThreadId;
    hThread[0] = CreateThread(NULL, 0, WriteThread, NULL, 0, &dwThreadId);
    hThread[1] = CreateThread(NULL, 0, ReadThread, NULL, 0, &dwThreadId);
    hThread[2] = CreateThread(NULL, 0, ReadThread, NULL, 0, &dwThreadId);

    // 읽기 완료 알림
    EnterCriticalSection(&readMutex);
    readDone = 1;
    LeaveCriticalSection(&readMutex);
    WakeConditionVariable(&readCond);

    // 스레드 세 개 종료 대기
    WaitForMultipleObjects(3, hThread, TRUE, INFINITE);

    // 조건 변수와 뮤텍스 제거
    DeleteCriticalSection(&writeMutex);
    DeleteCriticalSection(&readMutex);
    return 0;
}
