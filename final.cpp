// title: memset, OS homework
// author: Jiahong Ma
// date: 2020/12/03
// coding：UTF-8

#include <iostream>
#include <fstream>
#include <bitset>
#include <windows.h>

#define MAX_LENGTH 200000//内存替换的最大长度
#define LOG_LENGTH (MAX_LENGTH / 10)//每隔LOG_LENGTH输出一个log日志信号
#define CH 25//需要替换的字符
#define TIMES 30 //1e5//函数执行的次数

void mymemset(void *s, int ch, size_t n);
void originmemset(void *s, int ch, size_t n);

//进行实验，统计mymemset(),memset()所用时间
int main()
{
    std::ofstream outfile;
    outfile.open("result.txt");//将结果输出

    char a[MAX_LENGTH];//被替换的内存空间的地址
    //int a[MAX_LENGTH/4];

    //时间统计相关函数
    LARGE_INTEGER cpuFreq;
    LARGE_INTEGER startTime;
    LARGE_INTEGER endTime;
    double runTime, perTime;

    for (int length = 0; length < MAX_LENGTH; length++)
    {
        //-------------------------------------------------------

        //mymemset(a, CH, length);
        runTime = 0;
        for (int i = 0; i < TIMES; i++)
        {
            QueryPerformanceFrequency(&cpuFreq);
            QueryPerformanceCounter(&startTime);
            mymemset(a, CH, length);
            QueryPerformanceCounter(&endTime);
            perTime = (((endTime.QuadPart - startTime.QuadPart) * 1000.0f) / cpuFreq.QuadPart);
            runTime += perTime;
        }

        std::cout << "mymemset:(" << runTime / TIMES * 1e3 << ")us\t";
        outfile << "mymemset:(" << runTime / TIMES * 1e3 << ")us\t";
        //------------------------------------------------------

        //-------------------------------------------------------

        //originmemset(a, CH, length);
        runTime = 0;
        for (int i = 0; i < TIMES; i++)
        {
            QueryPerformanceFrequency(&cpuFreq);
            QueryPerformanceCounter(&startTime);
            originmemset(a, CH, length);
            QueryPerformanceCounter(&endTime);
            perTime = (((endTime.QuadPart - startTime.QuadPart) * 1000.0f) / cpuFreq.QuadPart);
            runTime += perTime;
        }

        std::cout << "originmemset:(" << runTime / TIMES * 1e3 << ")us\n";
        outfile << "originmemset:(" << runTime / TIMES * 1e3 << ")us\n";
        //------------------------------------------------------

        if (0 == length % LOG_LENGTH)
        {
            std::cout << "-----------------LOG:" << length / float(MAX_LENGTH) << "-----------------\n";
            outfile << "-----------------LOG:" << length / float(MAX_LENGTH) << "-----------------\n";
        }
    }

    outfile.close();
    return 0;
}

void mymemset(void *s, int ch, size_t n)
{
    //提前定义一些循环需要的变量并赋值
    const short BitPerByte = 8;

    size_t cnt = 0;

    //为了确保函数的正确性，所有长度均由sizeof()得到，即便sizeof(unsigned long long)!=8
    //函数也能正确执行
    short len_ch = sizeof(unsigned char);
    short len_ll = sizeof(unsigned long long);

    unsigned char u_ch = (unsigned char)ch;
    unsigned long long u_ll = (unsigned long long)u_ch;

    unsigned char *schar = static_cast<unsigned char *>(s);
    unsigned long long *sullong;

    //将1Byte长度的usigned char拓展为8Byte长度的unsigned long long便于后续替换
    for (int i = 1; i < len_ll / len_ch; i += len_ch)
    {
        u_ll += ((unsigned long long)u_ch << (i * BitPerByte));
        //std::cout << (std::bitset<64>)u_ll << std::endl;
    }

    //对于当前地址不为8的倍数的数据，函数每次只替换1个Byte的数据
    while ((unsigned long long)s % (unsigned long long)len_ll != 0 && cnt < n)
    {
        *schar++ = u_ch;
        cnt++;
        //std::cout << "conditionA\n";
    }

    sullong = (unsigned long long *)schar;

    //对于当前地址为8的倍数的数据，函数每次替换8个Byte的数据，直到剩余待替换的长度小于8Byte
    while (n - cnt >= len_ll)
    {
        *sullong++ = u_ll;
        cnt += len_ll;
        //std::cout << "conditionB\n";
    }

    schar = (unsigned char *)sullong;

    //之后改为每次替换1个Byte的数据直到替换结束
    while (cnt < n)
    {
        *schar++ = u_ch;
        cnt++;
        //std::cout << "conditionC\n";
    }
}

void originmemset(void *s, int ch, size_t n)
{
    size_t cnt = 0;

    unsigned char u_ch = (unsigned char)ch;

    unsigned char *schar = static_cast<unsigned char *>(s);

    while (cnt < n)
    {
        *schar++ = u_ch;
        cnt++;
    }
}
