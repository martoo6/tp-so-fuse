#include <stdlib.h>
#include <semaphore.h>
#include <stdbool.h>
#include "utilidades.h"

//#include "../PPD/PPD.h"

/*
 * @NAME: collection_any_satisfy
 * @DESC: Igual a funcion any_satisfy de Haskell
 */
bool collection_any_satisfy( t_list* list, char (*closure)(void*, void*),void *arg){
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
bool collection_all_satisfy( t_list* list, char (*closure)(void*, void*),void *arg){
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
/*
 * @NAME: collection_any_satisfy2
 * @DESC: Igual a funcion any_satisfy de Haskell
 */
bool collection_any_satisfy2( t_list* list, char (*closure)(void*, ...),...){
	va_list args_list;
	va_start(args_list,*closure);
	t_link_element *element = list->head;
	sem_wait( &list->semaforo );
	while( element != NULL ){
		if(closure(element->data, args_list)){
			return(1);
		}
		element = element->next;
	}
	sem_post( &list->semaforo );
	return(0);
}
/*
 * @NAME: collection_all_satisfy2
 * @DESC: Igual a funcion all_satisfy de Haskell
 */
bool collection_all_satisfy2( t_list* list, char (*closure)(void*, ...),...){
	va_list args_list;
	va_start(args_list,*closure);
	t_link_element *element = list->head;
	sem_wait( &list->semaforo );
	while( element != NULL ){
		if(closure(element->data, args_list)!=1){
			return(0);
		}
		element = element->next;
	}
	sem_post( &list->semaforo );
	return(1);
}
/*
 * @NAME: collection_filter2
 * @DESC: Igual a la funcion filter de haskell con argumentos variables
 */
void *collection_filter2( t_list* list, char (*closure)(void*, ...),...){
	va_list args_list;
	va_start(args_list,*closure);
	t_list *listaAux=collection_list_create();
	t_link_element *element = list->head;
	sem_wait( &list->semaforo );
	while( element != NULL ){
		if(closure(element->data, args_list)){
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
 * @NAME: collection_map2
 * @DESC: Igual a la funcion map de haskell pero con argumentos variables
 */
void *collection_map2( t_list* list, void *(*closure)(void*, ...),...){
	va_list args_list;
	va_start(args_list,*closure);
	t_list *listaAux=collection_list_create();
	t_link_element *element = list->head;
	sem_wait( &list->semaforo );
	while( element != NULL ){
		void *newData=malloc(sizeof(*closure));
		newData=closure(element->data, args_list);
		collection_list_add(listaAux,newData);
		element = element->next;
	}
	sem_post( &list->semaforo );
	return(listaAux);
}
/*
 * @NAME: collection_take
 * @DESC: Toma n elementos
 */
void *collection_take( t_list* list, unsigned int n){
	t_list *listaAux=collection_list_create();
	t_link_element *element = list->head;
	unsigned int i=0;
	sem_wait( &list->semaforo );
	while(( element != NULL) && (i<n)){
		void *newData=malloc(sizeof(*element));
		newData=element->data;
		collection_list_add(listaAux,newData);
		element = element->next;
		i++;
	}
	sem_post( &list->semaforo );
	return(listaAux);
}
