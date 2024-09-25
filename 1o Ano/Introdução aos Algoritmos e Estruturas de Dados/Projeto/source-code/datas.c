//Modulo dedicado a manipulacao e operacoes com datas

#include <stdio.h>
#include <stdlib.h>

#include "datas.h"
#include "constantes.h"




/**
 * Funcao publica que recebe as varias componentes de uma data e organiza-as
 * no formato AAAAMMDDHHMM. Por exemplo, 22-03-2024 00:51 fica 202403220051
 */
char* cria_data(int dia, int mes, int ano, int hora, int minuto) {
	char* data;
	data = (char*) malloc(sizeof(char) * COMP_DATA + 1);
	int i;

	for(i = 0; i < COMP_DATA; i++)
		data[i] = '0';

	data[0] += ano / 1000;
	if( (ano = ano % 1000) > 99)
		data[1] += ano / 100;
	if( (ano = ano % 100) > 9)
		data[2] += ano / 10;
	data[3] += ano % 10;

	if(mes > 9)
		data[4] += mes / 10;
	data[5] += mes % 10;
	
	if(dia > 9)
		data[6] += dia / 10;
	data[7] += dia % 10;

	if(hora > 9)
		data[8] += hora / 10;
	data[9] += hora % 10;

	if(minuto > 9)
		data[10] += minuto / 10;
	data[11] += minuto % 10;

	data[12] = '\0';

	return data;

}




/**
 * Funcao publica que recebe as varias componentes de uma data e verifica se
 * sao todas validas. Se alguma componente nao for valida, a data nao e valida
 * e por isso a funcao retorna 0. Se for valida, retorna 1.
 */
int valida_data(int dia, int mes, int ano, int hora, int minuto) {
	if(dia <= 0 || mes <= 0 || mes > 12 || ano > 9999 || \
	hora < 0 || hora > 23 || minuto < 0 || minuto > 59)
		return 0;
	
	switch(mes) {
		case 2:
			if(dia > 28)
				return 0;
			break;
		case 4:
		case 6:
		case 9:
		case 11:
			if(dia > 30)
				return 0;
			break;
		default:
			if(dia > 31)
				return 0;
	}
	
	return 1;
}




/**
 * Funcao publica que recebe uma string com uma data no formato AAAAMMDDHHMM
 * e transforma-a para o formato DD-MM-AAAA HH:MM.
 *
 * Tal como a funcao cria_data, esta funcao devole uma string dinamicamente
 * alocada. Contudo, no cria_data a string estara sempre associada a uma
 * estrutura. Aqui, apenas devolvo para poder imprimir no ecra, e vital ter o
 * cuidado de libertar esta string assim que a imprimir e ja nao precisar dela.
 */
char* formata_data(char data[]) {
	char* data_formatada;
	
	data_formatada = (char*) malloc(sizeof(char) * COMP_DATA_FORMATADA + 1);	
	
	//Dia
	data_formatada[0] = data[6];
	data_formatada[1] = data[7];
	data_formatada[2] = '-';

	//Mes
	data_formatada[3] = data[4];
	data_formatada[4] = data[5];
	data_formatada[5] = '-';

	//Ano
	data_formatada[6] = data[0];
	data_formatada[7] = data[1];
	data_formatada[8] = data[2];
	data_formatada[9] = data[3];
	data_formatada[10] = ' ';

	//Hora
	data_formatada[11] = data[8];
	data_formatada[12] = data[9];
	data_formatada[13] = ':';
	
	//Minuto
	data_formatada[14] = data[10];
	data_formatada[15] = data[11];
	data_formatada[16] = '\0';

	return data_formatada;
}




/**
 * Funcao interna que calcula a diferenca em minutos entre dois meses.
 * 
 * E auxiliar a funcao calcula_meses_anos e, consequentemente, a funcao
 * calcula_minutos (raciocinio completo explicado nessa funcao)
 */
