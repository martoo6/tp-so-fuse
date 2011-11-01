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

#include <math.h>

#include "PPD.h"
#include "list.c"
#include "utilidades.h"




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

unsigned int tiempo_entre_sectores(unsigned long numeroSector1,unsigned long numeroSector2){
	/*RPM, tiempoPorCilindro (txc) y SectoresPorPista (sxp) debe ser proporcionado por el archivo de configuración*/
	//No esta teniendo en cuenta el tiempo de busqueda hacia arriva o abajo

	/*A modo de PRUEBA:*/
	int RPM=6000;
	int tiempoPorCilindro=20;
	int SectoresPorPista=10;
	/*A modo de PRUEBA:*/

	CHS dirFisica1 = getRealSector(numeroSector1,1,SectoresPorPista);
	CHS dirFisica2 = getRealSector(numeroSector2,1,SectoresPorPista);

	float tiempoPorSector=60000/RPM; /*tiempoPorSector*/
	float tiempoEntreCilindrosTotal=tiempoPorCilindro*fabs(dirFisica2.cilindro-dirFisica1.cilindro);//tiempoEntreCilindrosTotal
	float vs=dirFisica2.sector-((dirFisica1.sector+(int)(tiempoEntreCilindrosTotal/tiempoPorSector))%SectoresPorPista);//Variación de Sectores
	float tt=tiempoPorSector + (SectoresPorPista/2 + vs*(1 - SectoresPorPista/(2*(fabs(vs)))));

	return tt;
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




void createSearchThread(pthread_t *id,searchType *param){
	int result;
	if(param->nPasos==0){
		result = pthread_create(&id, NULL,threadScan, &param);
	}else{
		result = pthread_create(&id, NULL,threadScanNPasos, &param);
	}
	if (result){
		printf("ERROR; return code from pthread_create() is %d\n", *id);
		exit(-1);
	}
}

void *sectorMasCercano(t_list *list,unsigned long sectorActual, char flagSuboBajo){
	//Calcula el tiempo en llegar a cada sector y se queda con el que demore menos tiempo

	t_link_element *elem = list->head;
	unsigned long numeroSector;
	unsigned int ultimoTiempo=65535;//Maximo Numero de Unsigned Int
	void *masCercano;

	while(elem!=NULL){
		memcpy(&numeroSector,elem->data,sizeof(unsigned long));
		if(tiempo_entre_sectores(numeroSector,sectorActual)<ultimoTiempo){
			masCercano = elem->data;
		}
		elem=elem->next;
	}
	return masCercano;
}

void sector_destroy(void *sectorAux){
	free(sectorAux);
}


/*
 * ___________________FIN FUNCIONES___________________
 */

void threadConsola(void *threadarg){

}

void threadScanNPasos(void *threadarg){

}
void threadScan(void *threadarg){
	//searchType *my_data;
	//my_data = (searchType*)(threadarg);
	searchType *my_data = threadarg;
	sectType *pArchivo = my_data->pArchivo;
	t_list *listaPedidos=my_data->listaPedidos;
	t_list *listaPedidosAtender = listaPedidos;
	bool ultimoSector=false;

	char flagSuboBajo=0;//La funcion enCilindroMayorMenorIgual utiliza un flag para saber si la cabeza esta subiendo o bajando
	unsigned long sectorActual=140;//Por archivo de configuracion o random
	void *sectorBuscado;

	while(1){
		if(my_data->nPasos!=0){
			listaPedidosAtender = collection_take(listaPedidos,my_data->nPasos);
		}

		while(listaPedidosAtender->elements_count!=0){
			//HACEN FALTA SEMAFOROS SI LAS FUNCIONES QUE MANEJAN LA LSITA YA LOS USAN ?
			t_list *newList=collection_filter2(listaPedidos,enCilindroMayorMenorIgual,sectorActual,flagSuboBajo);


			if(newList->elements_count!=0){
				sectorBuscado =sectorMasCercano(newList, sectorActual, flagSuboBajo);
			}else{
				if(ultimoSector){
					sectorBuscado =sectorMasCercano(listaPedidos, sectorActual, flagSuboBajo);
					flagSuboBajo= (!flagSuboBajo);
				}else{
					sectorBuscado = traerUltimoSector(flagSuboBajo);
					ultimoSector=true;
				}

			}
			if(sizeof(*sectorBuscado)==sizeof(sectorLectura)){
				sectorLectura *sLectura = sectorBuscado;
				if(ultimoSector){
					//ESTOY LEYENDO EL ULTIMO SECTOR SIN PROCESARLO !
					ultimoSector=false;
				}else{
					//USO EN FORMA CORRECTA EL POSIX_MADVISE ?
					//posix_madvise(pArchivo+sLectura->numeroSector, 512, POSIX_MADV_SEQUENTIAL );
					//sendViaSocket(pArchivo[sLectura->numeroSector]);
					printf("Sector:%d Data:%d\n",sLectura->numeroSector,pArchivo[sLectura->numeroSector]);
				}
			}else{
				//Con el numero de sector obtengo sus datos y luego los escribo.
				sectorEscritura *sEscritura = sectorBuscado;
				pArchivo[sEscritura->numeroSector]=sEscritura->datos;
				msync(pArchivo+sEscritura->numeroSector,512,MS_SYNC);//Si falla es porq pArchivo+sEscritura->numeroSector tiene que se multiplo de el tamaño de pagina, podria syncronizar todo a lo caco tambien..=P
				//Avisar que se escribio el archivo
			}
			collection_list_removeByPointer(listaPedidos, sectorBuscado, sector_destroy );
			collection_list_removeByPointer(listaPedidosAtender, sectorBuscado, sector_destroy );
			collection_list_destroy(newList,sector_destroy);//ELIMINA LISTA DEL FILTER
			sectorActual=sectorBuscado+1;//CUIDADO QUE NO VUELVE AL COMIENZO Y SALTA DE PISTA ! CAMBIAR!
		}
	}
}




int main(void) {
	int archivo,i;
	long cant;
	sectType *pArchivo;
	searchType param;
	pthread_t *(searchThread);
	pid_t idConsola;


	idConsola=fork();

	if(idConsola){
		//Fork generó una copia, reemplazo la copia (YO) por el proceso PPD-Consola con execv
		int error;
		error=execv("/home/utn_so/Desarrollo/Workspace/PPD-Consola/Debug/PPD-Consola",NULL);
		if(error==-1)perror("Error en cargado de proceso Consola");
	}else{
		archivo=openFile(FILEPATH);
		pArchivo = mapFile(archivo, FILESIZE, PROT_READ);


		//Simulacion de carga de sectores
		t_list *nLista=collection_list_create();


		for(i=0;i<20;i++){
			sectorLectura *nSector=malloc(sizeof(sectorLectura));
			nSector->numeroSector=(long)rand()%20001;
			cant=collection_list_add(nLista,nSector);
			printf("Sector: %d\n",nSector->numeroSector);
		}

		//Parametros para el Thread !
		param.pArchivo=pArchivo;
		param.listaPedidos=nLista;

		createSearchThread(searchThread,&param);
		pthread_create(&idConsola, NULL,threadConsola, NULL);

		if (munmap(pArchivo,FILESIZE) == -1) {
			perror("Error desmapeando el archivo");
		}
		close(archivo);

		scanf("%d",&i);
	}

	return 0;
}
