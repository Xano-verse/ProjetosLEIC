#ifndef _CARROS_H_
#define _CARROS_H_

#include "constantes.h"


/**
 * Estrutura para representar um carro.
 */
typedef struct carro_no {
	char* nome_parque;
 	char matricula[COMP_MATRICULA+1];
	char* data_hora_entrada, *data_hora_saida;
	struct carro_no* prox;
 } Carro;


/**
 * Estrutura para guardar um ponteiro para um Carro, mas poder encadear estes
 * ponteiros numa lista ligada.
 */
typedef struct carro_ptr {
	Carro* carro;
	struct carro_ptr* prox;
} Carro_ptr;


typedef struct parque_no Parque;

Carro* cria_carro(Parque* parque, char matricula[], char* data);

Carro* procura_carro_dentro_parque(char matricula[], Carro_ptr** hashtable);

void remove_carros_parque(Parque* parque, Carro_ptr** hashtable);

Carro_ptr* cria_lista_instancias(char matricula[], Carro_ptr** hashtable);

void liberta_lista_instancias(Carro_ptr* inicio_lista);

//Funcao do teste pratico
float faturacao_toda_dum_carro(char matricula[], Carro_ptr** hashtable);


#endif
