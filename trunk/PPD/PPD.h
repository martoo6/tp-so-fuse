#define T_READ 1
#define T_WRITE 0

#include "list.h"
/*
 * ____________VARIABLES Y ESTRUCTURAS________________
 */


typedef struct{
	char a[512];
}sectType;

typedef struct{
	int cilindro;
	int cabeza;
	int sector;
}CHS;

typedef struct{
	unsigned long numeroSector;
}sectorLectura;

typedef struct{
	unsigned long numeroSector;
	sectType datos;
}sectorEscritura;

typedef struct{
	sectType *pArchivo;
	t_list *listaPedidos;
}searchType;

/*
 * ________FIN VARIABLES Y ESTRUCTURAS________________
 */

/*
 * ___________________HEADERS_________________________
 */

long getSector(CHS dir,int cilinders,int heads,int sectors);

CHS getRealDir(long sector,int cilinders,int heads,int sectors);

int GetBitValue(int n, int bitPosition);

unsigned int valorEnBits(int n);

int openFile(char *path);

sectType* mapFile(int fileDescriptor, long fileSize, int mode);

long getLogicSector(CHS dir,int cilindros,int cabezas,int sectoresPorPista);

CHS getRealSector(long sector,int cabezas,int sectoresPorPista);

void createSearchThread(pthread_t *thread,searchType *param);

//void bringSector2(long sector, listaSectoresLectura **lstSector);

//void writeSector(long sector,sectType data, listaSectoresEscritura **lstSector);
/*
 *___________________FIN HEADERS_____________________
 */
