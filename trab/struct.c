#include "struct.h"
#include <stdlib.h>

//extern Servidor serverChannels;

void criarCanal(char name[15], Client *c, Servidor *sv)
{
    Canal* nova = NULL;

    nova = malloc(sizeof(Canal));
    if (nova == NULL)
    {
        printf("ERRO ALOCAÇÃO!\n");
        return NULL;
    }

    nova->admin = c;
    nova->participantes = NULL;
    nova->numParticipantes = 0;
    insereNoFim(nova, sv);
}

void insereNoFim(Canal *novo, Servidor *serverChannels)
{
    Listc* c = NULL;

    c = malloc(sizeof(Listc));
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
    if (serverChannels->primeiro == NULL)
    {
        serverChannels->primeiro = c;
    }


    return;
}

bool removeCanal(char name[15], Client *c, Servidor *sv) //todo ver
{
    Listc *aux = sv->primeiro;
    if(aux->chnl->name == name)
    {
        if(aux->prox != NULL)
        {
            aux->prox->ant= NULL
            sv->aux = aux->prox;
        }
        else
        {
            sv->primeiro = NULL;
        }
        free(aux);
    }

    Listc *aux2;
    while(aux != NULL)
    {
        if(strncmp(aux->chnl->name, name)==0)
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

