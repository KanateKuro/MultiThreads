#include <iostream>
#include <pthread.h>
#include<windows.h>
using namespace std;

class TimerCounter {
public:
    TimerCounter(void);//构造函数
    ~TimerCounter(void);//析构函数
private:
    LARGE_INTEGER startCount;//记录开始时间

    LARGE_INTEGER endCount;//记录结束时间

    LARGE_INTEGER freq;//本机CPU时钟频率
public:
    double dbTime;//程序运行的时间保存在这里

public:
    void Start();//被测程序开始点处开始计时
    void Stop();//被测程序结束点处结束计时
};
TimerCounter::TimerCounter(void) {
    QueryPerformanceFrequency(&freq);//获取主机CPU时钟频率
}
TimerCounter::~TimerCounter(void) {
}
void TimerCounter::Start() {
    QueryPerformanceCounter(&startCount);//开始计时
}
void TimerCounter::Stop() {
    QueryPerformanceCounter(&endCount);//停止计时
    dbTime = ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)freq.QuadPart;//获取时间差
}

#define N 512 // 矩阵大小
#define THREAD_NUM 12 // 线程数

int A[N][N], B[N][N], C[N][N]; // 矩阵

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
    // 初始化矩阵
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[i][j] = i + j;
            B[i][j] = i + j;
            C[i][j] = 0;
        }
    }

    // 创建线程
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

    // 等待线程结束
    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_join(threads[i], NULL);
    }
    tc.Stop();
    cout<<tc.dbTime;


    return 0;
}

