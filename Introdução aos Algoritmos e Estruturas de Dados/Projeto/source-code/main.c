//Projeto IAED - Alexandre Carapeto Delgado 109441 2023-2024

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constantes.h"
#include "carros.h"
#include "saidas.h"
#include "parques.h"
#include "datas.h"
#include "hashtable.h"



/**
 * Guia geral
 * Estruturas principais: Carro, Parque
 * Estruturas auxiliares: Carro_ptr, Saida, Nome_parque
 * 
 * Organizacao de dados: lista ligada de Parques; cada Parque tem uma lista
 * ligada de Carros e uma lista ligada de Saidas; cada Saida representa um
 * dia e tem uma lista ligada de Carro_ptrs; hashtable geral com listas ligadas
 * de Carro_ptrs em cada indice; cada Carro_ptr aponta para um Carro;
 */




/**
 * Funcao a executar apenas no final do programa. Liberta toda a memoria alocada
 * manualmente que ainda nao foi libertada.
 */
void liberta_tudo(Carro_ptr** hashtable, char* ultima_data) {	
	remove_todos_parques(hashtable);
	free(hashtable);
	free(ultima_data);

}


/**
 * Funcao para verificar se uma matricula e valida. Retorna 1 se for e 0 caso
 * contrario. Conto os pares de numeros e de letras que estao juntos.
 * Tem que haver 1 de cada.
 */

int valida_matricula(char matricula[]) {
	int i, pares_nums = 0, pares_letras = 0;

	for(i = 0; i < COMP_MATRICULA; i += 2) {
		if(matricula[i] == '-') {
			//recuamos 1 posicao para quando adicionarmos 2 dar bem
			i--;
			continue;
		}

		if(matricula[i] >= '0' && matricula[i] <= '9' \
		&& matricula[i+1] >= '0' && matricula[i+1] <= '9')
			pares_nums++;
		
		else if(matricula[i] >= 'A' && matricula[i] <= 'Z' \
		&& matricula[i+1] >= 'A' && matricula[i+1] <= 'Z')
			pares_letras++;
		
		//se o par nao for apenas digitos nem apenas letras
		else
			return 0;

	}

	//e preciso haver pelo menos um par de cada tipo
	if(pares_nums == 0 || pares_letras == 0)
		return 0;

	return 1;
}



/**
 * Funcao que extrai o nome do parque do buffer fornecido.
 * Guarda o nome do parque e o indice do caracter logo a seguir a este numa
 * estrutura Nome_parque.
 */
Nome_parque le_nome_parque(char* buffer) {
	Nome_parque nome_parque;
	int tamanho_nome = 0;
	int i, j, i_depois_letras;

	//comeca no 2 pois posicao 0 e o comando e a 1 um espaco obrigatorio
	for(i = 2; buffer[i] == ' '; i++);
	
	if(buffer[i] == '"') {
		for(i++ ; buffer[i] != '"'; i++)
			tamanho_nome++;
		//+1 para saltar ultimas aspas quando ler os outros argumentos
		nome_parque.depois_nome = i + 1;
	}
	else {
		//ate encontrar espaco (geral) ou ate buffer acabar (comando f)
		for( ; buffer[i] != ' ' && buffer[i+1] != '\0'; i++)
			tamanho_nome++;
		//como acima, guarda indice a seguir ao ultimo caracter do nome
		nome_parque.depois_nome = i;
	}
	//quando nao ha aspas esta variavel e igual a depois_nome,
	//quando ha entao marca a ultima aspa para nao a incluir depois
	i_depois_letras = i;
	
	nome_parque.nome = (char*) malloc(sizeof(char) * tamanho_nome + 1);
	for(i = i - tamanho_nome, j = 0; i < i_depois_letras; i++, j++)
		nome_parque.nome[j] = buffer[i];
	nome_parque.nome[j] = '\0';
		
	return nome_parque;
}




/**
 * Funcao para verificar os erros dos comandos f e r. Retorna 0 se nao houverem
 * erros. Variavel com_data serve apenas para o comando f com 2 argumentos e
 * sera 1 nesse caso de modo a entrar no if
 */
int erros_f_r(int com_data, Parque* parque, char* nome_parque, char* data, \
char* ultima_data, int dia, int mes, int ano, int hora, int minuto) {
	if(!parque) {
		printf("%s: no such parking.\n", nome_parque);
		return 1;
	}

	if(com_data) {
		if( !valida_data(dia, mes, ano, hora, minuto) \
		|| strcmp(data, ultima_data) > 0) {
			printf("invalid date.\n");
			return 2;
		}
	}

	return 0;
}




/**
 * Funcao para executar o comando r. 
 * Depois de ler o input e verificar que nao ha erros, remove o parque indicado,
 * diminui o numero de parques e imprime os parques que sobram por ordem
 * lexicografica pelo codigo ASCII
 */
