#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "map.h"

void usage2(char * commande);

int setCommande(char * commande){
	int argI = -1;
	if(!strcmp(commande, "--getwidth"))
		argI=0;
	if(!strcmp(commande, "--getheight"))
		argI=1;
	if(!strcmp(commande, "--getobjects"))
		argI=2;
	if(!strcmp(commande, "--getinfo"))
		argI=3;
	if(!strcmp(commande, "--setwidth"))
		argI=4;
	if(!strcmp(commande, "--setheight"))
		argI=5;
	if(!strcmp(commande, "--setobjects"))
		argI=6;
	if(!strcmp(commande, "--pruneobjects"))
		argI=7;
	return argI;
}

void getInfos(int argI, char * file){
	int mapFile = open(file, O_RDONLY, 0666);
	int x;
	if(mapFile==-1)
		usage2("Le fichier ne s'est pas ouvert correctement.");
	int k = 1;
	if(argI==1)
		lseek(mapFile, sizeof(int), SEEK_SET);
	if(argI==2)
		lseek(mapFile, 2*sizeof(int), SEEK_SET);
	if(argI==3)
		k=3;
	for(int i=0 ; i<k ; i++){
		read(mapFile, &x, sizeof(int));
		printf("%d\n", x);
	}
}

void setWidthHeight(char * argument, int mapFile, int fdTmp, int argI){
	int arg = atoi(argument);
	int whn, x = 0, y, n, changeOfMap, nbbytes = 1;
	if(!arg)
		usage2("Quatrieme argument invalide (requiert un entier).");
	if(argI==4 && (arg<MIN_WIDTH||arg>MAX_WIDTH))
		usage2("Largeur non valide.");
	if(argI==5 && (arg<MIN_HEIGHT||arg>MAX_HEIGHT))
		usage2("Hauteur non valide.");
	for(int i=0 ; i<3 ; i++){
		read(mapFile, &whn, sizeof(int));
		if((argI==4 && i==0) || (argI==5 && i==1)){
			write(fdTmp, &arg, sizeof(int));
			if(argI==5)
			changeOfMap = arg-whn;
		}else
			write(fdTmp, &whn, sizeof(int));
	}

	while(x!=-1){
		read(mapFile, &x, sizeof(int));
		if(x!=-1){
			read(mapFile, &y, sizeof(int));
			read(mapFile, &n, sizeof(int));
			if(argI==4 && x<arg){
				write(fdTmp, &x, sizeof(int));
				write(fdTmp, &y, sizeof(int));
				write(fdTmp, &n, sizeof(int));
			}else if(argI==5){
				y+=changeOfMap;
				if(y<arg && y>=0){
					write(fdTmp, &x, sizeof(int));
					write(fdTmp, &y, sizeof(int));
					write(fdTmp, &n, sizeof(int));
				}
			}
		}
	}
	write(fdTmp, &x, sizeof(int));
	while(nbbytes!=0){
		nbbytes = read(mapFile, &x, sizeof(int));
		write(fdTmp, &x, nbbytes);
	}
}

void setObjects(int nbArg, char ** arguments, int mapFile, int fdTmp){
	if(((nbArg-3)%6))
		usage2("Nombre d'arguments de la liste invalide (6 demandes).");
	int x=0, nbObj, length, frame, solidity, destructible, collectible, generator;
	char * name = NULL;
	char * objectArg = NULL;

	while(x!=-1){
		read(mapFile, &x, sizeof(int));
		write(fdTmp, &x, sizeof(int));
	}

	for(int i=3 ; i<nbArg ; i+=6){
		length = strlen(arguments[i]);
		name = realloc(name, length*sizeof(char));
		strcpy(name, arguments[i]);
		if(atoi(arguments[i+1])<1)
			usage2("Nombre de frames invalides.");
		else
			frame = atoi(arguments[i+1]);

		objectArg = realloc(objectArg, strlen(arguments[i+2])*sizeof(char));
		strcpy(objectArg, arguments[i+2]);
		if(!strcmp(objectArg,"solid"))
			solidity = MAP_OBJECT_SOLID;
		else if(!strcmp(objectArg,"semi-solid"))
			solidity = MAP_OBJECT_SEMI_SOLID;
		else if(!strcmp(objectArg,"air"))
			solidity = MAP_OBJECT_AIR;
		else
			usage2("Valeur de solidite : solid | semi-solid | air.");

		objectArg = realloc(objectArg, strlen(arguments[i+3])*sizeof(char));
		strcpy(objectArg, arguments[i+3]);
		if(!strcmp(objectArg,"destructible"))
			destructible = 1;
		else if(!strcmp(objectArg,"not-destructible"))
			destructible = 0;
		else
			usage2("Valeur de destructible : destructible | not-destructible.");

		objectArg = realloc(objectArg, strlen(arguments[i+4])*sizeof(char));
		strcpy(objectArg, arguments[i+4]);
		if(!strcmp(objectArg,"collectible"))
			collectible = 1;
		else if(!strcmp(objectArg,"not-collectible"))
			collectible = 0;
		else
			usage2("Valeur de collectible : collectible | not-collectible.");

		objectArg = realloc(objectArg, strlen(arguments[i+5])*sizeof(char));
		strcpy(objectArg, arguments[i+5]);
		if(!strcmp(objectArg,"generator"))
			generator = 1;
		else if(!strcmp(objectArg,"not-generator"))
			generator = 0;
		else
			usage2("Valeur de generator : generator | not-generator.");
		write(fdTmp, &length, sizeof(int));
		write(fdTmp, name, length*sizeof(char));
		write(fdTmp, &frame, sizeof(int));
		write(fdTmp, &solidity, sizeof(int));
		write(fdTmp, &destructible, sizeof(int));
		write(fdTmp, &collectible, sizeof(int));
		write(fdTmp, &generator, sizeof(int));
	}
	free(name);
	free(objectArg);

	lseek(fdTmp, 2*sizeof(int), SEEK_SET);
	nbObj = (nbArg-3)/6;
	write(fdTmp, &nbObj, sizeof(int));

	lseek(mapFile, 2*sizeof(int), SEEK_SET);
	read(mapFile, &x, sizeof(int));
	if(x>nbObj){
		nbObj--;
		while(x!=-1){
			read(mapFile, &x, sizeof(int));
			if(x!=-1){
				lseek(mapFile, sizeof(int), SEEK_CUR);
				read(mapFile, &x, sizeof(int));
				lseek(fdTmp, 2*sizeof(int), SEEK_CUR);
				if(x>nbObj)
					write(fdTmp, &nbObj, sizeof(int));
				else
					lseek(fdTmp, sizeof(int), SEEK_CUR);
			}
		}
	}
}

