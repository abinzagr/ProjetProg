#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "map.h"

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
	int mapFile, c, i = -1;
	char * commande = argv[2];

	if(!strcmp(commande, "--getwidth"))
		i=0;
	if(!strcmp(commande, "--getheight"))
		i=1;
	if(!strcmp(commande, "--getobjects"))
		i=2;
	if(!strcmp(commande, "--getinfo"))
		i=3;
	if(!strcmp(commande, "--setwidth"))
		i=4;
	if(!strcmp(commande, "--setheight"))
		i=5;
	if(!strcmp(commande, "--setobjects"))
		i=6;
	if(!strcmp(commande, "--pruneobjects"))
		i=7;

	if(i>=0 && i<=3){
		mapFile = open(argv[1], O_RDONLY, 0666);
		if(mapFile==-1)
			usage2("Le fichier ne s'est pas ouvert correctement.");
		int k = 1;
		if(i==1)
			lseek(mapFile, sizeof(int), SEEK_SET);
		if(i==2)
			lseek(mapFile, 2*sizeof(int), SEEK_SET);
		if(i==3)
			k=3;
		for(int j=0 ; j<k ; j++){
			read(mapFile, &c, sizeof(int));
			printf("%d\n", c);
		}
	}else if(i>=4){ //&& i<=7){
		mapFile = open(argv[1], O_RDONLY, 0666);
		int fdTmp = open("tmp.map", O_WRONLY|O_CREAT, 0666);
		if(mapFile==-1 || fdTmp==-1)
			usage2("Le fichier ne s'est pas ouvert correctement.");
		if(i!=7){
			if(argc<4)
				usage2("Quatrieme argument requis.");
			if(i==4 || i==5){
				int arg = atoi(argv[3]);
				int whn, x = 0, y, n, changeOfMap, nbbytes = 1;
				if(!arg)
					usage2("Quatrieme argument invalide (requiert un entier).");
				if(i==4 && (arg<MIN_WIDTH||arg>MAX_WIDTH))
					usage2("Largeur non valide.");
				if(i==5 && (arg<MIN_HEIGHT||arg>MAX_HEIGHT))
					usage2("Hauteur non valide.");
				for(int j=0 ; j<3 ; j++){
					read(mapFile, &whn, sizeof(int));
					if((i==4 && j==0) || (i==5 && j==1)){
						write(fdTmp, &arg, sizeof(int));
						if(i==5)
							changeOfMap = arg-whn;
					}
					else
						write(fdTmp, &whn, sizeof(int));
				}

				while(x!=-1){
					read(mapFile, &x, sizeof(int));
					if(x!=-1){
						read(mapFile, &y, sizeof(int));
						read(mapFile, &n, sizeof(int));
						if(i==4 && x<arg){
							write(fdTmp, &x, sizeof(int));
							write(fdTmp, &y, sizeof(int));
							write(fdTmp, &n, sizeof(int));
						}else if(i==5){
							y+=changeOfMap;
							if(y<arg && y>=0){
								write(fdTmp, &x, sizeof(int));
								write(fdTmp, &y, sizeof(int));
								write(fdTmp, &n, sizeof(int));
							}
						}
					}
				}printf("test\n");
				write(fdTmp, &x, sizeof(int));
				while(nbbytes!=0){
					nbbytes = read(mapFile, &x, sizeof(int));
					write(fdTmp, &x, nbbytes);
				}
			}else{
				if(((argc-3)%6))
					usage2("Nombre d'arguments de la liste invalide (6 demandes).");
				int x=0, y, n, length, frame, solidity, destructible, collectible, generator;
				char * name = NULL;
				char * objectArg = NULL;

				while(x!=-1){
					read(mapFile, &x, sizeof(int));
					write(fdTmp, &x, sizeof(int));
				}

				for(int i=3 ; i<argc ; i+=6){
					length = strlen(argv[i]);
					name = realloc(name, length*sizeof(char));
					strcpy(name, argv[i]);
					if(atoi(argv[i+1])<1)
						usage2("Nombre de frames invalides.");
					else
						frame = atoi(argv[i+1]);

					objectArg = realloc(objectArg, strlen(argv[i+2])*sizeof(char));
					strcpy(objectArg, argv[i+2]);
					if(strcmp(objectArg,"solid"))
						solidity = MAP_OBJECT_SOLID;
					else if(strcmp(objectArg,"semi-solid"))
						solidity = MAP_OBJECT_SEMI_SOLID;
					else if(strcmp(objectArg,"air"))
						solidity = MAP_OBJECT_AIR;
					else
						usage2("Valeur de solidite : solid | semi-solid | air.");

					objectArg = realloc(objectArg, strlen(argv[i+3])*sizeof(char));
					strcpy(objectArg, argv[i+3]);
					if(strcmp(objectArg,"destructible"))
						destructible = 1;
					else if(strcmp(objectArg,"not-destructible"))
						destructible = 0;
					else
						usage2("Valeur de destructible : destructible | not-destructible.");

					objectArg = realloc(objectArg, strlen(argv[i+4])*sizeof(char));
					strcpy(objectArg, argv[i+4]);
					if(strcmp(objectArg,"collectible"))
						collectible = 1;
					else if(strcmp(objectArg,"not-collectible"))
						collectible = 0;
					else
						usage2("Valeur de collectible : collectible | not-collectible.");

					objectArg = realloc(objectArg, strlen(argv[i+5])*sizeof(char));
					strcpy(objectArg, argv[i+5]);
					if(strcmp(objectArg,"generator"))
						collectible = 1;
					else if(strcmp(objectArg,"not-generator"))
						collectible = 0;
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
				lseek(fdTmp, 2*sizeof(int), SEEK_SET);
				length = (argc-3)/6;
				write(fdTmp, &length, sizeof(int));
				free(name);
				free(objectArg);

				lseek(mapFile, 2*sizeof(int), SEEK_SET);
				read(mapFile, &x, sizeof(int));
				if(x>length){
					length--;
					while(x!=-1){
						read(mapFile, &x, sizeof(int));
						if(x!=-1){
							read(mapFile, &y, sizeof(int));
							read(mapFile, &n, sizeof(int));
							lseek(fdTmp, 2*sizeof(int), SEEK_CUR);
							if(n>length)
								write(fdTmp, &length, sizeof(int));
							else
								lseek(fdTmp, sizeof(int), SEEK_CUR);
						}
					}
				}
			}
		}else{
		}
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
