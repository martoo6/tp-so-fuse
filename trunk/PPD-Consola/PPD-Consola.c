/*
 * PPD-Consola.c
 *
 *  Created on: 26/10/2011
 *      Author: Martin Silberkasten
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>
#include <stdarg.h>

#include "list.c"
#include "array.c"
#include "scanner.c"
#include "commands.c"

void funcInfo(void *nada, t_array *nada2){
	printf("Posicion actual: \n");
}
void funcClean(void *nada, t_array *clusters){
	//NO FUNCIONA ARRAY_GET y la "!#$%$&/%$/
	int *clusterInicial, *clusterFinal;
	clusterInicial = array_get(clusters,1);
	clusterFinal = array_get(clusters,2);
	printf("Cluster inicial: %d\n",clusters->data[0]);
	printf("Cluster final: %d\n",clusters->data[1]);
}
void funcTrace(void *nada, t_array *nada2){
	printf("Posicion actual:");
}

int main(int argc,char *argv[]){
	t_commands *comandos = commands_create("-", "", " ");
	commands_add(comandos, "info",funcInfo);
	commands_add(comandos, "clean",funcClean);
	commands_add(comandos, "trace",funcTrace);

	char input[20];
	while(1){
		scanf("%s",input);
		commands_parser(comandos,input);
		sleep(1);
	}
	return 0;
}
