#ifndef STRUCT_H
#define STRUCT_Hz

#include <stdbool.h>

typedef struct {
    int numDeCanais;
    struct Listc *primeiro;
    struct Listc *ultimo;
}Servidor;

typedef struct {
	struct Client  *clt;
	struct Listp	*prox;
    struct Listp   *ant;
}Listp;

typedef struct {
	struct Canal   *chnl;
	struct Listc	*prox;
    struct Listc   *ant;
}Listc;

typedef struct {
	struct Client  *admin;
	char 	name[15];
	struct Listp	*participantes;
	int 	numParticipantes;
}Canal;

typedef struct{
	char name[15];
	char channel[15];
}Client;

void criarCanal(char name[15], Client *c, Servidor *sv);
bool removeCanal(char name[15], Client *c, Servidor *sv); //TODO AVISAR TDS USUARIOS


#endif