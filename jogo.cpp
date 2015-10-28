/*
 * printf("\e[?25l"); -> hide the cursor 
 * printf("\e[?25h"); -> show the cursor 
 */
#include <stdio.h> //usado para usleep/sleep
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "headers/getch_getche.h" //funcao getche e getch do windows
#include <string.h>
#include "headers/kbhit.h" //funcao kbhit do windows
#include <time.h>
#define NUMERO_DE_FASES 3
#define NUMERO_MAXIMO_DE_BOMBAS 20
#define TEMPO_PARA_EXPLODIR 15000 //em clocks da CPU
#define TOPO 1 //MARGENS DO MAPA  - TEM Q SER IMPAR
#define RODAPE 13 //TEM Q SER IMPAR
#define ESQUERDA 15 //TEM Q SER IMPAR
#define DIREITA 1 //TEM Q SER IMPAR
#define INICIO_INTERFACE_LINHA 25
#define VIDAS 3


char mapa[14][16]; //LINHA X COLUNA
struct bomb{
	int pos[2]; //posição das bombas
	clock_t timer; //time de lançamento da bomba
	int active; //ativa ou não
} bombas[NUMERO_MAXIMO_DE_BOMBAS];

void gotoxy(int y, int x){ //OK
	//equivalente gotoxy do windows LINHA X COLUNA
	//Não mexer em nada, não sei como funciona mas funciona
	printf("\033[%d;%df", y, x);
	fflush(stdout);
}

void limpa_tela(int modo){ //OK
	//limpa tudo = 1; limpa bordas = 0
	int j, i;
	if(modo == 1){
		for(i = 0; i<30; i++){
			for(j = 0; j < 50; j++){
				gotoxy(i,j);
				printf(" ");
			}
		}
	}
	else if (modo==0){
		for(i = 0; i<30; i++){
			gotoxy(i,ESQUERDA+1);
			printf(" ");
		}
		for(i = 0; i<30; i++){
			gotoxy(RODAPE+1,i);
			printf(" ");
		}
	}
}

void monta_mapa(){ //QUASE OK
	//monta os pontos fixos do mapa
	int i, j;
    for(i = TOPO; i<RODAPE+1; i++){
        for(j = DIREITA; j < ESQUERDA+1; j++){
			if(i%2 != 0 && j%2 != 0){
				mapa[i][j] = 'P';
			}
			if(i == TOPO || i == RODAPE){
				mapa[i][j] = '-';
			}
			if(j == DIREITA || j == ESQUERDA){
				mapa[i][j] = '|';
			}
        }
    }
}


void mostra_mapa(int *pos_per){ //OK
	//mostra o que está na matriz
	int i, j;
    for(i = TOPO; i<RODAPE+1; i++){
        for(j = DIREITA; j < ESQUERDA+1; j++){
            gotoxy(i,j);
            if(i == pos_per[0] && j == pos_per[1])
				printf("H");
			if(mapa[i][j] == 'P') //Parades fixas
				printf("\u2588"); 
			if(mapa[i][j] == 'Q') //paredes Quebraveis
				printf("\u2592");
			else
				printf("%c", mapa[i][j]);
        }
    }
}


void monta_mapa_paredes(int *paredes, int *fase){ //OK
	int i;
	//int random_bool = rand() % 2;
    for(i = 0; i< paredes[*fase]; i++){
		int linha, coluna;
		do{
			linha = (rand() % RODAPE-2)+2;
			coluna = (rand() % ESQUERDA-2)+2;
		}while(mapa[linha][coluna] != ' ');
		mapa[linha][coluna] = 'Q';
    }
}

void ler_tecla_especial(char *tecla){ //OK
	//ler teclas que não estão na tabela ASCII
	*tecla = getchar(); //segundo caracter
	*tecla = getchar(); //terceiro caracter (o q varia conforme a tecla)
	//gotoxy(20, 30);
	//printf("%d", *tecla);
	switch(*tecla){
		case 65: //seta para cima
			*tecla = 'w';
			break;
		case 66: //seta para baixo
			*tecla = 's';
			break;
		case 67: //seta para direita
			*tecla = 'd';
			break;
		case 68: //seta para esquerda
			*tecla = 'a';
			break;
	}
}

