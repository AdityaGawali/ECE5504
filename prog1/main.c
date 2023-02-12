/*
    gcc -O0 -lm  main.c -o prog1
    ./prog1 
*/
 

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <float.h>

#define NO_OF_DATA 10000


int get_CPU_frequency_in_MHZ()
{
  FILE *fp;
  char path[1035];
  int total_n = 0;
  int n;
  int i;
  int number = 0;
  int checked = 0;

  /* Open the command for reading. */
    fp = popen("cpupower frequency-info", "r");
    if (fp == NULL) 
    {
      printf("Failed to run command\n" );
      exit(1);
    }
    for (int i=0;i<11;i++)
    {
      fgets(path, sizeof(path), fp);
    }

    fgets(path, sizeof(path), fp);
    while (1 == sscanf(path + total_n, "%*[^0123456789]%d%n", &i, &n))
    {
      total_n += n;
      if(checked == 0)
      {
        checked=1;
        number = i*1000;
        //printf("%d\n", i);
      }
      else
      {
        checked = 0;
        //printf("%d\n", i);
        number = number + i*10;
    } 
  }
pclose(fp);
return number;
}


static inline uint64_t rdtsc() 
{
    unsigned int eax, edx;
    __asm__ __volatile__("rdtsc":"=a"(eax),"=d"(edx));
    return ((uint64_t)edx<<32) | eax;
}
int main()
{

    uint32_t i;
    
    double min = DBL_MAX ;
    double max = 0.0 ; 
    double sum = 0.0;
    double average = 0.0;

    uint64_t start = 0;
    uint64_t end = 0;
    uint64_t elapsed = 0;

    uint8_t* buffer;
    int count = 128*1024;

    double elapsed_time;
    double elapsed_time_buffer[NO_OF_DATA];

    for (i = 0 ; i < NO_OF_DATA; i++)
    {
        start = rdtsc();
        buffer = (uint8_t*) malloc(count);        
        *buffer = i;
        end = rdtsc();
        free(buffer);
        count = count + 128*1024;
        elapsed = end -start;
        
        int freq = get_CPU_frequency_in_MHZ();
        elapsed_time  = (double)elapsed/freq;
        elapsed_time_buffer[i] = elapsed_time;

        // printf("%d\n", freq);

        if(elapsed_time < min)
        {
            min = elapsed_time;
        } 
         if (elapsed_time > max )
        {
            max = elapsed_time;
        }
        sum = sum + elapsed_time;
    }

    average = sum/NO_OF_DATA ;
    
    double std_dev = 0.0;
    double temp = 0.0;
    for(int j=0; j< NO_OF_DATA; j++)
    {
        temp = elapsed_time_buffer[j] - average;
        std_dev = std_dev + (temp*temp);
    }
    std_dev = std_dev/NO_OF_DATA;
    std_dev = sqrt(std_dev);



   printf("Min: %f us\n",min);
   printf("Max: %f us\n",max);
   printf("Avg: %f us\n", average);
   printf("SD: %f us\n", std_dev);

    return 0;
}
