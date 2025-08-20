#include <stdio.h>
#include <string.h>


#include <stdio.h>
int main(){
    char *possibilidade1 = "PROG2";
    char possibilidade2 [] = "PROG2";
    char possibilidade3 [] = { 'P', 'R', 'O',
    'G', '2', '\0' };
    printf("%s -- %d\n", possibilidade1,
    possibilidade1);
    printf("%s -- %d\n", possibilidade2,
    possibilidade2);
    printf("%s -- %d\n", possibilidade3,
    possibilidade3);
    return 0;
}