void r_comando(char* buffer, Carro_ptr** hashtable, int* num_parques_ptr) {
	Nome_parque nome_parque;
	Parque* parque;

	nome_parque = le_nome_parque(buffer);
	parque = procura_parque(nome_parque.nome);

	if(erros_f_r(0, parque, nome_parque.nome, NULL, NULL, 0, 0, 0, 0, 0)) {
		free(nome_parque.nome);
		return;
	}

	remove_parque(parque, hashtable);
	(*num_parques_ptr)--;

	imprime_parques_ordem_ascii((*num_parques_ptr));
	
	free(nome_parque.nome);
}




/**
 * Funcao para executar o comando f.
 * Depois de ler o input e verificar que nao ha erros, imprime as faturacoes
 * de cada dia caso o input tenha 1 argumento e, caso tenha 2, imprime as
 * faturacoes de cada carro no dia fornecido
 */
void f_comando(char* buffer, char* ultima_data) {
	Nome_parque nome_parque;
	int dia, mes, ano;
	char* data;
	Parque* parque;

	nome_parque = le_nome_parque(buffer);
	parque = procura_parque(nome_parque.nome);

	if(buffer[nome_parque.depois_nome] == '\n') {
		if(erros_f_r(0, parque, nome_parque.nome, \
		NULL, NULL, 0, 0, 0, 0, 0)) {
			free(nome_parque.nome);
			return;
		}

		lista_faturacoes_diarias(parque);
	}
	
	else {
		sscanf(buffer+nome_parque.depois_nome, "%d-%d-%d", \
		&dia, &mes, &ano);
		
		data = cria_data(dia, mes, ano, 0, 0);
		if(erros_f_r(1, parque, nome_parque.nome, data, ultima_data, \
		dia, mes, ano, 0, 0)) {
			free(data);
			free(nome_parque.nome);
			return;
		}
		
		lista_faturacoes_dum_dia(parque, data);

		free(data);
	}
	
	free(nome_parque.nome);
}




/**
 * Funcao para verificar os erros do comando v.
 * Retorna 0 se nao houverem erros.
 */
int erros_v(char matricula[], Carro_ptr* instancia_carro) {
	if(!valida_matricula(matricula)) {
		printf("%s: invalid licence plate.\n", matricula);
		return 1;
	}

	if(instancia_carro == NULL) {
		printf("%s: no entries found in any parking.\n", matricula);
		return 2;
	}

	return 0;
}




/**
 * Funcao para executar o comando v.
 * Depois de ler o input e verificar que nao ha erros, percorro a lista de
 * instancias de um carro imprimindo o nome do parque e as datas e horas
 * de cada instancia.
 *
 * A lista esta ordenada pelo nome dos parques e depois pela data de entrada
 */
void v_comando(char* buffer, Carro_ptr** hashtable) {
	char matricula[COMP_MATRICULA+1];
	char* entrada_formatada, *saida_formatada;
	Carro_ptr* inicio_lista, *instancia_carro;
	Carro* carro;

	sscanf(buffer+2, "%s", matricula);
	
	inicio_lista = cria_lista_instancias(matricula, hashtable);
	
	if(erros_v(matricula, inicio_lista))
		return;
	
	instancia_carro = inicio_lista;
	while(instancia_carro != NULL) {
		carro = (*instancia_carro).carro;
		entrada_formatada = formata_data((*carro).data_hora_entrada);

		if( (*carro).data_hora_saida == NULL)
			printf("%s %s\n", \
			(*carro).nome_parque, entrada_formatada);
		else {
			saida_formatada = formata_data(\
			(*carro).data_hora_saida);
			
			printf("%s %s %s\n", (*carro).nome_parque, \
			entrada_formatada, saida_formatada);
			
			free(saida_formatada);
		}

		free(entrada_formatada);
		instancia_carro = (*instancia_carro).prox;
	}

	liberta_lista_instancias(inicio_lista);
}



/**
 * Funcao para verificar os erros dos comandos e e s. Retorna 0 se nao houverem
 * erros. A variavel comando permite escolher que condicoes verificar ja que
 * ha erros especificos a cada comando e erros comuns aos dois. 
 */
