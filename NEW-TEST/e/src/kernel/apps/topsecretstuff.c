#include <k/syscalls.h>
#include <stdio.h>

int secretHelp() {
    printf("\n Available secret and developer testing commands");
    printf("\n ----------------------------------------------- \n\n");
    printf(" -crash!  :Calls system crash disaster\n\n");
    return 0;
}

int whyAreWeStillHere(char *justToSuffer) {
    fatal(justToSuffer);
    return -1;
}
