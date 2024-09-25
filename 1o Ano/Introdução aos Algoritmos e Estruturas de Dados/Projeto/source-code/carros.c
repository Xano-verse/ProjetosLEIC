//Modulo dedicado a manipulacao e organizacao da estrutura Carro

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "carros.h"
#include "saidas.h"
#include "parques.h"
#include "hashtable.h"




/**
 * Funcao publica que adiciona um novo Carro a lista ligada de Carros do
 * Parque fornecido e lhe atribui a matricula e data de entrada fornecidas
 */
Carro* cria_carro(Parque* parque, char matricula[], char* data) {
	Carro* carro_novo;
	carro_novo = (Carro*) malloc(sizeof(Carro));
	
	(*carro_novo).prox = (*parque).carros;
	(*parque).carros = carro_novo;
	
	(*carro_novo).nome_parque = (*parque).nome;
	strcpy((*carro_novo).matricula, matricula);
	
	(*carro_novo).data_hora_entrada = data;
	(*carro_novo).data_hora_saida = NULL;
	
	(*parque).num_carros++;
	return carro_novo;
}




/**
 * Funcao publica que procura um Carro no sistema de acordo com a matricula dada
 * Na realidade, a mesma matricula pode aparecer mais do que uma vez no sistema
 * se o carro entrar e sair diversas vezes de parques, mas esta funcao
 * procura A instancia do Carro que esta dentro dum parque.
 *
 * Para procurar, acedo ao indice da hashtable que o hash da matricula indica e
 * percorro a lista ligada nesse indice 
 *
 * Se encontrar, retorna um ponteiro para o Carro (ou seja a instancia dessa
 * matricula que esta dentro dum parque). Caso contrario, retorna NULL
 */
Carro* procura_carro_dentro_parque(char matricula[], Carro_ptr** hashtable) {
	int valor_hashed;
	Carro_ptr* carro_ptr;

	valor_hashed = hash(matricula);
	carro_ptr = hashtable[valor_hashed];

	while(carro_ptr != NULL) {
		if( !strcmp( (*(*carro_ptr).carro).matricula, matricula ) \
		&& (*(*carro_ptr).carro).data_hora_saida == NULL)
			return (*carro_ptr).carro;

		carro_ptr = (*carro_ptr).prox;
	}

	return NULL;
}




/**
 * Funcao publica que dado um parque, remove todos os registos de 
 * carros presentes nele
 * 
 * Para cada carro, acedo a hashtable e removo o carro_ptr na hashtable que
 * aponta para ele, mantendo todos os nos da lista ligados
 *
 * (ligo o no anterior ao no a seguir aquele que vou remover)
 *
 * Nota: nao preciso de libertar o nome do parque em cada carro pois esse
 * pointer aponta para o mesmo sitio que o pointer da estrutura parque
 * e esse pointer ja e libertado na funcao remove_parque
 */
void remove_carros_parque(Parque* parque, Carro_ptr** hashtable) {
	Carro* carro, *carro_aux;
	int valor_hashed;
	Carro_ptr* carro_ptr, *carro_ptr_anterior;

	carro = (*parque).carros;
	while(carro != NULL) {
		valor_hashed = hash((*carro).matricula);

		//se carro_ptr do inicio da lista for o que aponta para o carro
		if( (*hashtable[valor_hashed]).carro == carro ) {
			carro_ptr = hashtable[valor_hashed];
			
			hashtable[valor_hashed] = (*carro_ptr).prox;
			free(carro_ptr);
		}
		
		//caso contrario, procuro o carro_ptr anterior aquele que aponta
		//para o carro que quero remover
		else {
			carro_ptr_anterior = hashtable[valor_hashed];
			while( (*(*carro_ptr_anterior).prox).carro != carro )
				carro_ptr_anterior = (*carro_ptr_anterior).prox;

			carro_ptr = (*carro_ptr_anterior).prox;
			(*carro_ptr_anterior).prox = (*carro_ptr).prox;
			free(carro_ptr);
		}
		
		free((*carro).data_hora_entrada);
		free((*carro).data_hora_saida);
		
		carro_aux = carro;
		carro = (*carro).prox;
		free(carro_aux);
	}
}




