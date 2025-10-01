#include <stdio.h>
#include <unistd.h>
#include <papi.h>
#include <papi_test.h>

extern int launch_kernel(int device_id);

int main(int argc, char *argv[])
{
    int papi_errno;
#define NUM_EVENTS (7)
    long long counters[NUM_EVENTS] = { 0 };

    const char *events[NUM_EVENTS] = {
        "rocp_sdk:::SQ_CYCLES:device=0:DIMENSION_INSTANCE=0:DIMENSION_SHADER_ENGINE=3",
        "rocp_sdk:::TCC_CYCLE:device=0:DIMENSION_INSTANCE=2",
        "rocp_sdk:::SQ_INSTS:device=0:DIMENSION_INSTANCE=0",
        "rocp_sdk:::SQ_BUSY_CYCLES:device=0:DIMENSION_INSTANCE=0:DIMENSION_SHADER_ENGINE=0",
        "rocp_sdk:::SQ_BUSY_CYCLES:device=0:DIMENSION_INSTANCE=0:DIMENSION_SHADER_ENGINE=1",
        "rocp_sdk:::SQ_BUSY_CYCLES:device=0:DIMENSION_INSTANCE=0:DIMENSION_SHADER_ENGINE=2",
        "rocp_sdk:::SQ_BUSY_CYCLES:device=0:DIMENSION_INSTANCE=0"
    };

    papi_errno = PAPI_library_init(PAPI_VER_CURRENT);
    if (papi_errno != PAPI_VER_CURRENT) {
        test_fail(__FILE__, __LINE__, "PAPI_library_init", papi_errno);
    }

    int eventset = PAPI_NULL;
    papi_errno = PAPI_create_eventset(&eventset);
    if (papi_errno != PAPI_OK) {
        test_fail(__FILE__, __LINE__, "PAPI_create_eventset", papi_errno);
    }

    for (int i = 0; i < NUM_EVENTS; ++i) {
        papi_errno = PAPI_add_named_event(eventset, events[i]);
        if (papi_errno != PAPI_OK) {
            test_fail(__FILE__, __LINE__, "PAPI_add_named_event", papi_errno);
        }
    }

    papi_errno = PAPI_start(eventset);
    if (papi_errno != PAPI_OK) {
        test_fail(__FILE__, __LINE__, "PAPI_start", papi_errno);
    }

    printf("---------------------  launch_kernel(0)\n");
    papi_errno = launch_kernel(0);
    if (papi_errno != 0) {
        test_fail(__FILE__, __LINE__, "launch_kernel(0)", papi_errno);
    }

    usleep(10000);

    papi_errno = PAPI_read(eventset, counters);
    if (papi_errno != PAPI_OK) {
        test_fail(__FILE__, __LINE__, "PAPI_read", papi_errno);
    }
    printf("---------------------  PAPI_read()\n");

    for (int i = 0; i < NUM_EVENTS; ++i) {
        printf("%s: %.2lfM\n", events[i], (double)counters[i]/1e6);
    }

    papi_errno = PAPI_stop(eventset, counters);
    if (papi_errno != PAPI_OK) {
        test_fail(__FILE__, __LINE__, "PAPI_stop", papi_errno);
    }

    printf("---------------------  PAPI_stop()\n");

    for (int i = 0; i < NUM_EVENTS; ++i) {
        printf("%s: %.2lfM\n", events[i], (double)counters[i]/1e6);
    }
    
    papi_errno = PAPI_cleanup_eventset(eventset);
    if (papi_errno != PAPI_OK) {
        test_fail(__FILE__, __LINE__, "PAPI_cleanup_eventset", papi_errno);
    }

    papi_errno = PAPI_destroy_eventset(&eventset);
    if (papi_errno != PAPI_OK) {
        test_fail(__FILE__, __LINE__, "PAPI_destroy_eventset", papi_errno);
    }

    PAPI_shutdown();
    test_pass(__FILE__);
    return 0;
}
