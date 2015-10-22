#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include "headers/getch_getche.h" //funcao getche e getch do windows
#include <string.h>
#include <termios.h> //usado no kbhit
#include <fcntl.h> //usado no kbhit

char mapa[11][13];

int kbhit(void) //equivalente ao KBHIT do windows
{
  struct termios oldt, newt;
  int ch;
  int oldf;
 
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
 
  ch = getchar();
 
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);
 
  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }
 
  return 0;
}

void gotoxy(int y, int x) //equivalente gotoxy do windows LINHA X COLUNA
{
  printf("\033[%d;%df", y, x);
  fflush(stdout);
}

void limpa_tela(){ //limpa tudo
	int i, j;
	for(i = 0; i<30; i++){
		for(j = 0; j < 50; j++){
			gotoxy(i,j);
			printf(" ");
		}
	}
}

void mostra_mapa(){ //mostra o que está na matriz
	int i, j;
        for(i = 0; i<11; i++){
                for(j = 0; j < 13; j++){
                        gotoxy(i,j);
                        printf("%c", mapa[i][j]);
                }
        }

}

void restart(){ //devolve tudo para a forma inicial
	memset(mapa, '#', sizeof(mapa)-1);
	limpa_tela();
	mostra_mapa();
}

void move_personagem(){
	
}

int main(){
	std::cout << "asddgsfdfbnklsfajjjjjjjcfhdrsghnadscghkjdcghjsjghsaghjfcj";//suja tela
	restart();
	return 0;
}

