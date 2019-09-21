#ifndef STRUCT_H
#define STRUCT_H

#include <stdbool.h>

struct LISTP Listp;
struct SERVER Servidor;
struct LISTC Listc;
struct CHANNELS Canal;
struct CLIENTS Client;

struct SERVER{
    int numDeCanais;
    struct Listc *primeiro;
    struct Listc *ultimo;
}Servidor;

struct LISTP{
	struct Client  *clt;
	struct Listp	*prox;
    struct Listp   *ant;
}Listp;

struct LISTC{
	struct Canal   *chnl;
	struct Listc	*prox;
    struct Listc   *ant;
}Listc;

struct CHANNELS{
	struct Client  *admin;
	char 	name[15];
	struct Listp	*participantes;
	int 	numParticipantes;
}Canal;

struct CLIENTS{
	char name[15];
	char channel[15];
}Client;

void criarCanal(char name[15], Client *c, Servidor *sv);
bool removeCanal(char name[15], Client *c, Servidor *sv); //TODO AVISAR TDS USUARIOS


#endif