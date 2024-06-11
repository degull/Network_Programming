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
        // �б� �Ϸ� ���
        EnterCriticalSection(&readMutex);
        while (readDone == 0)
            SleepConditionVariableCS(&readCond, &readMutex, INFINITE);
        readDone = 0;
        LeaveCriticalSection(&readMutex);

        // ���� ���ۿ� ������ ����
        for (int i = 0; i < BUFSIZE; i++)
            buf[i] = k;

        // ���� �Ϸ� �˸�
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
        // ���� �Ϸ� ���
        EnterCriticalSection(&writeMutex);
        while (writeDone == 0)
            SleepConditionVariableCS(&writeCond, &writeMutex, INFINITE);
        writeDone = 0;
        LeaveCriticalSection(&writeMutex);

        // ���� ������ ��� �� ���۸� 0���� �ʱ�ȭ
        printf("Thread %4d:\t", GetCurrentThreadId());
        for (int i = 0; i < BUFSIZE; i++)
            printf("%3d ", buf[i]);
        printf("\n");
        memset(buf, 0, sizeof(buf));

        // �б� �Ϸ� �˸�
        EnterCriticalSection(&readMutex);
        readDone = 1;
        LeaveCriticalSection(&readMutex);
        WakeConditionVariable(&readCond);
    }
    return 0;
}

int main(int argc, char* argv[])
{
    // ���� ������ ���ؽ� �ʱ�ȭ
    InitializeConditionVariable(&writeCond);
    InitializeConditionVariable(&readCond);
    InitializeCriticalSection(&writeMutex);
    InitializeCriticalSection(&readMutex);

    // ������ �� �� ����
    HANDLE hThread[3];
    DWORD dwThreadId;
    hThread[0] = CreateThread(NULL, 0, WriteThread, NULL, 0, &dwThreadId);
    hThread[1] = CreateThread(NULL, 0, ReadThread, NULL, 0, &dwThreadId);
    hThread[2] = CreateThread(NULL, 0, ReadThread, NULL, 0, &dwThreadId);

    // �б� �Ϸ� �˸�
    EnterCriticalSection(&readMutex);
    readDone = 1;
    LeaveCriticalSection(&readMutex);
    WakeConditionVariable(&readCond);

    // ������ �� �� ���� ���
    WaitForMultipleObjects(3, hThread, TRUE, INFINITE);

    // ���� ������ ���ؽ� ����
    DeleteCriticalSection(&writeMutex);
    DeleteCriticalSection(&readMutex);
    return 0;
}