int calcula_minutos_entre_meses(int mes_entrada, int mes_saida, int excedeu) {
	int num_minutos = 0, i;

	for(i = mes_entrada + excedeu; i < mes_saida; i++) {
		if(i == 2)
			num_minutos += 28 * HORAS_NUMDIA * MINUTOS_NUMAHORA;
		else if( (i <= 6 && i % 2 == 0) \
			|| (i > 7 && i % 2 != 0) )
			num_minutos += 30 * HORAS_NUMDIA * MINUTOS_NUMAHORA;
		else
			num_minutos += 31 * HORAS_NUMDIA * MINUTOS_NUMAHORA;
		}

	return num_minutos;
}




/**
 * Funcao interna que calcula a diferenca em minutos entre dois meses e
 * entre dois anos. Usa a funcao calcula_minutos_entre_meses para fazer
 * o calculo em si dos minutos entre os meses, mas controla o comportamento
 * desta funcao consoante o caso.
 *
 * E auxiliar a funcao calcula_minutos.
 * (raciocinio completo explicado nessa funcao)
 */
void calcula_meses_anos(int* num_minutos_ptr, int mes_entrada, int mes_saida, \
int ano_entrada, int ano_saida, int* excedeu_ptr) {
	if(mes_entrada <= mes_saida) {
		(*num_minutos_ptr) += calcula_minutos_entre_meses(mes_entrada, \
		mes_saida, (*excedeu_ptr));
		(*excedeu_ptr) = 0;
	}
	else {
		(*num_minutos_ptr) += calcula_minutos_entre_meses(mes_entrada, \
		13, (*excedeu_ptr));
		(*num_minutos_ptr) += calcula_minutos_entre_meses(1, \
		mes_saida, (*excedeu_ptr));
		(*excedeu_ptr) = 1;
	}

	//ano entrada e sempre menor que o de saida, nao e uma medida ciclica
	(*num_minutos_ptr) += (ano_saida - ano_entrada - (*excedeu_ptr)) \
	* DIAS_NUMANO * HORAS_NUMDIA * MINUTOS_NUMAHORA;
}




/**
 * Funcao interna que calcula a diferenca em minutos entre dois dias.
 *
 * E auxiliar a funcao calcula_minutos.
 * (raciocinio completo explicado nessa funcao)
 */
void calcula_dias(int* num_minutos_ptr, int dia_entrada, int dia_saida, \
int mes_entrada, int* excedeu_ptr) {
	if(dia_entrada <= dia_saida) {
		(*num_minutos_ptr) += (dia_saida - dia_entrada \
		- (*excedeu_ptr)) * HORAS_NUMDIA * MINUTOS_NUMAHORA;
		(*excedeu_ptr) = 0;
	}
	else {
		if( (mes_entrada <= 6 && mes_entrada % 2 == 0) \
		|| (mes_entrada > 7 && mes_entrada % 2 != 0) )
			(*num_minutos_ptr) += (30 - dia_entrada \
			- (*excedeu_ptr)) * HORAS_NUMDIA * MINUTOS_NUMAHORA;	
		else
			(*num_minutos_ptr) += (31 - dia_entrada \
			- (*excedeu_ptr)) * HORAS_NUMDIA * MINUTOS_NUMAHORA;
		
		(*num_minutos_ptr) += dia_saida * HORAS_NUMDIA \
		* MINUTOS_NUMAHORA;
		(*excedeu_ptr) = 1;
	}
}




/**
 * Funcao interna que calcula a diferenca em minutos entre dois minutos e
 * entre duas horas.
 *
 * E auxiliar a funcao calcula_minutos.
 * (raciocinio completo explicado nessa funcao)
 */
void calcula_minutos_horas(int* num_minutos_ptr, int minuto_entrada, \
int minuto_saida, int hora_entrada, int hora_saida, int* excedeu_ptr) {
	if(minuto_entrada <= minuto_saida)
		(*num_minutos_ptr) = minuto_saida - minuto_entrada;
	else {
		(*num_minutos_ptr) = MINUTOS_NUMAHORA - minuto_entrada;
		(*num_minutos_ptr) += minuto_saida;
		(*excedeu_ptr) = 1;
	}

	//excedeu volta a 0 sempre que a unidade em que estou nao excede
	if(hora_entrada <= hora_saida) {
		(*num_minutos_ptr) += (hora_saida - hora_entrada \
		- (*excedeu_ptr)) * MINUTOS_NUMAHORA;
		(*excedeu_ptr) = 0;
	}
	else {
		(*num_minutos_ptr) += (HORAS_NUMDIA - hora_entrada \
		- (*excedeu_ptr)) * MINUTOS_NUMAHORA;
		(*num_minutos_ptr) += hora_saida * MINUTOS_NUMAHORA;
		(*excedeu_ptr) = 1;
	}
}




