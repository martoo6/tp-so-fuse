#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>

#include <stdarg.h> //PARA ARGUMENTOS VARIABLES YAY !

#include "PPD.h"
#include "list.c"
#include "utilidades.c"






#define FILEPATH "/home/utn_so/fat32.disk" //CAMBIAR PATH !
#define SECTORSIZE (512)
#define CLUSTERSIZE  (SECTORSIZE*8) //8 sectores por cluster = 4Kb
#define CANTSECT  (1048576) //Cantidad de sectores a traer
#define FILESIZE (524288*1024) //Segun el ejemplo del TP


//___________________________________________________
//___________________FUNCIONES_______________________
//___________________________________________________




long getLogicSector(CHS dir,int cilindros,int cabezas,int sectoresPorPista){
	return ((dir.cilindro*cabezas*sectoresPorPista)+(sectoresPorPista*dir.cabeza)+(dir.sector-1));
}

CHS getRealSector(long sector,int cabezas,int sectoresPorPista){
	//DEBERIA DE LEER EL ARCHIVO DE CONFIGURACION !
	cabezas=1;
	sectoresPorPista=100;
	CHS result;
	result.cilindro=sector/(cabezas*sectoresPorPista);
	result.cabeza=(sector%(cabezas*sectoresPorPista))/sectoresPorPista;
	result.sector=((sector%(cabezas*sectoresPorPista))%sectoresPorPista)+1;//Con codificacion sin sector 0;
	return result;
}

char enCilindroMayorIgual(void *sector,unsigned long sectorActual){
	CHS sectorReal,sectorRealActual;
	unsigned long numeroSector;
	memcpy(&numeroSector,sector,sizeof(unsigned long));
	sectorReal=getRealSector(numeroSector,1,100);
	sectorRealActual=getRealSector(sectorActual,1,100);
	return(sectorReal.cilindro>=sectorRealActual.cilindro);
}

char enCilindroMayorIgual2(void *sector,va_list args_list){
	CHS sectorReal,sectorRealActual;
	unsigned long numeroSector;
	memcpy(&numeroSector,sector,sizeof(unsigned long));
	sectorReal=getRealSector(numeroSector,1,100);
	sectorRealActual=getRealSector(va_arg(args_list,unsigned long),1,100);
	return(sectorReal.cilindro>=sectorRealActual.cilindro);
}
/*

int openFile(char *path){
	int fileDescriptor;
	fileDescriptor = open(path, O_RDWR);
	if (fileDescriptor == -1) {
		perror("Error abriendo el archivo para lectura");
		exit(EXIT_FAILURE);
	}
	return fileDescriptor;
}

sectType* mapFile(int fileDescriptor, long fileSize, int mode){
	sectType *pArchivo;
	//pArchivo = mmap(0, FILESIZE, PROT_READ, MAP_SHARED, fileDescriptor, 0);
	pArchivo = mmap(0, fileSize, mode, MAP_SHARED, fileDescriptor, 0);
	if (pArchivo == MAP_FAILED) {
		close(fileDescriptor);
		perror("Error mapeando el archivo");
		exit(EXIT_FAILURE);
	}
	return pArchivo;
}

//posix_madvise(0, size, POSIX_MADV_RANDOM );





void traerSector(long sector, void **lista){
	sectorLectura *aux= (sectorLectura*)malloc(sizeof(sectorLectura));
	aux->sector=sector;
	insertaUltimo(&aux,*lista);
}

int tipoSector(void **estructura){
	return(sizeof(*estructura)==sizeof(sectorLectura));
}




void createSearchThread(pthread_t *thread,searchType *param){
	int result;
	result = pthread_create(&thread, NULL,(void *) &threadScan, &param);
	if (result){
		printf("ERROR; return code from pthread_create() is %d\n", *thread);
		exit(-1);
	}
}


//___________________________________________________
//___________________FIN FUNCIONES___________________
//___________________________________________________


*/

void *sectorMasCercano(void *lista,unsigned long sectorActual){
	//Calcula el tiempo en llegar a cada sector y se queda con el que demore menos tiempo
	//Se podria usar la funcion any_satisfy creada con una funcion duraMenosQue y tiempoBusqueda
}

void sector_destroy(void *sectorAux){
	free(sectorAux);
}

