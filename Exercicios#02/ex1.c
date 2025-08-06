#include <stdio.h>
#include <string.h>

#define TAM 100

int main (){
    
    char string[TAM + 1];

    scanf ("%[^\n]", string);

    printf ("%s\n", string[strlen (string)]);

    return 0;
}