#include <stdlib.h>
#include <semaphore.h>
#include "utilidades.h"
//#include "../PPD/PPD.h"

/*
 * @NAME: collection_any_satisfy
 * @DESC: Igual a funcion any_satisfy de Haskell
 */
char collection_any_satisfy( t_list* list, char (*closure)(void*, void*),void *arg){
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
/*
 * @NAME: collection_all_satisfy
 * @DESC: Igual a funcion all_satisfy de Haskell
 */
char collection_all_satisfy( t_list* list, char (*closure)(void*, void*),void *arg){
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
/*
 * @NAME: collection_filter
 * @DESC: Igual a la funcion filter de haskell
 */
void *collection_filter( t_list* list, char (*closure)(void*, void*),void *arg){
	t_list *listaAux=collection_list_create();
	t_link_element *element = list->head;
	sem_wait( &list->semaforo );
	while( element != NULL ){
		if(closure(element->data, arg)){
			void *newData=malloc(sizeof(element->data));
			newData=element->data;
			collection_list_add(listaAux,newData);
		}
		element = element->next;
	}
	sem_post( &list->semaforo );
	return(listaAux);
}
/*
 * @NAME: collection_map
 * @DESC: Igual a la funcion map de haskell
 */
void *collection_map( t_list* list, void *(*closure)(void*, void*),void *arg){
	t_list *listaAux=collection_list_create();
	t_link_element *element = list->head;
	sem_wait( &list->semaforo );
	while( element != NULL ){
		void *newData=malloc(sizeof(*closure));
		newData=closure(element->data, arg);
		collection_list_add(listaAux,newData);
		element = element->next;
	}
	sem_post( &list->semaforo );
	return(listaAux);
}
