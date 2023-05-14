#include <iostream>
#include <immintrin.h>
#include <pthread.h>
#include<Windows.h>

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
const int N = 4096 ;
const int NUM_THREADS = 12;

float *A, *B, *C;
int current_row = 0;
pthread_mutex_t mutex;

void* matrix_multiply(void* arg) {
    int thread_id = *(int*)arg;
    while (true) {
        pthread_mutex_lock(&mutex);
        int row = current_row;
        current_row++;
        pthread_mutex_unlock(&mutex);
        if (row >= N) {
            break;
        }
        for (int j = 0; j < N; j++) {
            __m128 c = _mm_setzero_ps();
            for (int k = 0; k < N; k += 4) {
                __m128 a = _mm_load_ps(&A[row * N + k]);
                __m128 b = _mm_load_ps(&B[j * N + k]);
                c = _mm_add_ps(c, _mm_mul_ps(a, b));
            }
            C[row * N + j] = ((float*)&c)[0] + ((float*)&c)[1] + ((float*)&c)[2] + ((float*)&c)[3];
        }
    }
    pthread_exit(NULL);
}

int main() {
    TimerCounter tc;
    A = new float[N * N];
    B = new float[N * N];
    C = new float[N * N];
    // 初始化 A 和 B 矩阵
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[i * N + j] = i + j;
            B[i * N + j] = i + j;
        }
    }
    tc.Start();
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];
    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, matrix_multiply, (void*)&thread_ids[i]);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    tc.Stop();
    cout<<tc.dbTime;
    delete[] A;
    delete[] B;
    delete[] C;
    return 0;
}
