#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>

#include <unistd.h>

#include "PPD.h"







#define FILEPATH "/home/utn_so/fat32.disk" //CAMBIAR PATH !
#define SECTORSIZE (512)
#define CLUSTERSIZE  (SECTORSIZE*8) //8 sectores por cluster = 4Kb
#define CANTSECT  (1048576) //Cantidad de sectores a traer
#define FILESIZE (524288*1024) //Segun el ejemplo del TP


//SEMAFOROS !
pthread_mutex_t  bufferSem= PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t  bVacioSem= PTHREAD_MUTEX_INITIALIZER;

//___________________________________________________
//___________________FUNCIONES_______________________
//___________________________________________________



long getSector(CHS dir,int cilinders,int heads,int sectors){
	return ((dir.cilinder*heads*sectors)+(sectors*dir.head)+(dir.sector-1));
}

CHS getRealDir(long sector,int cilinders,int heads,int sectors){
	CHS result;
	result.cilinder=sector/(heads*sectors);
	result.head=(sector%(heads*sectors))/sectors;
	result.sector=((sector%(heads*sectors))%sectors)+1;//Con codificacion sin sector 0;
	return result;
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

//posix_madvise(0, size, POSIX_MADV_RANDOM );



void insertaPrimero(void *estructura, nodo **lista){
	nodo *newNode = malloc(sizeof(nodo));
	void *newStructure=malloc(sizeof(*estructura));
	//newNode->estructura=estructura; METODO ALTERNATIVO
	newNode->estructura=newStructure;
	newNode->sgte=*lista;
	*lista=newNode;
}

void insertaUltimo(void *estructura, nodo **lista){
	nodo *ptr;
	ptr=*lista;
	while(ptr->sgte!=NULL){
		ptr=ptr->sgte;
	}
	nodo *newNode = malloc(sizeof(nodo));
	//newNode->estructura=estructura; METODO ALTERNATIVO
	void *newStructure=malloc(sizeof(*estructura));
	newNode->estructura=newStructure;
	newNode->sgte=NULL;
	ptr->sgte=newNode;
}

void insertaUltimo2(void *estructura, nodo **finLista){
	nodo *ptr;
	ptr=*finLista;
	nodo *newNode = malloc(sizeof(nodo));
	//newNode->estructura=estructura; METODO ALTERNATIVO
	void *newStructure=malloc(sizeof(*estructura));
	newNode->estructura=newStructure;
	newNode->sgte=NULL;
	ptr->sgte=newNode;
}

void borraNodo(void *estructura, nodo **lista){
	nodo *ptr,*aux;
	void *ptr2;
	ptr=*lista;
	ptr2=(void *)*lista;
	if((ptr->estructura)!=(&estructura)){
		while((ptr!=NULL)&&(ptr->sgte->estructura!=&estructura)){
			ptr=ptr->sgte;
		}
	}else{
		*lista=ptr->sgte;
	}
	if(ptr->sgte==NULL){
		aux=ptr->sgte;
		ptr->sgte=aux->sgte;
		free(aux);
	}else{
		//No existe el nodo
	}
}

void *traerEstrucutraNodo(nodo nodo){
	return (nodo.estructura);//MIRAR ESTE
}

void traerSector(long sector, void **lista){
	sectorLectura *aux= (sectorLectura*)malloc(sizeof(sectorLectura));
	aux->sector=sector;
	insertaUltimo(&aux,*lista);
}

int tipoSector(void **estructura){
	return(sizeof(*estructura)==sizeof(sectorLectura));
}


void threadScan(void *threadarg){
	searchType *my_data;
	my_data = (searchType*)(threadarg);

	int flag=0;
	long sectActual=0;
	long n;
	sectType *buffer;

	sectorLectura *sectR;
	sectorEscritura *sectW;
	nodo *ptrSector;

	while(1){
		ptrSector=my_data->fstSector;
		if(ptrSector!=NULL){
			//SACAR SIMPRE EL SECTOR CON MEMCOPY
			if(tipoSector(ptrSector->estructura)==T_READ){
				sectR = (sectorLectura*)(ptrSector->estructura);
				if(sectR->sector==sectActual){
					pthread_mutex_lock(&bufferSem);
					*buffer=my_data->pArchivo[sectActual];
					(my_data->buffer)=buffer;
					pthread_mutex_unlock(&bufferSem);
					//Lo elimino de la lista
					borraNodo(ptrSector->estructura,&(my_data->fstSector));
					//generar funcion elimina nodo que retorne la estructura del sector eliminado
				}
			}else{
				sectW = (sectorEscritura*)(ptrSector->estructura);
				if(sectW->sector==sectActual){
					*buffer=my_data->pArchivo[n];
					my_data->buffer=buffer;
					//Lo elimino de la lista
					borraNodo(ptrSector->estructura,&(my_data->fstSector));
				}
			}
		}
//-------------------------------------------------------------------------------
		while(ptrSector!=NULL){
			if(sizeof(*ptrSector)==sizeof(listaSectoresLectura)){
				sectR = (listaSectoresLectura)(*ptrSector);
				if(sectR.sector==sectActual){
					*buffer=my_data->pArchivo[n];
					*(my_data->buffer)=buffer;
					//Lo elimino de la lista
					ptrSector
				}
			}else{
				sectW = (listaSectoresEscritura)(my_data->fstSector[n]);
				if(sectW.sector==sectActual){
					*buffer=my_data->pArchivo[n];
					my_data->buffer=buffer;
				}
			}

		ptrSector
		}


		if(sectActual==0 || sectActual==my_data->maxSector){
			flag=flag & 1;
		}else{
			if(flag==0){
				sectActual++;
			}else{
				sectActual--;
			}
		}
		sleep(1);
	}
}

void createSearchThread(pthread_t *thread,searchType *param){
	int result;
	result = pthread_create(&thread, NULL,(void *) &threadScan, &param);
	if (result){
		printf("ERROR; return code from pthread_create() is %d\n", *thread);
		exit(-1);
	}
}



/*void bringSector2(long sector, void **lstSector){
	//Se genera una pila de sectores a buscar
	listaSectoresLectura* newSector = (listaSectoresLectura*)malloc(sizeof(listaSectoresLectura));
	newSector->sector=sector;
	newSector->nextSector=NULL;
	lstSector=&newSector;
}

void writeSector(long sector,sectType data, void **lstSector){
	listaSectoresEscritura* newSector = (listaSectoresEscritura*)malloc(sizeof(listaSectoresEscritura));
	newSector->sector=sector;
	newSector->data=data;
	newSector->nextSector=NULL;
	lstSector=&newSector;
}*/

//___________________________________________________
//___________________FIN FUNCIONES___________________
//___________________________________________________



int main(void) {
	int archivo;
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
	close(archivo);
	return 0;
}
























//___________________________________________________
//________________FUNCIONES VIEJAS___________________
//___________________________________________________

//sectType bringSector(int fileDescriptor ,long sector);
//void writeSector(int fileDescriptor ,long sector, sectType datos);
/*
sectType bringSector(int fileDescriptor ,long sector){
	sectType *pArchivo;
	sectType datos;
	//int npagina = (sector*SECTORSIZE)/getpagesize();
	int npagina = (sector/8);
	//int spagina = (8*(sector*SECTORSIZE)/getpagesize());
	int spagina= sector -(8*npagina);

	printf("Numero de Pagina: %d\n", npagina);
	printf("Sector dentro de la pagina: %d\n", spagina);
	pArchivo = mmap(0, getpagesize(), PROT_READ, MAP_SHARED, fileDescriptor, npagina*getpagesize());

	if (pArchivo == MAP_FAILED) {
		close(fileDescriptor);
		perror("Error mapeando el archivo");
		exit(EXIT_FAILURE);
	}
	datos=pArchivo[spagina];
	if (munmap(pArchivo,getpagesize()) == -1) {
		perror("Error desmapeando el archivo");
	}
	return datos;
}

void writeSector(int fileDescriptor ,long sector, sectType datos){
	sectType *pArchivo;
	int npagina = (sector/8);
	int spagina= sector -(8*npagina);

	printf("Numero de Pagina: %d\n", npagina);
	printf("Sector dentro de la pagina: %d\n", spagina);
	pArchivo = mmap(0, getpagesize(), PROT_WRITE, MAP_SHARED, fileDescriptor, npagina*getpagesize());

	if (pArchivo == MAP_FAILED) {
		close(fileDescriptor);
		perror("Error mapeando el archivo");
		exit(EXIT_FAILURE);
	}
	pArchivo[spagina]=datos;
	if (munmap(pArchivo,getpagesize()) == -1) {
		perror("Error desmapeando el archivo");
	}
}
void asignarPtrProxNodo(void *ptrSector, void *ptrProxNodo){
	listaSectoresLectura *sectR;
	listaSectoresEscritura *sectW;
	if(sizeof(*ptrSector)==sizeof(listaSectoresLectura)){
		sectR = (listaSectoresLectura *)(ptrSector);
		sectR.nextSector = ptrProxNodo;
	}else{
		sectW = (listaSectoresEscritura *)(ptrSector);
		sectW.nextSector = ptrProxNodo;
	}
}

/*long traerSectorNodo(void **ptrSector){
	listaSectoresLectura sectR;
	listaSectoresEscritura sectW;
	if(sizeof(*ptrSector)==sizeof(listaSectoresLectura)){
		sectR = (listaSectoresLectura)(*ptrSector);
		return sectR.sector;
	}else{
		sectW = (listaSectoresEscritura)(*ptrSector);
		return sectW.sector;
	}
}
void *traerPtrProxNodo(void *ptrSector){
	listaSectoresLectura sectR;
	listaSectoresEscritura sectW;
	if(sizeof(*ptrSector)==sizeof(listaSectoresLectura)){
		sectR = (listaSectoresLectura)(*ptrSector);
		return sectR.nextSector;
	}else{
		sectW = (listaSectoresEscritura)(*ptrSector);
		return sectW.nextSector;
	}
}

void asignarPtrProxNodo(void **ptrSector){
	listaSectoresLectura *sectR;
	listaSectoresEscritura *sectW;
	if(sizeof(*ptrSector)==sizeof(listaSectoresLectura)){
		sectR = (listaSectoresLectura *)(ptrSector);
		sectR.nextSector = traerPtrProxNodo(ptrSector);
	}else{
		sectW = (listaSectoresEscritura *)(ptrSector);
		*sectW.nextSector = traerPtrProxNodo(ptrSector);
	}
}

void eliminarNodo(long sectorBuscado,void **fstSector){
	void* ptrSector;
	void* ptrAux;

	//Verifica si es el primero
	if(traerSectorNodo(&fstSector)==sectorBuscado){
		ptrAux=fstSector;
		*fstSector=traerPtrProxNodo(&fstSector);
		free(ptrAux);
	}else{
		//Si no es el primero continua con el resto
		ptrSector=fstSector;
		while(ptrSector!=NULL){
			if(traerSectorNodo(traerPtrProxNodo(&ptrSector))==sectorBuscado){
				ptrAux=traerPtrProxNodo(&ptrSector);
				asignarPtrProxNodo(&ptrSector);
				free(ptrAux);
				ptrSector=NULL;
			}else{
				asignarPtrProxNodo(&ptrSector);
			}
		}
	}
}


int GetBitValue( int n, int bitPosition ) {
   return ((n >> bitPosition) & 1);
}

unsigned int valorEnBits(int n){
	int i;
	unsigned int result=0;
	for(i=0;i<8;i++){
		result+=GetBitValue(n,i);
		result*=10;
	}
	result/=10;
	return result;
}

*/
