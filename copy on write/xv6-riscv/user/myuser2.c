#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{

   printf("Hello World \n");
   sleep(atoi(argv[1]));
   printf("Hi\n");
   exit(0);
}
