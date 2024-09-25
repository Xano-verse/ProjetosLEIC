#ifndef _CONSTANTES_H_
#define _CONSTANTES_H_


/**
 * Notar que:
 * As constantes de comprimento da matricula ou das datas sao frequentemente
 * usadas juntamente com um termo +1. Isto deve-se a usa-las na definicao de
 * strings e precisar de espaco para o caracter \0. Contudo, e-me util manter
 * as constantes com o valor real dos comprimentos e nao incluir o +1 na sua
 * definicao.
 */


#define COMP_MATRICULA 8

#define TAMANHO_HASHTABLE 419

#define COMPONENTES_DATA 5

#define COMP_DATA 12

//DD-MM-AAAA HH:MM sao 16 caracteres (contando com o espaco)
#define COMP_DATA_FORMATADA 16


#define MINUTOS_NUMAHORA 60
#define HORAS_NUMDIA 24
#define DIAS_NUMANO 365


#endif
