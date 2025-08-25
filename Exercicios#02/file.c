#include <stdio.h>
#include <stdlib.h>
 
int main ()
{
  FILE *arq ;
  unsigned char c ;
 
  // abre o arquivo em escrita
  arq = fopen ("ascii.txt", "w+") ;
  if ( ! arq )
  {
     perror ("Erro ao abrir/criar arquivo") ;
     exit (1) ;
  }
 
  // escreve os caracteres ascii
  fputs ("caracteres ASCII:", arq) ;
  for (c = 32; c < 128; c++)
  {
    fputc (c, arq) ;
    fputc (' ', arq) ;
  }
  fputc ('\n', arq) ;
 
  // fecha o arquivo
  fclose (arq) ;

  return 0;
}