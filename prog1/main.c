/*
    g++ -O0 -lm  main.c -o prog1
    ./prog1 
*/
 
#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>


#define NO_OF_DATA 10000
#define LEN 1000

static inline uint64_t rdtsc() 
{
    unsigned int eax, edx;
    __asm__ __volatile__("rdtsc":"=a"(eax),"=d"(edx));
    return ((uint64_t)edx<<32) | eax;
}

static inline uint64_t getPID() 
{
    char line[LEN];
    FILE *pidCmd = popen("echo $$", "r");

    fgets(line, LEN, pidCmd);
    uint64_t pid = strtoul(line, NULL, 10);

    pclose(pidCmd);
    return pid;
}

static inline double getMhz(uint64_t pid)
{
    uint64_t core = sched_getcpu();

    char line[LEN];
    char cmdText[100];
    sprintf(cmdText, "cat /sys/devices/system/cpu/cpu%lu/cpufreq/scaling_cur_freq", core);
    FILE *hzCmd = popen(cmdText, "r");

    fgets(line, LEN, hzCmd);
    double Mhz = strtoul(line, NULL, 10) / 1000.0;

    pclose(hzCmd);
    return Mhz;
}

int main()
{
    uint64_t pid = getPID();

    uint32_t i;
    
    double min = SIZE_MAX ;
    double max = 0.0; 
    double sum = 0.0;
    double average = 0.0;

    double Mhz = 0.0;
    uint64_t start = 0;
    uint64_t end = 0;
    double elapsed = 0;

    uint8_t* buffer;
    int count = 128*1024;

    double data[NO_OF_DATA];

    for (i = 0 ; i < NO_OF_DATA; i++)
    {
        Mhz = getMhz(pid);
        start = rdtsc(); 
        buffer = (uint8_t*) malloc(count);       
        *buffer = i;
        end = rdtsc();
        Mhz = (Mhz + getMhz(pid)) / 2;
        free(buffer);
        count = count + 128*1024;
        elapsed = (end - start) / Mhz;
        data[i] = elapsed;

        if(elapsed < min)
        {
            min = elapsed;
        } 
         if (elapsed > max )
        {
            max = elapsed;
        }
        sum = sum + elapsed;
    }

    average = sum/NO_OF_DATA ;
    double std_dev = 0.0;
    double temp = 0.0;
    for(int j=0; j< NO_OF_DATA; j++)
    {
         temp = data[j] - average;
        std_dev = std_dev + (temp*temp);
    }
    std_dev = std_dev/NO_OF_DATA;
    std_dev = sqrt(std_dev);

    printf("Min: %f us\n", min);
    printf("Max: %f us\n", max);
    printf("Avg: %f us\n", average);
    printf("SD: %f us\n", std_dev);

    return 0;
}