#include <iostream>
#include <pthread.h>
#include<windows.h>
using namespace std;

class TimerCounter {
public:
    TimerCounter(void);//���캯��
    ~TimerCounter(void);//��������
private:
    LARGE_INTEGER startCount;//��¼��ʼʱ��

    LARGE_INTEGER endCount;//��¼����ʱ��

    LARGE_INTEGER freq;//����CPUʱ��Ƶ��
public:
    double dbTime;//�������е�ʱ�䱣��������

public:
    void Start();//�������ʼ�㴦��ʼ��ʱ
    void Stop();//�����������㴦������ʱ
};
TimerCounter::TimerCounter(void) {
    QueryPerformanceFrequency(&freq);//��ȡ����CPUʱ��Ƶ��
}
TimerCounter::~TimerCounter(void) {
}
void TimerCounter::Start() {
    QueryPerformanceCounter(&startCount);//��ʼ��ʱ
}
void TimerCounter::Stop() {
    QueryPerformanceCounter(&endCount);//ֹͣ��ʱ
    dbTime = ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)freq.QuadPart;//��ȡʱ���
}

#define N 512 // �����С
#define THREAD_NUM 12 // �߳���

int A[N][N], B[N][N], C[N][N]; // ����

struct ThreadArg {
    int start, end;
};

void* multiply(void* arg) {
    ThreadArg* threadArg = (ThreadArg*)arg;
    int start = threadArg->start, end = threadArg->end;
    for (int i = start; i < end; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    pthread_exit(NULL);
}

int main() {
    TimerCounter tc;
    // ��ʼ������
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[i][j] = i + j;
            B[i][j] = i + j;
            C[i][j] = 0;
        }
    }

    // �����߳�
    tc.Start();
    pthread_t threads[THREAD_NUM];
    ThreadArg threadArgs[THREAD_NUM];
    int step = N / THREAD_NUM;
    for (int i = 0; i < THREAD_NUM; i++) {
        threadArgs[i].start = i * step;
        threadArgs[i].end = (i + 1) * step;
        if (i == THREAD_NUM - 1) {
            threadArgs[i].end = N;
        }
        pthread_create(&threads[i], NULL, multiply, (void*)&threadArgs[i]);
    }

    // �ȴ��߳̽���
    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_join(threads[i], NULL);
    }
    tc.Stop();
    cout<<tc.dbTime;


    return 0;
}

