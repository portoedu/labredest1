#include "struct.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

void insereNoFim(CANAL *novo, SERVER *serverChannels);
void criarCanal(char name[15], CLIENTE *c, SERVER *sv)
{
    CANAL* nova = NULL;

    nova = malloc(sizeof(CANAL));
    if (nova == NULL)
    {
        printf("ERRO ALOCAÇÃO!\n");
        return;
    }

    strcpy(nova->name, name);
    nova->admin = c;
    nova->participantes = NULL;
    nova->numParticipantes = 0;
    insereNoFim(nova, sv);
}

void insereNoFim(CANAL *novo, SERVER *serverChannels)
{
    LISTC* c = NULL;

    c = malloc(sizeof(LISTC));
    if (c == NULL)
    {
        printf("ERRO ALOCAÇÃO!\n");
        return;
    }

    c->prox;
    c->chnl = novo;

    c->ant = serverChannels->ultimo;
    if (serverChannels->ultimo != NULL)
    {
        serverChannels->ultimo->prox = c;
    }


    serverChannels->ultimo = c;
    serverChannels->numDeCanais += 1;
    if (serverChannels->primeiro == NULL)
    {
        serverChannels->primeiro = c;
    }


    return;
}

/*bool removeCanal(char name[15], CLIENTE *c, SERVER *sv) //todo ver
{
    LISTC *aux = sv->primeiro;
    if(aux->chnl->name == name)
    {
        if(aux->prox != NULL)
        {
            aux->prox->ant= NULL;
            sv->primeiro = aux->prox;
        }
        else
        {
            sv->primeiro = NULL;
        }
        free(aux);
    }

    LISTC *aux2;
    while(aux != NULL)
    {
        if(strncmp(aux->chnl->name, name, sizeof(name))==0)
        {
            if(aux->chnl->admin == c)
            {
                if(aux->prox != NULL)
                {
                    aux2 = aux->prox;
                    aux2->ant = aux->ant;
                    aux2 = aux->ant;
                    aux2->prox = aux->prox;
                }
                else
                {
                    aux->ant = NULL;
                }
                free(aux);
                return true;
            }
            else
            {
                return false;
            }
        }
        aux = aux->prox;
    }
    return false;
}

*/