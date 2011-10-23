#include <stdlib.h>
#include <semaphore.h>
#include "utilidades.h"

/*
 * @NAME: collection_list_iterator
 * @DESC: Itera la lista llamando al closure por cada elemento
 */
char any_satisfy( t_list* list, char (*closure)(void*, void*),void *arg){
	t_link_element *element = list->head;
	sem_wait( &list->semaforo );
	while( element != NULL ){
		if(closure(element->data, arg)){
			return(1);
		}
		element = element->next;
	}
	sem_post( &list->semaforo );
	return(0);
}

char all_satisfy( t_list* list, char (*closure)(void*, void*),void *arg){
	t_link_element *element = list->head;
	sem_wait( &list->semaforo );
	while( element != NULL ){
		if(closure(element->data, arg)!=1){
			return(0);
		}
		element = element->next;
	}
	sem_post( &list->semaforo );
	return(1);
}
