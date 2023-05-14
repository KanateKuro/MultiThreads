#include<iostream>
#include<Windows.h>
#include <immintrin.h>

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

#define N 8
float A[N][N], B[N][N], multi[N][N];
void AVXOptimization(float* A, float* B, float* C, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            __m256 c = _mm256_setzero_ps();
            for (int k = 0; k < n; k += 8) {
                __m256 a = _mm256_loadu_ps(&A[i * n + k]);
                __m256 b = _mm256_loadu_ps(&B[j * n + k]);
                c = _mm256_add_ps(c, _mm256_mul_ps(a, b));
            }
            __m128 c_low = _mm256_extractf128_ps(c, 0);
            __m128 c_high = _mm256_extractf128_ps(c, 1);
            c_low = _mm_add_ps(c_low, c_high);
            c_low = _mm_hadd_ps(c_low, c_low);
            c_low = _mm_hadd_ps(c_low, c_low);
            float temp;
            _mm_store_ss(&temp, c_low);
            C[i * n + j] = temp;
        }
    }
}

int main() {
    TimerCounter tc;
    for (long long i = 0; i < N; i++) {
        for (long long j = 0; j < N; j++) {
            A[i][j] = i + j;
            B[i][j] = i + j;
            multi[i][j] = 0;
        }
    }
    tc.Start();
    AVXOptimization(&A[0][0], &B[0][0], &multi[0][0], N);
    tc.Stop();
    cout << tc.dbTime;
}