void bombas_ctrl(int nb, int *pos_personagem, int *bombas_no_mapa, int *vidas){ //OK
	int i = 0;
	float timer;
	for(i = 0; i < nb; i++){
		//printf("\n\n\n\n\n%d", bombas[i].active); //DEV
		if(bombas[i].active == 1){
			timer = (clock()-bombas[i].timer);
			//printf("\n\n%f", timer); //DEV
			if(timer > TEMPO_PARA_EXPLODIR){
				mapa[bombas[i].pos[0]][bombas[i].pos[1]] = 'X';
				if(mapa[bombas[i].pos[0]+1][bombas[i].pos[1]] != 'P' && bombas[i].pos[0]+1 != RODAPE)
					mapa[bombas[i].pos[0]+1][bombas[i].pos[1]] = '|';
				if(mapa[bombas[i].pos[0]-1][bombas[i].pos[1]] != 'P' && bombas[i].pos[0]-1 != TOPO)
					mapa[bombas[i].pos[0]-1][bombas[i].pos[1]] = '|';
				if(mapa[bombas[i].pos[0]][bombas[i].pos[1]+1] != 'P' && bombas[i].pos[1]+1 != ESQUERDA)
					mapa[bombas[i].pos[0]][bombas[i].pos[1]+1] = '-';
				if(mapa[bombas[i].pos[0]][bombas[i].pos[1]-1] != 'P' && bombas[i].pos[1]-1 != DIREITA)
					mapa[bombas[i].pos[0]][bombas[i].pos[1]-1] = '-';
				*bombas_no_mapa -= 1;
				bombas[i].active = 2;
				bombas[i].timer = clock(); //reinicia o cronometro
				//puts("tu é lindo cara"); //DEV
				//explode bomba
				if(	(bombas[i].pos[0] == pos_personagem[0] && bombas[i].pos[1] == pos_personagem[1]) || 
					(bombas[i].pos[0]+1 == pos_personagem[0] && bombas[i].pos[1] == pos_personagem[1]) || 
					(bombas[i].pos[0]-1 == pos_personagem[0] && bombas[i].pos[1] == pos_personagem[1]) ||
					(bombas[i].pos[1]+1 == pos_personagem[1] && bombas[i].pos[0] == pos_personagem[0]) || 
					(bombas[i].pos[1]-1 == pos_personagem[1] && bombas[i].pos[0] == pos_personagem[0])){
						
						gotoxy(15, 50);
						puts("MORREU FILHA DA PUTA");
						*vidas -= 1;
					}
			}
		}
		if(bombas[i].active == 2){
			//limpa rastro da bomba
			timer = (clock()-bombas[i].timer);
			if(timer > 10000){
				mapa[bombas[i].pos[0]][bombas[i].pos[1]] = ' ';
				if(mapa[bombas[i].pos[0]+1][bombas[i].pos[1]] != 'P' && bombas[i].pos[0]+1 != RODAPE)
					mapa[bombas[i].pos[0]+1][bombas[i].pos[1]] = ' ';
				if(mapa[bombas[i].pos[0]-1][bombas[i].pos[1]] != 'P' && bombas[i].pos[0]-1 != TOPO)
					mapa[bombas[i].pos[0]-1][bombas[i].pos[1]] = ' ';
				if(mapa[bombas[i].pos[0]][bombas[i].pos[1]+1] != 'P' && bombas[i].pos[1]+1 != ESQUERDA)
					mapa[bombas[i].pos[0]][bombas[i].pos[1]+1] = ' ';
				if(mapa[bombas[i].pos[0]][bombas[i].pos[1]-1] != 'P' && bombas[i].pos[1]-1 != DIREITA)
					mapa[bombas[i].pos[0]][bombas[i].pos[1]-1] = ' ';
				bombas[i].active = 0;
			}
		}
	}
}

