#ifndef _PARQUES_H_
#define _PARQUES_H_


/**
 * Estrutura para representar um parque. Nunca perco as referencias para as
 * listas de Saidas e de Carros pois cada Parque guarda os ponteiros para
 * elas
 */ 
typedef struct parque_no {
 	char* nome;
 	int capacidade, num_carros;
 	float valor15, valor15_apos1h, max_diario;
 	Carro* carros;
	Saida* primeiro_dia_saidas, *ultimo_dia_saidas;
	struct parque_no* prox;
} Parque;


/**
 * Esta estrutura guarda um ponteiro para caracteres que serao o nome do parque
 * e um inteiro que contem o indice no buffer logo a seguir ao nome, para saber
 * de onde comecar para ler os outro argumentos
 */
typedef struct nome_parque_e_indice {
	char* nome;
	int depois_nome;
} Nome_parque;


void cria_parque(char* nome, int capacidade, float valor15, \
float valor15_apos1h, float max_diario);

void imprime_parques();

void imprime_parques_ordem_ascii(int num_parques);

Parque* procura_parque(char* nome);

void remove_parque(Parque* parque, Carro_ptr** hashtable);

void remove_todos_parques(Carro_ptr** hashtable);

float calcula_faturacao(Parque* parque, Carro* carro);


#endif
