#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void usage(char * commande){
	fprintf(stderr, "%s fichier commande (entier/listeCaracteristiquesObjets)\n", commande);
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
		if(mapFile==-1){
			fprintf(stderr, "Le fichier ne s'est pas ouvert correctement.\n");
			exit(EXIT_FAILURE);
		}
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
		if(mapFile==-1 || fdTmp==-1){
			fprintf(stderr, "Le fichier ne s'est pas ouvert correctement.\n");
			exit(EXIT_FAILURE);
		}
		if(i!=7){
			if(argc<4){
				fprintf(stderr, "Quatrieme argument requis.\n");
				usage(argv[0]);
			}
			if(i==4 || i==5){
				int arg = atoi(argv[3]);
				int whn, x = 0, y, n, changeOfMap, nbbytes = 1;
				if(arg==0){
					fprintf(stderr, "Quatrieme argument invalide (requiert un entier).\n");
					exit(EXIT_FAILURE);
				}

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
				}
				write(fdTmp, &x, sizeof(int));
				while(nbbytes!=0){
					nbbytes = read(mapFile, &x, sizeof(int));
					write(fdTmp, &x, nbbytes);
				}
			}else{
			}
		}else{
		}
		close(mapFile);
		int length = (int)strlen("mv tmp.map ")+(int)strlen(argv[1]);
		char * commandeSys = malloc(sizeof(char)*length);
		strcat(commandeSys, "mv tmp.map ");
		strcat(commandeSys, argv[1]);
		execlp("/bin/sh" ,"sh", "-c", commandeSys, NULL);
		free(commandeSys);
	}else{
		fprintf(stderr, "Commande invalide.\n");
		exit(EXIT_FAILURE);
	}
	return EXIT_SUCCESS;
}                                                                                                                                                                      
