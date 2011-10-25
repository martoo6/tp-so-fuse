#define T_READ 1
#define T_WRITE 0

//___________________________________________________
//____________VARIABLES Y ESTRUCTURAS________________
//___________________________________________________

typedef struct{
	char a[512];
}sectType;

typedef struct{
	int cilindro;
	int cabeza;
	int sector;
}CHS;

typedef struct node{
	void *estructura;
	struct node *sgte;
}nodo;

typedef struct{
	unsigned long numeroSector;
}sectorLectura;

typedef struct{
	unsigned long numeroSector;
	sectType datos;
}sectorEscritura;

typedef struct{
	long maxSector;
	sectType *buffer;
	sectType *pArchivo;
	nodo *lstSector;
	nodo *fstSector;
}searchType;

//___________________________________________________
//________FIN VARIABLES Y ESTRUCTURAS________________
//___________________________________________________



//___________________________________________________
//___________________HEADERS_________________________
//___________________________________________________

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

//___________________________________________________
//___________________FIN HEADERS_____________________
//___________________________________________________