int erros_e_s(char comando, Parque* parque, char* nome_parque, \
char matricula[], Carro_ptr** hashtable, char* data, char* ultima_data, \
int dia, int mes, int ano, int hora, int minuto, Carro* carro_a_sair) {
	if(!parque) {
		printf("%s: no such parking.\n", nome_parque);
		return 1;
	}

	if(comando == 'e' && (*parque).capacidade == (*parque).num_carros) {
		printf("%s: parking is full.\n", nome_parque);
		return 2;
	}

	if(!valida_matricula(matricula)) {
		printf("%s: invalid licence plate.\n", matricula);
		return 3;
	}
	
	//se procura for NULL entao o carro esta dentro dum parque
	if(comando == 'e' && procura_carro_dentro_parque(matricula,hashtable)) {
		printf("%s: invalid vehicle entry.\n", matricula);
		return 4;
	}
	
	//se o parque onde esta o carro nao for igual ao parque introduzido
	if(comando == 's' && (carro_a_sair == NULL || \
	strcmp((*carro_a_sair).nome_parque, nome_parque))) {
		printf("%s: invalid vehicle exit.\n", matricula);
		return 5;
	}

	if(!valida_data(dia,mes,ano,hora,minuto)||strcmp(data,ultima_data)<0) {	
		printf("invalid date.\n");
		return 6;
	}

	return 0;

}




/**
 * Funcao auxiliar a funcao s_comando.
 * Atualiza os atributos necessarios do carro e do parque e a ultima data do
 * sistema. Depois, calcula a faturacao e imprimo-a juntamente com
 * as outras informacoes
 */
void s_comando_segunda_parte(Parque* parque, Carro* carro, char* data, \
char* ultima_data) { 
	char* entrada_formatada, *saida_formatada;
	float valor_pagar;

	(*carro).data_hora_saida = data;
	(*parque).num_carros--;
	strcpy(ultima_data, data);

	valor_pagar = calcula_faturacao(parque, carro);
	
	entrada_formatada = formata_data((*carro).data_hora_entrada);
	saida_formatada = formata_data(data);
	
	printf("%s %s %s %.2f\n", (*carro).matricula, entrada_formatada, \
	saida_formatada, valor_pagar);
	
	free(entrada_formatada);
	free(saida_formatada);
}




/**
 * Funcao para executar o comando s.
 * Depois de ler o input e verificar que nao ha erros, vejo se ja existe, no
 * parque dado, alguma saida no dia dado. Se nao houver, adiciono uma Saida para
 * esse dia a lista de Saidas. Depois, adiciono um Carro_ptr a lista nessa Saida
 * que aponta para o Carro que acabou de sair. 
 */
void s_comando(char* buffer, Carro_ptr** hashtable, char* ultima_data) {
	Parque* parque;
	Carro* carro;
	int dia, mes, ano, hora, minuto;
	char* data;//, *entrada_formatada, *saida_formatada;
	char matricula[COMP_MATRICULA+1];
	//float valor_pagar;
	Nome_parque nome_parque = le_nome_parque(buffer);
	
	
	sscanf(buffer+nome_parque.depois_nome, "%s%d-%d-%d%d:%d", \
	matricula, &dia, &mes, &ano, &hora, &minuto);

	parque = procura_parque(nome_parque.nome);	
	data = cria_data(dia, mes, ano, hora, minuto);
	
	/**
	 * Importante notar que: carros sao sempre adicionados ao inicio da
	 * lista (para cada indice da hashtable), logo se um carro esta num
	 * parque, mesmo havendo outras instancias dele que ja sairam de outros
	 * ou do mesmo parque, irei aceder sempre aquela que esta num parque
	 */
	carro = procura_carro_dentro_parque(matricula, hashtable);


	if(erros_e_s('s', parque, nome_parque.nome, matricula, hashtable, \
	data, ultima_data, dia, mes, ano, hora, minuto, carro)) {
		
		free(nome_parque.nome);
		free(data);
		return;
	}

	//caso ainda nao haja uma Saida para este dia
	if( (*parque).ultimo_dia_saidas == NULL || \
	(*(*parque).ultimo_dia_saidas).dia != dia)
		adiciona_dia_saida(parque, dia);
	
	adiciona_carro_saida((*parque).ultimo_dia_saidas, carro);

	s_comando_segunda_parte(parque, carro, data, ultima_data);
	free(nome_parque.nome);
}





/**
 * Funcao para executar o comando e.
 * Depois de ler o input e verificar que nao ha erros, adiciono um Carro_ptr a
 * posicao da hashtable que o hash da matricula indica. A resolucao de colisoes
 * e feita por encadeamento numa lista ligada.
 *
 * Adiciono o carro a lista ligada de Carros do Parque, atualizo a ultima data
 * do sistema e imprimo as informacoes necessarias. 
 */
