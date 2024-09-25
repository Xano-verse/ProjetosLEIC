//Modulo dedicado a manipulacao e organizacao das estruturas Parque

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "carros.h"
#include "saidas.h"
#include "parques.h"
#include "datas.h"
#include "constantes.h"




/**
 * Como nunca posso perder referencia para a lista ligada de Parques e nao a
 * consigo guardar em nenhuma estrutura (ao contrario, por exemplo, das listas
 * de Carros que sao guardadas em cada Parque), uso duas variaveis globais
 * apenas a este modulo. Nao passo estes ponteiros sempre para as funcoes como
 * alternativa para manter a barreira de abstracao mais definida e o codigo mais
 * modular.
 *
 * primeiro_parque funciona como cabeca da lista e ultimo_parque como cauda
 */
static Parque* primeiro_parque = NULL;
static Parque* ultimo_parque = NULL;




/**
 * Funcao publica que adiciona um Parque a lista ligada de Parques e 
 * lhe atribui as caracteristicas dadas pelo utilizador.
 */
void cria_parque(char* nome, int capacidade, float valor15, \
float valor15_apos1h, float max_diario) {
	Parque* parque_novo;
	parque_novo = (Parque*) malloc(sizeof(Parque));

	//se ainda não houver parques, o novo parque é o primeiro
	if(primeiro_parque == NULL) {
		(*parque_novo).prox = ultimo_parque;
		ultimo_parque = parque_novo;
		primeiro_parque = parque_novo;
	}

	else {
		(*parque_novo).prox = (*ultimo_parque).prox;
		(*ultimo_parque).prox = parque_novo;
		ultimo_parque = parque_novo;
	}


	(*parque_novo).nome = nome;	

	(*parque_novo).capacidade = capacidade;
	(*parque_novo).num_carros = 0;
	
	(*parque_novo).valor15 = valor15;
	(*parque_novo).valor15_apos1h = valor15_apos1h;
	(*parque_novo).max_diario = max_diario;	

	(*parque_novo).carros = NULL;
	(*parque_novo).primeiro_dia_saidas = NULL;
	(*parque_novo).ultimo_dia_saidas = NULL;
}




/**
 * Funcao publica que percorre a lista inteira de Parques e vai imprimindo os
 * seus nomes, capacidades e lugares livres.
 */
void imprime_parques() {
	Parque* parque = primeiro_parque;

	while(parque != NULL) {
		printf("%s %d %d\n", (*parque).nome, (*parque).capacidade, \
		(*parque).capacidade - (*parque).num_carros);

		parque = (*parque).prox;
	}
}




/**
 * Funcao publica que cria um vetor com os nomes dos parques ordenados
 * por ordem ascii do nome e imprime-os.
 *
 * Percorro a lista ligada de parques e vou inserindo os seus nomes no vetor
 * A insercao e feita logo pela ordem pretendida
 *
 * Comeco sempre por inserir o nome do primeiro parque no inicio do vetor
 *
 * Para cada parque, encontro a posicao onde devo inserir o nome ao avancar no
 * vetor ate o nome nao ser maior do que o nome da posicao em que estou
 * Percorro o vetor desde o final ate a posicao onde vou inserir, movendo todos
 * os elementos uma posicao para a direita e insiro o nome na posicao
 */
void imprime_parques_ordem_ascii(int num_parques) {
	char* nomes_parques[num_parques];
	Parque* parque;
	int i, j;
	
	if(primeiro_parque == NULL)
		return;

	for(i = 0; i < num_parques; i++)
		nomes_parques[i] = NULL;

	nomes_parques[0] = (*primeiro_parque).nome;

	parque = (*primeiro_parque).prox;
	while(parque != NULL) {
		for(i = 0; nomes_parques[i] != NULL && strcmp((*parque).nome, \
		nomes_parques[i]) > 0; i++) ;
		
		for(j = num_parques - 1; j != i; j--)
			nomes_parques[j] = nomes_parques[j-1];

		nomes_parques[i] = (*parque).nome;

		parque = (*parque).prox;
	}
	
	for(i = 0; i < num_parques; i++)
		printf("%s\n", nomes_parques[i]);
}




