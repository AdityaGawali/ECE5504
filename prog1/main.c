/*
    gcc -O0 -lm  main.c -o prog1
    ./prog1 
*/
 

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>


#define NO_OF_DATA 10000

static inline uint64_t rdtsc() 
{
    unsigned int eax, edx;
    __asm__ __volatile__("rdtsc":"=a"(eax),"=d"(edx));
    return ((uint64_t)edx<<32) | eax;
}
int main()
{

    uint32_t i;
    
    uint64_t min = UINT64_MAX ;
    uint64_t max = 0 ; 
    double sum = 0.0;
    double average = 0.0;

    uint64_t start = 0;
    uint64_t end = 0;
    uint64_t elapsed = 0;

    uint8_t* buffer;
    int count = 128*1024;

    uint64_t data[NO_OF_DATA];

    for (i = 0 ; i < NO_OF_DATA; i++)
    {
        start = rdtsc();
        buffer = (uint8_t*) malloc(count);        
        *buffer = i;
        end = rdtsc();
        free(buffer);
        count = count + 128*1024;
        elapsed = end -start;
        data[i] = elapsed;
        //printf("%lu\n", elapsed);

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

    uint64_t ts0, ts1;    
    ts0 = rdtsc();
    sleep(1);
    ts1 = rdtsc();    
    ts1 = ((ts1 -ts0) / 10e5);
    printf("clock frequency = %llu Mhz\n", ts1 );


   printf("Min: %f us\n", (double)min/ts1);
   printf("Max: %f us\n", (double)max/ts1);
   //printf("Sum: %f us\n", sum/3900);
   printf("Avg: %f us\n", average/ts1);
   printf("SD: %f us\n", std_dev/ts1);


    // printf("Min: %lu cycles\n", min);
    // printf("Max: %lu cycles\n", max);
    // printf("Sum: %lu cycles\n", sum);
    // printf("Avg: %f cycles\n", average);
    // printf("SD: %f cycles\n", std_dev);


    return 0;
}