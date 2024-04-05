#ifndef _SAIDAS_H_
#define _SAIDAS_H_


/**
 * Estrutura para representar um dia em que houve uma saida. Para cada Parque
 * terei uma lista destes dias e cada dia tera uma lista de Carro_ptrs dos
 * carros que sairam nesse dia.
 */
typedef struct saida_pordia {
	int dia;
	Carro_ptr* primeiro_carro, *ultimo_carro;
	struct saida_pordia* prox;
} Saida;


void adiciona_dia_saida(Parque* parque, int dia);

void adiciona_carro_saida(Saida* saida, Carro* carro);

void remove_todas_saidas(Parque* parque);

void lista_faturacoes_diarias(Parque* parque);

void lista_faturacoes_dum_dia(Parque* parque, char* data);


#endif
