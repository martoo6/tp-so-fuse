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
#include <stdbool.h>
#include "list.h"

bool collection_any_satisfy( t_list* list, char (*closure)(void*, void*),void *arg);
bool collection_any_satisfy2( t_list* list, char (*closure)(void*, ...),...);
bool collection_all_satisfy( t_list* list, char (*closure)(void*, void*),void *arg);
bool collection_all_satisfy2( t_list* list, char (*closure)(void*, ...),...);
void *collection_filter( t_list* list, char (*closure)(void*, void*),void *arg);
void *collection_filter2( t_list* list, char (*closure)(void*, ...),...);
void *collection_map( t_list* list, void *(*closure)(void*, void*),void *arg);
void *collection_map2( t_list* list, void *(*closure)(void*, ...),...);
void *collection_take( t_list* list, unsigned int n);
bool collection_list_removeByContent( t_list *list, void *data, void (*data_destroyer)(void*) );
bool collection_list_removeByPointer2( t_list *list, void *data, void (*data_destroyer)(void*) );

#endif /* UTILIDADES_H_ */
