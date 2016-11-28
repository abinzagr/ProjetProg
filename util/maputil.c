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

	int mapFile, c;
	char * commande = argv[2];
	if(!strcmp(commande, "--getwidth") || !strcmp(commande, "--getheight") || !strcmp(commande, "--getobjects") || !strcmp(commande, "--getinfo")){
		mapFile = open(argv[1], O_RDONLY, 0666);
		if(mapFile==-1){
			fprintf(stderr, "Le fichier ne s'est pas ouvert correctement.\n");
			exit(EXIT_FAILURE);
		}else{
			int k = 1;
			if(!strcmp(commande, "--getheight"))
				lseek(mapFile, sizeof(int), SEEK_SET);
			if(!strcmp(commande, "--getobjects"))
				lseek(mapFile, 2*sizeof(int), SEEK_SET);
			if(!strcmp(commande, "--getinfo"))
				k=3;
			for(int i=0 ; i<k ; i++){
				read(mapFile, &c, sizeof(int));
				printf("%d\n", c);
			}
		}
	}else if(!strcmp(commande, "--setwidth"), !strcmp(commande, "--setheight"), !strcmp(commande, "--setobjects"), !strcmp(commande, "--pruneobjects")){
		mapFile = open(argv[1], O_WRONLY, 0666);
		if(mapFile==-1){
			fprintf(stderr, "Le fichier ne s'est pas ouvert correctement.\n");
			exit(EXIT_FAILURE);
		}


	}else{
		fprintf(stderr, "Commande invalide.\n");
		exit(EXIT_FAILURE);
	}
	return EXIT_SUCCESS;
}                                                                                                                                                                      
