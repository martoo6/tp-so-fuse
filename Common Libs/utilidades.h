/*
 * utilidades.h
 *
 *  Created on: 23/10/2011
 *      Author: utn_so
 */

#ifndef UTILIDADES_H_
#define UTILIDADES_H_

#include <semaphore.h>
#include "../C-Talks Libs/linux/common-libs/src/lib/collections/list.h"

char collection_any_satisfy( t_list* list, char (*closure)(void*, void*),void *arg);
char collection_all_satisfy( t_list* list, char (*closure)(void*, void*),void *arg);
void *collection_filter( t_list* list, char (*closure)(void*, void*),void *arg);
void *collection_map( t_list* list, void *(*closure)(void*, void*),void *arg);


#endif /* UTILIDADES_H_ */
