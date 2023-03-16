/*
    g++ prog2.c -p -o prog2 -O0 -I/usr/local/lib -lpapi && ./prog2
*/

#include <stdio.h>
#include <stdlib.h>
#include "papi.h"

#define ROWS 4096 // do not change
#define COLS 512  // do not change

typedef struct
{
    unsigned long long toAccess;  // use this. do not change
    unsigned long long ignore[7]; // ignore this. do not change
} elementOf2DArray;

int  part1A_1B(elementOf2DArray (&myArray)[ROWS][COLS])
{


    unsigned long long sum = 0;
    int retval = 0, EventSet = PAPI_NULL;
    unsigned long long count = 0;
    long long values_row_major[2];
    long long values_col_major[2];


    const int eventlist[] = {PAPI_L2_DCA, PAPI_L2_DCM}; //L2 Data Cache Access and L2 Data Cache Miss 
    retval = PAPI_library_init(PAPI_VER_CURRENT);
     if (retval != PAPI_VER_CURRENT)
    {
        fprintf(stderr, "Error initializing PAPI! %s\n", PAPI_strerror(retval));
        return 0;
    }
    if ((retval = PAPI_create_eventset(&EventSet)) != PAPI_OK)
    {
        fprintf(stderr, "Error creating EventSet! %s\n", PAPI_strerror(retval));
    }
    if ((retval = PAPI_add_event(EventSet, eventlist[0])) != PAPI_OK)
    {    
        printf("Error importing PAPI_L2_DCA\n");
    }
    if ((retval = PAPI_add_event(EventSet, eventlist[1])) != PAPI_OK)
    {    
        printf("Error importing PAPI_L2_DCM\n");
    }
    if ((retval = PAPI_start(EventSet)) != PAPI_OK)
    {    
        printf("PAPI Start Error\n");
    }
    //Part 1A:: ROW major order
    for(int i = 0; i< ROWS; i++)
    {
        for(int j = 0; j<COLS; j++ )
        {
            //Papi count access

            sum = sum + myArray[i][j].toAccess;

        }
    }
     if ((retval = PAPI_stop(EventSet, values_row_major)) != PAPI_OK)
    {
        printf("PAPI Stop Error\n");
    }
    printf("\n\nPart 1A and 1B\n\n");
    printf("For Row Major Order::\t");
    printf("L2 DCA: %llu L2 DCM: %llu\n", values_row_major[0], values_row_major[1]);


     if ((retval = PAPI_start(EventSet)) != PAPI_OK)
    {    
        printf("PAPI Start Error\n");
    }
    //Part 1B:: Coloum major order
    for(int j = 0; j< COLS; j++)
    {
        for(int i = 0; i<ROWS; i++ )
        {
            sum = sum + myArray[i][j].toAccess;

        }
    }
     if ((retval = PAPI_stop(EventSet, values_col_major)) != PAPI_OK)
    {
        printf("PAPI Stop Error\n");
    }

    printf("For Col Major Order::\t");
    printf("L2 DCA: %llu L2 DCM: %llu\n", values_col_major[0], values_col_major[1]);

    
    
    printf("Answer:: L2 Data cache access count in Coloum major order is more than row major order because, In C array elements are places in contiguous memory locationS\
    In Row major order, each array element is available in next memory location, thus can be accomdate in L1 cache hence CPU need to access L2 cache to obtain Data\
    However, In coloum major order, each array element is availble in memory location 1D array length far away, hence it cannot be accomadate in L1 cache, Thus\
    CPU needs to access L2 cache often to get the data.\n");
    PAPI_cleanup_eventset(EventSet);
    PAPI_destroy_eventset(&EventSet);
    PAPI_shutdown();
}

int part2(elementOf2DArray (&myArray)[ROWS][COLS])
{
    /* Start your code for Part 2 here */
    // Print your conceptual answer to Part 2

    printf("\n\nPart 2\n\n");

    unsigned long long sum = 0;
    int retval = 0, EventSet = PAPI_NULL;
    unsigned long long count = 0;
    long long values[2];
    unsigned int elements = 0;
    
    FILE *dca_fp = fopen("part2.csv", "w+"); 

    const int eventlist[] = {PAPI_L2_DCA, PAPI_L2_DCM}; //L2 Data Cache Access and L2 Data Cache Miss 
    retval = PAPI_library_init(PAPI_VER_CURRENT);
     if (retval != PAPI_VER_CURRENT)
    {
        fprintf(stderr, "Error initializing PAPI! %s\n", PAPI_strerror(retval));
        return 0;
    }
    if ((retval = PAPI_create_eventset(&EventSet)) != PAPI_OK)
    {
        fprintf(stderr, "Error creating EventSet! %s\n", PAPI_strerror(retval));
    }
    if ((retval = PAPI_add_event(EventSet, eventlist[0])) != PAPI_OK)
    {    
        printf("Error importing PAPI_L2_DCA\n");
    }
    if ((retval = PAPI_add_event(EventSet, eventlist[1])) != PAPI_OK)
    {    
        printf("Error importing PAPI_L2_DCM\n");
    }
    //Code here
    for(int ittr_row = 4; ittr_row < (ROWS+1); ittr_row+=8)
    {
            if ((retval = PAPI_start(EventSet)) != PAPI_OK)
            {    
                printf("PAPI Start Error\n");
            }

        for(int i = 0; i< ittr_row;i++ )
        {

            for(int j= 0; j < 1; j++)
            {
                sum = sum + myArray[i][j].toAccess;

            }
        }

            if ((retval = PAPI_stop(EventSet, values)) != PAPI_OK)
            {
                printf("PAPI Stop Error\n");
            }
            printf("Elements (pages): %d L2 DCA: %llu\n", ittr_row, values[0]);
            fprintf(dca_fp,"%d,%llu\n", ittr_row, values[0]);
    }
    printf("\nAnswer: The elements were accessed in column major order. Since each row is 64B * 512 = 32KB in size, accessing in column major order ensures that each access is a different page in memory, and thus a different TLB entry.\nThe L2 Data Cache Accesses dramatically increase around 1300-1400 page accesses. This implies that there are 1,284 TLB entries combined between the L1 and L2 TLBs, since 1,284 is the nearest convienient addition of two powers of 2 (1028 and 256) to 1300.\n");
    PAPI_cleanup_eventset(EventSet);
    PAPI_destroy_eventset(&EventSet);
    PAPI_shutdown();
}

