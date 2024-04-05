//Modulo dedicado a fazer operacoes relacionadas com a hashtable

#include <stdio.h>
#include <stdlib.h>

#include "carros.h"
#include "constantes.h"




/**
 * Funcao publica que cria e inicializa a hashtable
 * A hashtable e um vetor de tamanho fixo em que cada posicao contem um ponteiro
 * para um ponteiro para um Carro_ptr
 */ 
Carro_ptr** cria_hashtable() {
	int i;
	Carro_ptr** hashtable;
	hashtable = (Carro_ptr**) malloc(sizeof(Carro_ptr*) \
	* TAMANHO_HASHTABLE);

	for(i = 0; i < TAMANHO_HASHTABLE; i++)
	       hashtable[i] = NULL;
	
	return hashtable;
}




/**
 * Funcao de hashing
 * soma os valores ascii de todos os caracteres da matricula (excluindo os -),
 * multiplicando essa soma pelo numero primo 41 antes de cada soma
 *
 * por ultimo calcula o resto da divisao inteira desse valor pelo comprimento
 * da hashtable e retorna-o
*/
int hash(char matricula[]) {
	int i;
	unsigned int soma_caract = 0;
	for(i = 0; i < COMP_MATRICULA; i++) {
		if(i == 2 || i == 5)
			continue;
		soma_caract += soma_caract * 41 + matricula[i];
	}

	return soma_caract % TAMANHO_HASHTABLE;
}

