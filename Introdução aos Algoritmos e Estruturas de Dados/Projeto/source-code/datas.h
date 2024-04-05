#ifndef _DATAS_H_
#define _DATAS_H_


char* cria_data(int dia, int mes, int ano, int hora, int minuto);

int valida_data(int dia, int mes, int ano, int hora, int minuto);

char* formata_data(char data[]);

int calcula_minutos(char* data_hora_entrada, char* data_hora_saida);


#endif