void move_personagem(int *pos, int *fase, int *paredes, char *tecla){ //OK
	//gotoxy(30, 30);
	//printf("%d", tecla);
	switch(*tecla){
		case 'w':
			if(pos[0] != TOPO+1 && mapa[pos[0]-1][pos[1]] != 'P' && mapa[pos[0]-1][pos[1]] != 'Q'){
				pos[0] -= 1;
			}
			break;
		case 'a':
			if(pos[1] != DIREITA+1 && mapa[pos[0]][pos[1]-1] != 'P' && mapa[pos[0]][pos[1]-1] != 'Q'){
				pos[1] -= 1;
			}
			break;
		case 's':
			if(pos[0] != RODAPE-1 && mapa[pos[0]+1][pos[1]] != 'P' && mapa[pos[0]+1][pos[1]] != 'Q'){
				pos[0] += 1;
			}
			break;
		case 'd':
			if(pos[1] != ESQUERDA-1 && mapa[pos[0]][pos[1]+1] != 'P' && mapa[pos[0]][pos[1]+1] != 'Q'){
				pos[1] += 1;
			}
			break;
	}
}
 
int add_bomba(int nb, int *pos_personagem, int *b_no_mapa){ //aparentemente ok
	int i;
	if(nb <= *b_no_mapa) //limite de bombas atingido
		return 0;
	for(i = 0; i < nb; i++) //procura por um espaço vazio no vetor
		if(bombas[i].active != 1 && bombas[i].active != 2){
			bombas[i].pos[0] = pos_personagem[0]; 
			bombas[i].pos[1] = pos_personagem[1];
			bombas[i].timer = clock();
			bombas[i].active = 1;
			*b_no_mapa += 1;
			return 1;
		}
	return 0;
}

/*void mostra_ui(int fase, int limite_bombas, int vidas, clock_t *relogio){ //Não funciona
	gotoxy(INICIO_INTERFACE_LINHA, 2);
	printf("Limite de Bombas: %d Fase: %d Vidas: %d Clock: %ld", limite_bombas, fase, vidas, *relogio);
}*/

int main(){
	printf("\e[?25l"); //hide cursor
	srand( (unsigned)time(NULL) ); //gera seed rand() com base no time()
	int paredes[NUMERO_DE_FASES] = {10, 15, 20}; //numero de paredes
	int num_bombas[NUMERO_DE_FASES] = {1, 3, 5}; //limite de bombas ao mesmo tempo por fase
	int pos_personagem[2] = {TOPO+1, DIREITA+1}; //LINHA X COLUNA
	int fase_atual = 2;
	int bombas_no_mapa;
	char tecla;
	int vidas = 10;// = VIDAS;
	//clock_t game_start = clock();
	memset(mapa, ' ', sizeof(mapa)-1); //limpa a matriz do mapa
	monta_mapa(); //gera as paredes fisicas e limites laterais
	monta_mapa_paredes(paredes, &fase_atual); //monta paredes quebraveis
	limpa_tela(1); //limpa toda a tela
	mostra_mapa(pos_personagem); //mostra o mapa
	//printf("\a\7"); isso deveria fazer um beep na placa mãe mas não funciona :(
	while(1){ //OBS -> alterar condição de parada
		if(kbhit()){
			tecla = getchar();
			if(tecla == 27){ //ler teclas especiais
				ler_tecla_especial(&tecla); //ajusta as teclas especiais
			}
			if(tecla == 'w' || tecla == 'a' || tecla == 's' || tecla == 'd'){ //move personagem
				move_personagem(pos_personagem, &fase_atual, paredes, &tecla);
			}
			if(tecla == 'b' || tecla == 32 || tecla == ' '){
				if(add_bomba(num_bombas[fase_atual], pos_personagem, &bombas_no_mapa))
					mapa[pos_personagem[0]][pos_personagem[1]] = 'o'; //solta bomba no mapa
			}
		}
		//mostra_ui(fase_atual, num_bombas[fase_atual], vidas, &game_start);
		mostra_mapa(pos_personagem);
		limpa_tela(0);
		//usleep(100000);
		bombas_ctrl(num_bombas[fase_atual], pos_personagem, &bombas_no_mapa, &vidas); //controla as bombas ativas
		usleep(70000); //"dorme" por x microsegundos. Evita de processar a tela várias vezes sem necessidade
	}
	return 0;
}