void threadScan(void *threadarg){
	searchType *my_data;
	my_data = (searchType*)(threadarg);

	unsigned long sectorActual=140;//Por archivo de configuracion o random
	void *sectorBuscado;
	t_list *listaPedidos=collection_list_create();
	while(listaPedidos->elements_count!=0){
		//SEMAFORO !!
		//La funcion enCilindroMayorIgual2 deberia poder utilizar un flag apra que se fije tanto para arriva como para abajo.
		//y al calcular el tiempo que tambien vea el mismo flag.
		t_list *newList=collection_filter2(listaPedidos,enCilindroMayorIgual2,sectorActual);
		if(newList->elements_count!=0){
			sectorBuscado =sectorMasCercano(newList, sectorActual);
		}else{
			sectorBuscado =sectorMasCercano(listaPedidos, sectorActual);
		}
		//SE FIJA SI ES DE LECTURA O DE ESCRITURA PARA VER QUE OPERACION HACER
		if(sizeof(*sectorBuscado)==sizeof(sectorLectura)){
			//Ya tengo el numero de sector solo tengo q obtener sus datos y enviarlos
			sectorLectura sLectura = (sectorLectura)(*sectorBuscado);
			//enviar();
		}else{
			//Con el numero de sector obtengo sus datos y luego los escribo.
			sectorEscritura sEscritura = (sectorEscritura)(*sectorBuscado);
			//escribir(sectorBuscado.,);
		}
		//--HACER-- ELIMINAR EL NODO DE LA LISTA DE PEDIDOS
		collection_list_destroy(newList,sector_destroy);//ELINA LISTA DEL FILTER
		sectorActual=sectorBuscado+1;//CUIDADO QUE NO VUELVE AL COMIENZO Y SALTA DE PISTA ! CAMBIAR!
	}
}




int main(void) {
	/*int archivo;
	int i,printThread;
	long sector;
	sectType *pArchivo;
	sectType buffer;
	void *lstSectores;
	searchType param;
	pthread_t searchThread;

	lstSectores=NULL;

	archivo=openFile(FILEPATH);
	pArchivo = mapFile(archivo, FILESIZE, PROT_READ);

	param.buffer=&buffer;
	param.maxSector=(FILESIZE/512);
	param.pArchivo=pArchivo;
	param.sectorList=lstSectores;

	createSearchThread(&searchThread,&param);
	//createPrintThread(&printThread);

	//buffer = bringSector(archivo,sector);

	while(sector!=((long)("\0"))){
		printf("Ingrese sector a buscar\n");
		scanf("%d",&sector);
		bringSector2(sector,lstSectores);
		for(i=0;i<10000;i++){
			printf("%d\n",buffer);
		}

		//writeSector(archivo,sector,buffer);
		//buffer = bringSector(archivo,sector);
		//printf("%d",buffer);
	}
	printf("\nFin");

	if (munmap(pArchivo,FILESIZE) == -1) {
		perror("Error desmapeando el archivo");
	}
	close(archivo);*/
	void printLoQueVenga(long sector){
			printf("%d\n",sector);
		}
	void printSector(sectorLectura *sector){
		printf("%d\n",sector->numeroSector);
	}
	long multiplicarPor(sectorLectura *sector,int a){
		return(sector->numeroSector*a);
	}
	char mayorA(sectorLectura *data,int b){
		return(data->numeroSector>b);
	}

	t_list *nLista=collection_list_create();
	int i,cant;
	{
		for(i=0;i<100;i++){
			sectorLectura *nSector=malloc(sizeof(sectType));
			nSector->numeroSector=(long)rand()%201;
			cant=collection_list_add(nLista,nSector);
			printf("Sector: %d\n",nSector->numeroSector);
		}
	}
	/*{
		unsigned long a=60;
		if(collection_any_satisfy(nLista,enCilindroMayorIgual,a)){
			printf("Hay mayores a %d\n",a);
		}else{
			printf("No hay mayores a %d\n",a);
		}
	}*/
	{
		scanf("%d",&i);
		int sectorActual=101;
		//t_list *newList=collection_filter(nLista,mayorA,b);
		//t_list *newList2=collection_map(nLista,enCilindroMayorIgual,b);
		//t_list *newList2=collection_filter(nLista,enCilindroMayorIgual,sectorActual);
		t_list *newList3=collection_filter2(nLista,enCilindroMayorIgual2,sectorActual);
		//collection_list_iterator(newList,printSector);
		collection_list_iterator(newList3,printSector);
		//collection_list_destroy(newList,sectorLectura_destroy);
		collection_list_destroy(newList3,sector_destroy);

	}
	return 0;
}
