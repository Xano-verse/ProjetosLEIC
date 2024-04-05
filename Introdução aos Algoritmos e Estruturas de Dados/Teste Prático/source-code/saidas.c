//Modulo dedicado a manipular Saidas, estruturas feitas para
//estarem associadas a um dia e guardarem ponteiros para uma lista de Carro_ptrs

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "carros.h"
#include "saidas.h"
#include "parques.h"
#include "datas.h"




/**
 * Funcao publica que adiciona um no a lista de saidas do parque dado e atribui
 * o dia fornecido a Saida criada. As Saidas sao adicionadas atraves da cauda
 * da lista e cada Saida tem um ponteiro para a cabeca e um ponteiro para a
 * cauda da sua lista de Carro_ptrs
 */
void adiciona_dia_saida(Parque* parque, int dia) {
	Saida* saida_nova;
	saida_nova = (Saida*) malloc(sizeof(Saida));

	if((*parque).primeiro_dia_saidas == NULL) {
		(*parque).primeiro_dia_saidas = saida_nova;
		(*parque).ultimo_dia_saidas = saida_nova;
		(*saida_nova).prox = NULL;
	}
	
	else {
		(*saida_nova).prox = (*(*parque).ultimo_dia_saidas).prox;
		(*(*parque).ultimo_dia_saidas).prox = saida_nova;
		(*parque).ultimo_dia_saidas = saida_nova;
	}
		

	(*saida_nova).dia = dia;
	(*saida_nova).primeiro_carro = NULL;
	(*saida_nova).ultimo_carro = NULL;
}




/**
 * Funcao publica que adiciona um Carro_ptr a lista ligada da Saida dada e o
 * faz apontar para o Carro dado. Os Carro_ptrs sao adicionados ao final da
 * lista atraves da cauda chamada ultimo_carro
 */
void adiciona_carro_saida(Saida* saida, Carro* carro) {
	Carro_ptr* carro_saida_nova;
	carro_saida_nova = (Carro_ptr*) malloc(sizeof(Carro_ptr));

	if((*saida).primeiro_carro == NULL) {
		(*saida).primeiro_carro = carro_saida_nova;
		(*saida).ultimo_carro = carro_saida_nova;
		(*carro_saida_nova).prox = NULL;
	}

	else {
		(*carro_saida_nova).prox = (*(*saida).ultimo_carro).prox;
		(*(*saida).ultimo_carro).prox = carro_saida_nova;
		(*saida).ultimo_carro = carro_saida_nova;
	}

	(*carro_saida_nova).carro = carro;
}




/**
 * Funcao publica que percorre a lista de Saidas do parque dado e procura pela
 * Saida associada ao dia dado. Retorna um ponteiro para essa Saida se encontrar
 * Caso contrario, retorna NULL.
 */
Saida* procura_dia_saida(Parque* parque, int dia) {
	Saida* dia_saida;

	dia_saida = (*parque).primeiro_dia_saidas;

	while(dia_saida != NULL) {
		if((*dia_saida).dia == dia)
			return dia_saida;

		dia_saida = (*dia_saida).prox;
	}

	//sera NULL
	return dia_saida;
}




/**
 * Funcao publica que, dado um parque, remove todas as suas Saidas tal como
 * todos os Carro_ptrs associados a cada uma delas.
 *
 * Os ponteiros "aux" apontam para o no a libertar, enquanto que
 * os outros vao avancando para o seguinte.
 */
void remove_todas_saidas(Parque* parque) {
	Saida* saida, *saida_aux;
	Carro_ptr* carro_ptr, *carro_ptr_aux;

	saida = (*parque).primeiro_dia_saidas;
	while(saida != NULL) {
		carro_ptr = (*saida).primeiro_carro;

		while(carro_ptr != NULL) {
			carro_ptr_aux = carro_ptr;
			carro_ptr = (*carro_ptr).prox;
			free(carro_ptr_aux);
		}
		
		saida_aux = saida;
		saida = (*saida).prox;
		free(saida_aux);
	}
}




/**
 * Funcao interna que calcula todas as faturacoes de cada carro dum parque
 * num determinado dia e soma-as.
 * 
 * Serve apenas para auxiliar a funcao lista_faturacoes_diarias
 */ 
float calcula_faturacao_dum_dia(Parque* parque, Saida* dia_saida) {
	Carro_ptr* carro_ptr;
	float soma_das_faturacoes = 0;

	carro_ptr = (*dia_saida).primeiro_carro;

	while(carro_ptr != NULL) {
		soma_das_faturacoes += calcula_faturacao(parque, \
		(*carro_ptr).carro);
		carro_ptr = (*carro_ptr).prox;
	}
	
	return soma_das_faturacoes;
}




/**
 * Funcao publica que, para cada dia em que houve saidas num parque,
 * imprime o total faturado.
 */
void lista_faturacoes_diarias(Parque* parque) {
    	Saida* dia_saida;
    	char* data_saida_formatada, *formatada_sem_horas;
    	float faturacao_diaria;

    	dia_saida = (*parque).primeiro_dia_saidas;

    	while(dia_saida != NULL) {
        	//data saida e a mesma para todos nesse dia
		data_saida_formatada = formata_data( \
		(*(*(*dia_saida).primeiro_carro).carro).data_hora_saida );
		formatada_sem_horas = strndup(data_saida_formatada, 10);
		
		faturacao_diaria = calcula_faturacao_dum_dia(parque, dia_saida);
		printf("%s %.2f\n", formatada_sem_horas, faturacao_diaria);
		
		free(data_saida_formatada);
		free(formatada_sem_horas);
		dia_saida = (*dia_saida).prox;
	}
}




/**
 * Funcao publica que, dado um parque, lista as faturacoes individuais de cada
 * carro para um determinado dia.
 */
void lista_faturacoes_dum_dia(Parque* parque, char* data) {
	int dia;
	char hora[3], minuto[3];
	float valor_pago;
	Saida* dia_saida;
	Carro_ptr* carro_ptr;

	dia = (data[6]-'0') * 10 + data[7]-'0';
	dia_saida = procura_dia_saida(parque, dia);
	
	if(dia_saida == NULL)
		return;
	
	carro_ptr = (*dia_saida).primeiro_carro;
	while(carro_ptr != NULL) {
		hora[0] = (*(*carro_ptr).carro).data_hora_saida[8];
		hora[1] = (*(*carro_ptr).carro).data_hora_saida[9];
		hora[2] = '\0';
		minuto[0] = (*(*carro_ptr).carro).data_hora_saida[10];
		minuto[1] = (*(*carro_ptr).carro).data_hora_saida[11];
		minuto[2] = '\0';

		valor_pago = calcula_faturacao(parque, (*carro_ptr).carro);

		printf("%s %s:%s %.2f\n", (*(*carro_ptr).carro).matricula, \
		hora, minuto, valor_pago);
	
		carro_ptr = (*carro_ptr).prox;
	}
}	

