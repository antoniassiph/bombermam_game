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
#define TEMPO_PARA_EXPLODIR 20 //em loops da função jogar
#define TEMPO_PARA_LIMPAR_BOMBA 20 //em loops da função jogar
#define REDUTOR_DE_VELOCIDADE 35000 //em nanosegundos
#define TOPO 1 //MARGENS DO MAPA  - TEM Q SER IMPAR
#define RODAPE 13 //TEM Q SER IMPAR
#define ESQUERDA 15 //TEM Q SER IMPAR
#define DIREITA 1 //TEM Q SER IMPAR
#define INICIO_INTERFACE_LINHA 25
#define NUMERO_DE_VIDAS 3


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
		system("clear");
		for(i = 0; i<30; i++){
			for(j = 0; j < 50; j++){
				gotoxy(i,j);
				printf(" ");
			}
		}
	}
	else if (modo==0){
		for(i = 0; i<RODAPE+1; i++){
			gotoxy(i,ESQUERDA+1);
			printf("%c[%dm                                        ", 0x1B, 40); //Sim, isso é gammbiarra
		}
		for(i = 0; i<ESQUERDA+2; i++){
			gotoxy(RODAPE+1,i);
			printf("%c[%dm                                        ", 0x1B, 40); //Sim, isso é gammbiarra
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
				mapa[i][j] = '=';
			}
			if(j == DIREITA || j == ESQUERDA){
				mapa[i][j] = '|';
			}
			if(i == RODAPE && j == ESQUERDA){
				mapa[i][j] = ']';
			}
			if(i == RODAPE && j == DIREITA){
				mapa[i][j] = '[';
			}
			if(i == TOPO && j == DIREITA){
				mapa[i][j] = '<';
			}
			if(i == TOPO && j == ESQUERDA){
				mapa[i][j] = '>';
			}
        }
    }
}


void mostra_mapa(int *pos_per, int *saida){ //OK
	//mostra o que está na matriz
	int i, j;
    for(i = TOPO; i<RODAPE+1; i++){
        for(j = DIREITA; j < ESQUERDA+1; j++){
            gotoxy(i,j);
            if(i == pos_per[0] && j == pos_per[1])
				printf("%c[%d;%d;%dmH", 0x1B, 1, 34, 40); //font azul
			else if(mapa[i][j] == 'P') //Parades fixas
				printf("%c[%d;%d;%dm\u2588", 0x1B, 1, 37, 40); //font branco
			else if(mapa[i][j] == 'Q') //paredes Quebraveis
				printf("%c[%d;%d;%dm\u2592", 0x1B, 1, 37, 40); //font branco
			else if(i == saida[0] && j == saida[1]) //saida
				printf("%c[%d;%d;%dm#", 0x1B, 1, 35, 40); //font magenta
			else if(mapa[i][j] == '<') //margens
				printf("%c[%d;%d;%dm\u2554", 0x1B, 1, 37, 40); //font branco
			else if(mapa[i][j] == '>') //margens
				printf("%c[%d;%d;%dm\u2557", 0x1B, 1, 37, 40); //font branco
			else if(mapa[i][j] == '[') //margens
				printf("%c[%d;%d;%dm\u255a", 0x1B, 1, 37, 40); //font branco
			else if(mapa[i][j] == ']') //margens
				printf("%c[%d;%d;%dm\u255d", 0x1B, 1, 37, 40); //font branco
			else if(mapa[i][j] == '=') //margens
				printf("%c[%d;%d;%dm\u2550", 0x1B, 1, 37, 40); //font branco
			else if(mapa[i][j] == '|') //margens
				printf("%c[%d;%d;%dm\u2551", 0x1B, 1, 37, 40); //font branco
			else if(mapa[i][j] == 'b') //bomba
				printf("%c[%d;%d;%dm\u2508", 0x1B, 1, 33, 43); //tudo amarelo
			else if(mapa[i][j] == 'B') //bomba
				printf("%c[%d;%d;%dm\u250A", 0x1B, 1, 33, 43); //tudo amarelo
			else if(mapa[i][j] == '@') //bomba
				printf("%c[%d;%d;%dm@", 0x1B, 1, 33, 43); //tudo amarelo
			else
				printf("%c[%d;%d;%dm%c", 0x1B, 1, 37, 40, mapa[i][j]); //font branco
		}
    }
}