void pruneObjects(int mapFile, int fdTmp){
	int x=0, nbObj, changeOfObj=0, newNbObj=0;
	char * name = NULL;
	lseek(mapFile, 2*sizeof(int), SEEK_SET);
	read(mapFile, &nbObj, sizeof(int));
	int * used = malloc(nbObj*sizeof(int));
	for(int i=0 ; i<nbObj ; i++)
		used[i] = 0;
	while(x!=-1){
		read(mapFile, &x, sizeof(int));
		if(x!=-1){
			lseek(mapFile, sizeof(int), SEEK_CUR);
			read(mapFile, &x, sizeof(int));
			if(used[x]==0)
				used[x]=1;
		}
	}
	x=0;
	for(int i=0 ; i<nbObj ; i++)
		if(used[i]==0)
			changeOfObj=1;
	if(changeOfObj==1){
		lseek(mapFile, 0, SEEK_SET);
		while(x!=-1){
			read(mapFile, &x, sizeof(int));
			write(fdTmp, &x, sizeof(int));				
		}
		for(int i=0 ; i<nbObj ; i++){
			read(mapFile, &x, sizeof(int));
			name = realloc(name, (x+1)*sizeof(char));
			name[x]='\0';
			read(mapFile, name, x*sizeof(char));
			if(used[i]==1){
				write(fdTmp, &x, sizeof(int));
				write(fdTmp, name, x*sizeof(char));
				for(int j=0 ; j<5 ; j++){
					read(mapFile, &x, sizeof(int));
					write(fdTmp, &x, sizeof(int));
				}
				newNbObj++;
			}else
				for(int j=0 ; j<5 ; j++)
					read(mapFile, &x, sizeof(int));
		}
		lseek(fdTmp, 2*sizeof(int), SEEK_SET);
		write(fdTmp, &newNbObj, sizeof(int));
	}
	free(name);
}

void usage(char * commande){
	fprintf(stderr, "%s fichier commande (entier/listeCaracteristiquesObjets)\n", commande);
	exit(EXIT_FAILURE);
}

void usage2(char * commande){
	fprintf(stderr, "%s\n", commande);
	exit(EXIT_FAILURE);
}

int main(int argc, char ** argv){
	if(argc<3)
		usage(argv[0]);
	char * commande = argv[2];

	int argI = setCommande(commande);

	if(argI>=0 && argI<=3){
		getInfos(argI, argv[1]);
	}else if(argI>=4){
		int mapFile = open(argv[1], O_RDONLY, 0666);
		int fdTmp = open("tmp.map", O_WRONLY|O_CREAT, 0666);
		if(mapFile==-1 || fdTmp==-1)
			usage2("Le fichier ne s'est pas ouvert correctement.");
		if(argI!=7){
			if(argc<4)
				usage2("Quatrieme argument requis.");
			if(argI==4 || argI==5)
				setWidthHeight(argv[3], mapFile, fdTmp, argI);
			else
				setObjects(argc, argv, mapFile, fdTmp);
		}else
			pruneObjects(mapFile, fdTmp);
		close(mapFile);
		char * commandeSys = malloc(sizeof(char)*(strlen("mv tmp.map ")+strlen(argv[1])));
		strcat(commandeSys, "mv tmp.map ");
		strcat(commandeSys, argv[1]);
		execlp("/bin/sh" ,"sh", "-c", commandeSys, NULL);
		free(commandeSys);
	}else
		usage2("Commande invalide.");
	return EXIT_SUCCESS;
}                                                                                                                                                                      
