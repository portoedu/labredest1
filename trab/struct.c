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
    nova->primeiro = NULL;
    nova->ultimo = NULL;
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

    c->prox = NULL;
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

CANAL* retornaCanal(char name[15], SERVER *sv)
{
    if(sv->numDeCanais == 0)
    {
        return NULL;
    }
    
    LISTC *list = sv->primeiro;
    while(list != NULL)
    {
        if(strncmp(list->chnl->name,name,sizeof(name))==0)
        {
            return list->chnl;
        }
        list = list->prox;
    }
    return NULL;
}

int removeCanal(char name[15], CLIENTE *c, SERVER *sv) //todo ver
{
    LISTC *aux = sv->primeiro;
    if(aux == NULL)
    {
        return 0;
    }
    if(strncmp(aux->chnl->name,name,sizeof(name))==0)
    {
        if(aux->chnl->admin == c)
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
            sv->numDeCanais -=1;
            if(sv->numDeCanais == 0)
            {
                sv->ultimo = NULL;
            }
            free(aux);
            return 1;
        }
        return -1;
    }

    LISTC *aux2;
    aux = aux->prox;
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
                    aux->ant->prox = aux->prox;
                }
                else
                {
                    aux->ant->prox = NULL;
                }
                sv->numDeCanais -=1;
                if(sv->numDeCanais == 0)
                {
                    sv->ultimo = NULL;
                }
                free(aux);
                return 1;
            }
            else
            {
                return -1;
            }
        }
        aux = aux->prox;
    }
    return 0;
}

void adicionaParticipante(CLIENTE *c,CANAL *channel)
{
    LISTP* p = NULL;

    p = malloc(sizeof(LISTC));
    if (p == NULL)
    {
        printf("ERRO ALOCAÇÃO!\n");
        return;
    }

    p->prox = NULL;
    p->clt = c;
    if(channel->numParticipantes == 0)
    {
        channel->ultimo = p;
        channel->primeiro = p;
        channel->numParticipantes +=1;
        return;
    }

    p->ant = channel->ultimo;
    channel->ultimo->prox = p;
    channel->ultimo = p;
    channel->numParticipantes +=1;
    return;
}

void sairDoCanal(CLIENTE *c)
{
    LISTP *aux = c->channel->primeiro;

    if(strncmp(aux->clt->name,c->name,sizeof(c->name))==0)
    {
        if(aux->prox != NULL)
        {
            aux->prox->ant= NULL;
            c->channel->primeiro = aux->prox;
        }
        else
        {
            c->channel->primeiro = NULL;
        }
        c->channel->numParticipantes -=1;
        if(c->channel->numParticipantes == 0)
        {
            c->channel->ultimo = NULL;
        }
    }


    LISTP *aux2;
    aux = aux->prox;
    while(aux != NULL)
    {
        if(strncmp(aux->clt->name, c->name, sizeof(c->name))==0)
        {
            if(aux->prox != NULL)
            {
                aux2 = aux->prox;
                aux2->ant = aux->ant;
                aux->ant->prox = aux->prox;
            }
            else
            {
                aux->ant->prox = NULL;
            }
            c->channel->numParticipantes -=1;
            if(c->channel->numParticipantes == 0)
            {
                c->channel->ultimo = NULL;
            }
        }
        aux = aux->prox;
    }
    c->channel = NULL;
}