/**
 * Funcao publica que percorre a lista de Parques e procura pelo
 * parque com o nome dado.
 */
Parque* procura_parque(char* nome) {
	Parque* parque = primeiro_parque;

	while(parque != NULL) {
		if( !strcmp((*parque).nome, nome) )
			return parque;
		parque = (*parque).prox;
	}

	return NULL;
}




/**
 * Funcao publica que liberta a memoria alocada para o Parque dado e para todas
 * as informacoes associadas a ele. Chama funcoes de outros modulos para
 * ajudarem com isto.
 */
void remove_parque(Parque* parque, Carro_ptr** hashtable) {
	Parque* parque_anterior;

	if(primeiro_parque == parque) {
		primeiro_parque = (*primeiro_parque).prox;

		if(ultimo_parque == parque)
			ultimo_parque = primeiro_parque;
	}

	else {
		//procuro o parque anterior ao que quero remover
		parque_anterior = primeiro_parque;
		while((*parque_anterior).prox != parque)
			parque_anterior = (*parque_anterior).prox;

		(*parque_anterior).prox = (*parque).prox;
		
		if(ultimo_parque == parque)
			ultimo_parque = parque_anterior;
	}
	
	free((*parque).nome);
	remove_carros_parque(parque, hashtable);
	remove_todas_saidas(parque);
	
	free(parque);
}




/**
 * Funcao publica que liberta a memoria de todos os Parques existentes.
 * Comeco na cabeca lista dos Parques e removo esse parque. E importante notar
 * que a funcao remove_parque atualiza logo a cabeca da lista depois de remover
 */
void remove_todos_parques(Carro_ptr** hashtable) {
	Parque* parque;

	while(primeiro_parque != NULL) {
		parque = primeiro_parque;
		remove_parque(parque, hashtable);
	}
}




/**
 * Funcao publica que dados um Parque e um Carro calcula o valor que o Carro
 * tem que pagar quando sai do Parque.
 *
 * Calculo o numero em minutos entre a data de entrada e de saida, depois o
 * numero de dias completos em minutos dentro desse tempo e, por fim, calculo
 * o numero de blocos de 15 minutos existentes no tempo abaixo de um dia. 
 *
 * Para o numero de dias completos, uso o valor maximo diario do parque.
 * Para o tempo abaixo de um dia, uso o valor de 15 minutos para 4 dos blocos de
 * 15 minutos e o valor de 15 minutos apos 1 hora para os restantes blocos 
 * Por fim, comparo o preco abaixo de um dia com o maximo diario e escolho
 * o menor entre eles.
 *
 * Retorno a soma do preco dos dias completos e do tempo abaixo de um dia.
 */
float calcula_faturacao(Parque* parque, Carro* carro) {
	int num_minutos, dias, blocos_15mins;
	float valor_pagar = 0, valor_temp;

	num_minutos = calcula_minutos((*carro).data_hora_entrada, \
	(*carro).data_hora_saida);
	
	dias = num_minutos / (HORAS_NUMDIA * MINUTOS_NUMAHORA);
	//REMOVER DPSsubtraio os dias (em minutos) ao total de minutos
	num_minutos -= dias * HORAS_NUMDIA * MINUTOS_NUMAHORA;
	
	valor_pagar += dias * (*parque).max_diario;


	blocos_15mins = num_minutos / 15;
	//blocos_15mins inclui blocos de 15 mins nao completos
	if(num_minutos % 15 != 0)
		blocos_15mins++;

	if(blocos_15mins > 4)
		valor_temp = 4 * (*parque).valor15 + (blocos_15mins-4) \
		* (*parque).valor15_apos1h;
	else
		valor_temp = blocos_15mins * (*parque).valor15;
	
	if(valor_temp < (*parque).max_diario)
		valor_pagar += valor_temp;
	else
		valor_pagar += (*parque).max_diario;
	
	return valor_pagar;
}