int part3(elementOf2DArray (&myArray)[ROWS][COLS])
{

    /*
    https://stackoverflow.com/questions/35555898/total-cache-misses-fewer-than-data-cache-misses-papi-l1-dcm-papi-l1-tcm
    I am seeing L2D Cache miss more than L2D Cache Access 
    */

    printf("\n\nPart 3\n\n");

    unsigned long long sum = 0;
    int retval = 0, EventSet = PAPI_NULL;
    unsigned long long count = 0;
    long long values[2];
    unsigned int elements = 0;
    float cache_miss_rate  = 0.0;
    float threshold = 20.0;
    bool  belowThreshFlag = false;
    int num_trigger_elements = 0;
    FILE *dca_fp = fopen("part3.csv", "w+"); 

    const int eventlist[] = {PAPI_L2_DCA, PAPI_L2_DCM}; //L2 Data Cache Access and L2 Data Cache Miss 
    retval = PAPI_library_init(PAPI_VER_CURRENT);
     if (retval != PAPI_VER_CURRENT)
    {
        fprintf(stderr, "Error initializing PAPI! %s\n", PAPI_strerror(retval));
        return 0;
    }
    if ((retval = PAPI_create_eventset(&EventSet)) != PAPI_OK)
    {
        fprintf(stderr, "Error creating EventSet! %s\n", PAPI_strerror(retval));
    }
    if ((retval = PAPI_add_event(EventSet, eventlist[0])) != PAPI_OK)
    {    
        printf("Error importing PAPI_L2_DCA\n");
    }
    if ((retval = PAPI_add_event(EventSet, eventlist[1])) != PAPI_OK)
    {    
        printf("Error importing PAPI_L2_DCM\n");
    }
    //Code here
    for(int ittr_row = 1; ittr_row < ROWS+1; ittr_row++)
    {
        if ((retval = PAPI_start(EventSet)) != PAPI_OK)
        {    
            printf("PAPI Start Error\n");
        }
        for(int repetitions = 0; repetitions < 100; repetitions++) {
            for(int i = 0; i < ittr_row;i++ )
            {
                for(int j= 0; j < COLS; j++)
                {
                    sum = sum + myArray[i][j].toAccess;

                }
            }
        }
        
        if ((retval = PAPI_stop(EventSet, values)) != PAPI_OK)
        {
            printf("PAPI Stop Error\n");
        }

         cache_miss_rate = (float)values[1]/(float)values[0];
        

        printf("Elements (pages): %d L2 DCM: %.2f\n", ittr_row*COLS, cache_miss_rate*100);
        //fprintf(dca_fp,"%d,%llu\n", ittr_row*COLS, values[1]);
        // fprintf(dca_fp,"%d,%llu\n", ittr_row*COLS, values[1]);
        fprintf(dca_fp,"%d,%.2f\n", ittr_row*COLS, cache_miss_rate*100);
        if(cache_miss_rate*100 < threshold)
        {
            belowThreshFlag = true;
        }
        else if (belowThreshFlag)
        {
            num_trigger_elements = ittr_row*COLS;
            break;
        }
        cache_miss_rate = 0.0;

    }
    PAPI_cleanup_eventset(EventSet);
    PAPI_destroy_eventset(&EventSet);
    PAPI_shutdown();

    printf("\nAnswer:: The Cache Miss Rate Threshold of %2.f\% was exceeded at %d elements.\nTherefore, the size of the L2 cache is approximately 64B * %d = %dKB.\n", threshold, num_trigger_elements, num_trigger_elements, (num_trigger_elements*64)/1000);


}

int main()
{ // Do not change anything in main
    // Allocating and initializing a 2D array.
    elementOf2DArray array[ROWS][COLS];
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            array[i][j].toAccess = i * j;
        }
    }
    // Calling functions for Part 1A, 1B, 2, and 3.
    printf("\n\nProg2 by Aditya Rajendra Gawali and Maxwell Stelmack");
    part1A_1B(array);
    part2(array);
    part3(array);
    return 0;
}