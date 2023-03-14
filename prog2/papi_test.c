#include <stdio.h>
#include <stdlib.h>
#include "papi.h"



int monitor_L2_access_start()
{
 
}

void monitor_L2_access_stop()
{

   
}
int main()
{
    
   
    int retval = 0, EventSet = PAPI_NULL;
    unsigned long long count = 0, sum = 0;
    const int eventlist[] = {PAPI_L2_DCA, PAPI_L2_DCM};
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
        printf("PAPI Stop Error\n");
    }
    // Code to be instrumented
    printf("Hello, World!\n");
    if ((retval = PAPI_stop(EventSet, values)) != PAPI_OK)
    {
        printf("PAPI Stop Error\n");
    }
    printf("L2 DCA: %llu L2 DCM: %llu\n", values[0], values[1]);
    return 0;
}
