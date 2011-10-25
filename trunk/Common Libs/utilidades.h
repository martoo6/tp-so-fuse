/*
 * utilidades.h
 *
 *  Created on: 23/10/2011
 *      Author: utn_so
 */

#ifndef UTILIDADES_H_
#define UTILIDADES_H_

#include <semaphore.h>
#include <stdarg.h> //PARA ARGUMENTOS VARIABLES YAY !
#include "../C-Talks Libs/linux/common-libs/src/lib/collections/list.h"

char collection_any_satisfy( t_list* list, char (*closure)(void*, void*),void *arg);
char collection_any_satisfy2( t_list* list, char (*closure)(void*, ...),...);
char collection_all_satisfy( t_list* list, char (*closure)(void*, void*),void *arg);
char collection_all_satisfy2( t_list* list, char (*closure)(void*, ...),...);
void *collection_filter( t_list* list, char (*closure)(void*, void*),void *arg);
void *collection_filter2( t_list* list, char (*closure)(void*, ...),...);
void *collection_map( t_list* list, void *(*closure)(void*, void*),void *arg);
void *collection_map2( t_list* list, void *(*closure)(void*, ...),...);


#endif /* UTILIDADES_H_ */