/**
 * Funcao que calcula o numero de minutos entre duas datas
 * Comeco por extrair das duas strings os valores de cada componente da data
 *
 * Uso funcoes auxiliares para calcular os minutos entre as varias componentes
 * com o seguinte raciocinio:
 *
 * Calculo a diferenca entre os dois minutos que tenho
 * Se o minuto de saida for menor do que o de entrada entao excedi a unidade
 * do minuto e significa que avancei para a hora seguinte
 * Neste caso, calculo os minutos desde o minuto de entrada ate aos 60 minutos
 * e depois os minutos do 0 ate ao minuto de saida
 * Atualizo tambem a variavel "excedeu"
 * 
 * Depois, faco o mesmo raciocinio para as unidades seguintes
 * Sempre que a saida e menor que a entrada calculo os minutos ate ao limite
 * dessa unidade (60 minutos, 24 horas, etc) e depois os minutos do 0 ate
 * a saida. Quando a entrada e menor do que a saida faco apenas a diferenca,
 * converto para minutos se precisar, e deixo as unidades seguintes
 * preocuparem-se com as outras diferencas
 */
int calcula_minutos(char* data_hora_entrada, char* data_hora_saida) {
	int num_minutos, minuto_entrada, minuto_saida, hora_entrada, hora_saida;
	int dia_entrada,dia_saida,mes_entrada,mes_saida,ano_entrada,ano_saida;

	/**
	 * variavel para ajustar ao retirar 1 unidade no caso da unidade
	 * de tempo anterior ter excedido. Por exemplo, das 12h40 para as 14h20
	 * vao 1h40mins e quando calculo os minutos ponho esta variavel a 1 para
	 * quando calcular as horas e fizer 14-12 retirar 1 unidade adicional
	 * para dar certo
	*/
	int excedeu = 0;
	
	minuto_entrada = (data_hora_entrada[10]-'0')*10 + \
	(data_hora_entrada[11]-'0');
	minuto_saida = (data_hora_saida[10]-'0')*10 + (data_hora_saida[11]-'0');
	
	hora_entrada = (data_hora_entrada[8]-'0')*10 + \
	(data_hora_entrada[9]-'0');
	hora_saida = (data_hora_saida[8]-'0')*10 + (data_hora_saida[9]-'0');

	dia_entrada = (data_hora_entrada[6]-'0')*10 + \
	(data_hora_entrada[7]-'0');
	dia_saida = (data_hora_saida[6]-'0')*10 + (data_hora_saida[7]-'0');

	mes_entrada = (data_hora_entrada[4]-'0')*10 + \
	(data_hora_entrada[5]-'0');
	mes_saida = (data_hora_saida[4]-'0')*10 + (data_hora_saida[5]-'0');
	
	ano_entrada = (data_hora_entrada[0]-'0')*1000 + \
	(data_hora_entrada[1]-'0')*100 + (data_hora_entrada[2]-'0')*10 + \
	(data_hora_entrada[3]-'0');
	
	ano_saida = (data_hora_saida[0]-'0')*1000 + \
	(data_hora_saida[1]-'0')*100 + (data_hora_saida[2]-'0')*10 + \
	(data_hora_saida[3]-'0');

	calcula_minutos_horas(&num_minutos, minuto_entrada, minuto_saida, \
	hora_entrada, hora_saida, &excedeu);
	calcula_dias(&num_minutos, dia_entrada, dia_saida, mes_entrada, \
	&excedeu);
	calcula_meses_anos(&num_minutos, mes_entrada, mes_saida, \
	ano_entrada, ano_saida, &excedeu);
		
	return num_minutos;
}