void e_comando(char* buffer, Carro_ptr** hashtable, char* ultima_data) {
	Nome_parque nome_parque;
	char* data,  matricula[COMP_MATRICULA+1];
	int dia, mes, ano, hora, minuto, valor_hashed;
	Carro_ptr* entrada_nova;
	Parque* parque;

	nome_parque = le_nome_parque(buffer);
	sscanf(buffer+nome_parque.depois_nome, "%s%d-%d-%d%d:%d", \
	matricula, &dia, &mes, &ano, &hora, &minuto);

	parque = procura_parque(nome_parque.nome);	
	data = cria_data(dia, mes, ano, hora, minuto);
	
	if(erros_e_s('e', parque, nome_parque.nome, matricula, hashtable, \
	data, ultima_data, dia, mes, ano, hora, minuto, NULL)) {
		
		free(nome_parque.nome);
		free(data);
		return;
	}

	entrada_nova = (Carro_ptr*) malloc(sizeof(Carro_ptr));	
	valor_hashed = hash(matricula);
	
	//se a posicao na hashtable nao estiver vazia
	if(hashtable[valor_hashed])
		(*entrada_nova).prox = hashtable[valor_hashed];
	
	else
		(*entrada_nova).prox = NULL;
	
	hashtable[valor_hashed] = entrada_nova;
	(*entrada_nova).carro = cria_carro(parque, matricula, data);

	strcpy(ultima_data, data);
	
	printf("%s %d\n", nome_parque.nome, \
	(*parque).capacidade - (*parque).num_carros);

	free(nome_parque.nome);
}




/**
 * Funcao para verificar os erros do comando p.
 * Retorna 0 se nao houverem erros. 
 */
int erros_p(char* nome, int capacidade, float valor15, float valor15_apos1h, \
float max_diario, int* num_parques_ptr) {
	if(procura_parque(nome)) {
		printf("%s: parking already exists.\n", nome);
		return 1;
	}

	if(capacidade <= 0) {
		printf("%d: invalid capacity.\n", capacidade);
		return 2;
	}

	if( !( 0 < valor15 \
		&& valor15 < valor15_apos1h \
		&& valor15_apos1h < max_diario) ) {
		printf("invalid cost.\n");
		return 3;
	}
	
	if( (*num_parques_ptr) == 20) {
		printf("too many parks.\n");
		return 4;
	}

	return 0;
}




/**
 * Funcao para executar o comando p.
 * Se o input nao tiver argumentos imprimo os parques que existem por ordem de
 * criacao. Depois de ler o input todo, caso tenha argumentos, e verificar que
 * nao ha erros, adiciono um Parque a lista ligada de Parques e incremento a
 * variavel numero de parques. 
 */
void p_comando(char* buffer, int* num_parques_ptr) {	
	Nome_parque nome_parque;
	int capacidade;
	float valor15, valor15_apos1h, max_diario;

	if(buffer[1] == '\n')
		imprime_parques();
	
	else {
		nome_parque = le_nome_parque(buffer);

		sscanf(buffer+nome_parque.depois_nome, "%d%f%f%f", \
		&capacidade, &valor15, &valor15_apos1h, &max_diario);
		
		if(erros_p(nome_parque.nome, capacidade, \
		valor15, valor15_apos1h, max_diario, num_parques_ptr)) {
			free(nome_parque.nome);
			return;
		}

		cria_parque(nome_parque.nome, capacidade, valor15, \
		valor15_apos1h, max_diario);
		(*num_parques_ptr)++;
	}
}




/**
 * Funcao auxiliar a funcao main que le o input ate encontrar um '\n' e escolhe
 * que funcao chamar consoante o comando introduzido.
 * Isto repete-se ate ser introduzido o comando q.
 */
void escolhe_comando(Carro_ptr** hashtable, int* num_parques_ptr, \
char* ultima_data) {

	char* buffer = NULL;
	long unsigned int buffer_size;

	while( getline(&buffer, &buffer_size, stdin) && buffer[0] != 'q') {
		switch(buffer[0]) {
			case 'p':	
				p_comando(buffer, num_parques_ptr);
				break;
			
			case 'e':
				e_comando(buffer, hashtable, ultima_data);				
				break;

			case 's':
				s_comando(buffer, hashtable, ultima_data);
				break;

			case 'v':
				v_comando(buffer, hashtable);
				break;

			case 'f':
				f_comando(buffer, ultima_data);
				break;

			case 'r':
				r_comando(buffer, hashtable, num_parques_ptr);
				break;
		}
	
		free(buffer);
		buffer = NULL;
	}
	
	free(buffer);
}


/**
 * Funcao principal.
 * Inicializa algumas variaveis que quero manter ao longo do programa todo,
 * chama a funcao para criar a hashtable e a funcao para comecar a ler inputs.
 *
 * No fim, chama a funcao liberta_tudo() para libertar todos os dados alocados
 * manualmente.
 */
int main() {
	int num_parques = 0;
	Carro_ptr** hashtable;
	char* ultima_data;
	int i;

	ultima_data = (char*) malloc(sizeof(char) * COMP_DATA + 1);
	for(i = 0; i < COMP_DATA; i++)
		ultima_data[i] = '0';
	ultima_data[COMP_DATA] = '\0';

	hashtable = cria_hashtable();

	escolhe_comando(hashtable, &num_parques, ultima_data);
	
	liberta_tudo(hashtable, ultima_data);

	return 0;
}