/**
 * Funcao interna que adiciona uma instancia a lista de instancias e insere
 * logo em ordem alfabetica
 * 
 * Nota:
 * na lista ligada da hashtable as instancias dos carros estao a ser percorridas
 * por ordem contraria a de entrada logo,irao ser adicionadas a esta lista de
 * modo a que as instancias mais antigas fiquem automaticamente no inicio desta
 * lista (para um mesmo parque pois prioritiza-se a ordem ascii dos seus nomes)
 */

 Carro_ptr* adiciona_instancia(Carro_ptr* inicio_lista, Carro* carro) {
	Carro_ptr* carro_ptr;
	Carro_ptr* carro_ptr_novo;

	carro_ptr = inicio_lista;
	carro_ptr_novo = (Carro_ptr*) malloc(sizeof(Carro_ptr));
	(*carro_ptr_novo).carro = carro;

	if(inicio_lista == NULL) {
		(*carro_ptr_novo).prox = NULL;
		inicio_lista = carro_ptr_novo;
		return inicio_lista;
	}
	
	//se o parque vier antes, por ordem ascii, do que o primeiro da lista
	if( strcmp((*(*carro_ptr_novo).carro).nome_parque, \
	(*(*inicio_lista).carro).nome_parque) <= 0) {
		(*carro_ptr_novo).prox = inicio_lista;
		inicio_lista = carro_ptr_novo;
		return inicio_lista;
	}

	/**
	 * comparo o nome do parque da instancia a inserir ao nome do parque da
	 * instancia a seguir aquela em que estou. o ciclo corre ate o nome do
	 * parque da instancia a inserir for menor, ou seja, vir antes na ordem
	 * ascii. em cada iteracao do ciclo avanco para o proximo elemento
	 */	
	while( (*carro_ptr).prox != NULL && \
	strcmp((*(*carro_ptr_novo).carro).nome_parque, \
	(*(*(*carro_ptr).prox).carro).nome_parque) > 0 )
		carro_ptr = (*carro_ptr).prox;
 	
	(*carro_ptr_novo).prox = (*carro_ptr).prox;
	(*carro_ptr).prox = carro_ptr_novo;
	
	return inicio_lista;
 }




/**
 * Funcao publica que cria uma lista das varias instancias do mesmo carro
 * Estarao ordenadas pela ordem ascii e depois por ordem de entrada
 */
Carro_ptr* cria_lista_instancias(char matricula[], Carro_ptr** hashtable) {
	int valor_hashed;
	Carro_ptr* carro_ptr;
	Carro_ptr* inicio_lista = NULL;

	valor_hashed = hash(matricula);
	carro_ptr = hashtable[valor_hashed];

	while(carro_ptr != NULL) {
		//verificar que estou a selecionar o carro que quero caso haja
		//carros diferentes na lista do mesmo indice da hashtable
		if(!strcmp( (*(*carro_ptr).carro).matricula, matricula))
			inicio_lista = adiciona_instancia(inicio_lista, \
			(*carro_ptr).carro);

		carro_ptr = (*carro_ptr).prox;
	}
	
	return inicio_lista;
}	




/**
 * Funcao publica para libertar a memoria alocada para a lista de instancias
 * O ponteiro percorre_lista percorre a lista enquanto o inicio_lista
 * aponta para a memoria que vai ser libertada, sendo depois atualizado
 */
void liberta_lista_instancias(Carro_ptr* inicio_lista) {
	Carro_ptr* percorre_lista;
	
	while(inicio_lista != NULL) {
		percorre_lista = (*inicio_lista).prox;
		free(inicio_lista);
		inicio_lista = percorre_lista;
	}
}       
	