void monta_mapa_paredes(int *paredes, int *fase, int *saida){ //OK
	int i;
	//int random_bool = rand() % 2;
    for(i = 0; i< paredes[*fase]; i++){
		int linha, coluna;
		bool porta_de_saida_foi_colocada = false;
		do{
			linha = (rand() % RODAPE-2)+2;
			coluna = (rand() % ESQUERDA-2)+2;
		}while(mapa[linha][coluna] != ' ');
		mapa[linha][coluna] = 'Q';
		if(!porta_de_saida_foi_colocada){
			saida[0] = linha;
			saida[1] = coluna;
			porta_de_saida_foi_colocada = true;
		}
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

void bombas_ctrl(int nb, int *pos_personagem, int *bombas_no_mapa, int *nvidas, int *loops){ //OK
	int i = 0;
	float timer;
	for(i = 0; i < nb; i++){
		//printf("\n\n\n\n\n%d", bombas[i].active); //DEV
		if(bombas[i].active == 1){
			timer = (*loops-bombas[i].timer);
			//printf("\n\n%f", timer); //DEV
			if(timer > TEMPO_PARA_EXPLODIR){
				mapa[bombas[i].pos[0]][bombas[i].pos[1]] = '@';
				if(mapa[bombas[i].pos[0]+1][bombas[i].pos[1]] != 'P' && bombas[i].pos[0]+1 != RODAPE)
					mapa[bombas[i].pos[0]+1][bombas[i].pos[1]] = 'B';
				if(mapa[bombas[i].pos[0]-1][bombas[i].pos[1]] != 'P' && bombas[i].pos[0]-1 != TOPO)
					mapa[bombas[i].pos[0]-1][bombas[i].pos[1]] = 'B';
				if(mapa[bombas[i].pos[0]][bombas[i].pos[1]+1] != 'P' && bombas[i].pos[1]+1 != ESQUERDA)
					mapa[bombas[i].pos[0]][bombas[i].pos[1]+1] = 'b';
				if(mapa[bombas[i].pos[0]][bombas[i].pos[1]-1] != 'P' && bombas[i].pos[1]-1 != DIREITA)
					mapa[bombas[i].pos[0]][bombas[i].pos[1]-1] = 'b';
				*bombas_no_mapa -= 1;
				bombas[i].active = 2;
				bombas[i].timer = *loops; //reinicia o cronometro
				//puts("tu é lindo cara"); //DEV
				//explode bomba
				if(	(bombas[i].pos[0] == pos_personagem[0] && bombas[i].pos[1] == pos_personagem[1]) || //Se está na bomba
					(bombas[i].pos[0]+1 == pos_personagem[0] && bombas[i].pos[1] == pos_personagem[1]) || //a cima
					(bombas[i].pos[0]-1 == pos_personagem[0] && bombas[i].pos[1] == pos_personagem[1]) || // a baixo
					(bombas[i].pos[1]+1 == pos_personagem[1] && bombas[i].pos[0] == pos_personagem[0]) || //a esquerda
					(bombas[i].pos[1]-1 == pos_personagem[1] && bombas[i].pos[0] == pos_personagem[0])){  //a direita
						//mata herói
						//gotoxy(15, 50);
						//puts("MORREU FILHA DA PUTA");
						*nvidas -= 1;
					}
			}
		}
		if(bombas[i].active == 2){
			//limpa rastro da bomba
			timer = (*loops-bombas[i].timer);
			if(timer > TEMPO_PARA_LIMPAR_BOMBA){
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
 
int add_bomba(int nb, int *pos_personagem, int *b_no_mapa, int *loops){ //aparentemente ok
	int i;
	if(nb <= *b_no_mapa) //limite de bombas atingido
		return 0;
	for(i = 0; i < nb; i++) //procura por um espaço vazio no vetor
		if(bombas[i].active != 1 && bombas[i].active != 2){
			bombas[i].pos[0] = pos_personagem[0]; 
			bombas[i].pos[1] = pos_personagem[1];
			bombas[i].timer = *loops;
			bombas[i].active = 1;
			*b_no_mapa += 1;
			return 1;
		}
	return 0;
}

void mostra_ui(int fase, int limite_bombas, int vidas, int *loops){ //funciona - Quebra galho
	gotoxy(INICIO_INTERFACE_LINHA, 2);
	printf("Limite de Bombas: %d Fase: %d Vidas: %d Loops(Tempo): %d", limite_bombas, fase+1, vidas, *loops);
}

int jogar(int fase_atual){
	int nvidas = NUMERO_DE_VIDAS;
	int pos_personagem[2] = {TOPO+1, DIREITA+1}; //LINHA X COLUNA
	char tecla;
	int porta_saida[2]; //fim do jogo
	int bombas_no_mapa = 0;
	int num_bombas[NUMERO_DE_FASES] = {1, 3, 5}; //limite de bombas ao mesmo tempo por fase
	int paredes[NUMERO_DE_FASES] = {10, 15, 20}; //numero de paredes
	bool InGame = true;
	int loopCounter = 0;
	limpa_tela(1);
	monta_mapa(); //gera as paredes fisicas e limites laterais
	monta_mapa_paredes(paredes, &fase_atual, porta_saida); //monta paredes quebraveis
	mostra_mapa(pos_personagem, porta_saida); //mostra o mapa
	
	while(InGame){
		loopCounter++;
		if(kbhit()){
			//printf("\a\7"); //isso deveria fazer um beep na placa mãe mas não funciona :(
			tecla = getchar();
			if(tecla == 27){ //ler teclas especiais
				ler_tecla_especial(&tecla); //ajusta as teclas especiais
			}
			if(tecla == 'w' || tecla == 'a' || tecla == 's' || tecla == 'd'){ //move personagem
				move_personagem(pos_personagem, &fase_atual, paredes, &tecla);
			}
			if(tecla == 'b' || tecla == 32 || tecla == ' '){
				if(add_bomba(num_bombas[fase_atual], pos_personagem, &bombas_no_mapa, &loopCounter))
					mapa[pos_personagem[0]][pos_personagem[1]] = 'o'; //solta bomba no mapa
			}
			if(tecla == 'p' || tecla == 'P'){
				limpa_tela(1);
				gotoxy(1,1);
				puts("PAUSE");
				puts("Press any key to resume");
				tecla = getch();
				limpa_tela(1);
			}
			if(tecla == 'm' || tecla == 'M'){
				return 4;
			}
		}
		mostra_ui(fase_atual, num_bombas[fase_atual], nvidas, &loopCounter);
		mostra_mapa(pos_personagem, porta_saida);
		limpa_tela(0);
		if(nvidas <= 0)
			return 1;
		if(pos_personagem[0] == porta_saida[0] && pos_personagem[1] == porta_saida[1])
			return 2;
		bombas_ctrl(num_bombas[fase_atual], pos_personagem, &bombas_no_mapa, &nvidas, &loopCounter); //controla as bombas ativas
		usleep(REDUTOR_DE_VELOCIDADE); //"dorme" por x microsegundos. Evita de processar a tela várias vezes sem necessidade
	}
	return 0;
}

int main(){
	printf("\e[?25l"); //hide cursor
	srand( (unsigned)time(NULL) ); //gera seed rand() com base no time()
	bool HakunaMatataNaoColocaIssoComoVerdadeiro = 0; //isso mantém o loop principal do jogo
	int fim_de_jogo = 0;
	int fase_atual = 0;
	char c;
	memset(mapa, ' ', sizeof(mapa)-1); //limpa a matriz do mapa
	monta_mapa(); //gera as paredes fisicas e limites laterais
	limpa_tela(1); //limpa toda a tela
	while(!HakunaMatataNaoColocaIssoComoVerdadeiro){
		printf("%c[%dm", 0x1B, 40);
		gotoxy(1,1);
		puts("BOMBERMAN");
		if(fim_de_jogo == 1){
			puts("You died.");
			puts("Press 'S' to start the level again");
		} else if (fim_de_jogo == 2){ //proxima fase
			if(fase_atual < NUMERO_DE_FASES){
				fase_atual++;
				puts("NEXT LEVEL!");
				puts("Press 'S' to start next level");
			} else {
				puts("YOU HAVE COMPLETE THE GAME!");
				puts("Press 'R' to restart");
			}
		} else {
			if(fim_de_jogo == 4){
				//fase_atual = 0;
			}
			puts("Press 'S' to start");
		}
		puts("Press 'E' to exit");
		c = getch();
		if(c == 'e' || c == 'E'){
			exit(EXIT_SUCCESS);
		}
		if(c == 's' || c == 'S'){
			limpa_tela(1);
			fim_de_jogo = jogar(fase_atual);
			limpa_tela(1);
		}
		if(c == 'r' || c == 'R'){
			limpa_tela(1);
			fase_atual = 0;
			fim_de_jogo = 0;
			fim_de_jogo = jogar(fase_atual);
			limpa_tela(1);
		}
	}
	return 0;
}

