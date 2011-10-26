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


/*
 * ___________________FUNCIONES_______________________
 */


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


char enCilindroMayorMenorIgual(void *sector,va_list args_list){
	CHS sectorReal,sectorRealActual;
	unsigned long numeroSector;
	memcpy(&numeroSector,sector,sizeof(unsigned long));
	sectorReal=getRealSector(numeroSector,1,100);
	sectorRealActual=getRealSector(va_arg(args_list,unsigned long),1,100);
	if(va_arg(args_list,char)){
		return(sectorReal.cilindro>=sectorRealActual.cilindro);
	}else{
		return(sectorReal.cilindro<=sectorRealActual.cilindro);
	}

}


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

/*
 * ___________________FIN FUNCIONES___________________
 */




void *sectorMasCercano(void *lista,unsigned long sectorActual, char flagSuboBajo){
	//Calcula el tiempo en llegar a cada sector y se queda con el que demore menos tiempo
	//Se podria usar la funcion any_satisfy creada con una funcion duraMenosQue y tiempoBusqueda
}

void sector_destroy(void *sectorAux){
	free(sectorAux);
}

void threadScan(void *threadarg){
	//searchType *my_data;
	//my_data = (searchType*)(threadarg);
	searchType *my_data = threadarg;
	sectType *pArchivo = my_data->pArchivo;
	t_list *listaPedidos=my_data->listaPedidos;

	char flagSuboBajo=0;//La funcion enCilindroMayorMenorIgual utiliza un flag para saber si la cabeza esta subiendo o bajando
	unsigned long sectorActual=140;//Por archivo de configuracion o random
	void *sectorBuscado;



	while(listaPedidos->elements_count!=0){
		//HACEN FALTA SEMAFOROS SI LAS FUNCIONES QUE MANEJAN LA LSITA YA LOS USAN ?
		t_list *newList=collection_filter2(listaPedidos,enCilindroMayorMenorIgual,sectorActual,flagSuboBajo);
		if(newList->elements_count!=0){
			sectorBuscado =sectorMasCercano(newList, sectorActual, flagSuboBajo);
		}else{
			sectorBuscado =sectorMasCercano(listaPedidos, sectorActual, flagSuboBajo);
			flagSuboBajo= (!flagSuboBajo);
		}
		if(sizeof(*sectorBuscado)==sizeof(sectorLectura)){
			sectorLectura *sLectura = sectorBuscado;
			//USO EN FORMA CORRECTA EL POSIX_MADVISE ?
			//posix_madvise(pArchivo+sLectura->numeroSector, 512, POSIX_MADV_SEQUENTIAL );
			sendViaSocket(pArchivo[sLectura->numeroSector]);
		}else{
			//Con el numero de sector obtengo sus datos y luego los escribo.
			sectorEscritura *sEscritura = sectorBuscado;
			pArchivo[sEscritura->numeroSector]=sEscritura->datos;
			msync(pArchivo+sEscritura->numeroSector,512,MS_SYNC);//Si falla es porq pArchivo+sEscritura->numeroSector tiene que se multiplo de el tamaño de pagina, podria syncronizar todo a lo caco tambien..=P
			//Avisar que se escribio el archivo
		}
		collection_list_removeByPointer(listaPedidos, sectorBuscado, sector_destroy );
		collection_list_destroy(newList,sector_destroy);//ELIMINA LISTA DEL FILTER
		sectorActual=sectorBuscado+1;//CUIDADO QUE NO VUELVE AL COMIENZO Y SALTA DE PISTA ! CAMBIAR!
	}
}




int main(void) {
	/*int archivo,i;
	long sector;
	sectType *pArchivo;
	void *lstSectores;
	searchType param;
	pthread_t searchThread;

	lstSectores=NULL;

	archivo=openFile(FILEPATH);
	pArchivo = mapFile(archivo, FILESIZE, PROT_READ);

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

	t_list *nLista=collection_list_create();
	int i,cant;
	{
		for(i=0;i<100;i++){
			sectorLectura *nSector=malloc(sizeof(sectorLectura));
			nSector->numeroSector=(long)rand()%20001;
			cant=collection_list_add(nLista,nSector);
			printf("Sector: %d\n",nSector->numeroSector);
		}
	}
	scanf("%d",&i);
	return 0;
}
