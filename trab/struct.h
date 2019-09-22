#ifndef STRUCT_H
#define STRUCT_H

#include <stdbool.h>

typedef struct SERVER{
    int numDeCanais;
    struct LISTC* primeiro;
    struct LISTC* ultimo;
} SERVER;

typedef struct LISTP{
	struct CLIENTE*  clt;
	struct LISTP*	prox;
    struct LISTP*   ant;
} LISTP;

typedef struct LISTC{
	struct CANAL*   chnl;
	struct LISTC*	prox;
    struct LISTC*   ant;
} LISTC;

typedef struct CANAL{
	struct CLIENTE*  admin;
	char 	name[15];
	struct LISTP*	primeiro;
	struct LISTP*	ultimo;
	int 	numParticipantes;
} CANAL;

typedef struct CLIENTE{
	char name[15];
	CANAL* channel;
} CLIENTE;

void criarCanal(char name[15], CLIENTE *c, SERVER *sv);
int removeCanal(char name[15], CLIENTE *c, SERVER *sv); //TODO AVISAR TDS USUARIOS
CANAL* retornaCanal(char name[15], SERVER *sv);
void adicionaParticipante(CLIENTE *c,CANAL *channel);
void sairDoCanal(CLIENTE *c);

#endif