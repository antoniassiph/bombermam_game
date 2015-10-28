#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "headers/getch_getche.h" //funcao getche e getch do windows
#include <string.h>
#include "headers/kbhit.h" //funcao kbhit do windows

char mapa[11][13];

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
	printf("asddgsfdfbnklsfajjjjjjjcfhdrsghnadscghkjdcghjsjghsaghjfcj");//suja tela
	restart();
	return 0;
}

