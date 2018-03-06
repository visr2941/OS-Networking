#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>

main()
{
struct rlimit limit;
int size = getrlimit(RLIMIT_STACK, &limit);

printf("hello world %llu %llu\n", limit.rlim_cur, limit.rlim_max);

}
