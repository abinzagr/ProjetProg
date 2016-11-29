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
		mapFile = open(argv[1], O_RDWR, 0666); // ouvrir fichier original en RD_ONLY, créer copy modifier en WR_ONLY.
		if(mapFile==-1){
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
				int wh, x = 0, y, n;
				if(arg==0){
					fprintf(stderr, "Quatrieme argument invalide (requiert un entier).\n");
					exit(EXIT_FAILURE);
				}
				if(i==5)
					lseek(mapFile, sizeof(int), SEEK_SET);
				read(mapFile, &wh, sizeof(int));
				lseek(mapFile, -sizeof(int), SEEK_CUR);
				write(mapFile, &arg, sizeof(int));
				lseek(mapFile, 3*sizeof(int), SEEK_SET);
				while(x!=-1){
					read(mapFile, &x, sizeof(int));
					if(x!=-1){
						read(mapFile, &y, sizeof(int));
						read(mapFile, &n, sizeof(int));
					}
				} // A FINIR
			}else{
			}
		}else{
		}
	}else{
		fprintf(stderr, "Commande invalide.\n");
		exit(EXIT_FAILURE);
	}
	return EXIT_SUCCESS;
}                                                                                                                                                                      
