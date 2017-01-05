#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "map.h"
#include "error.h"

#ifdef PADAWAN

void map_new (unsigned width, unsigned height){
  map_allocate (width, height);

  for (int x = 0; x < width; x++)
    map_set (x, height - 1, 0); // Ground

  for (int y = 0; y < height - 1; y++) {
    map_set (0, y, 1); // Wall
    map_set (width - 1, y, 1); // Wall
  }

  map_object_begin (6);

  // Texture pour le sol
  map_object_add ("images/ground.png", 1, MAP_OBJECT_SOLID);
  // Mur
  map_object_add ("images/wall.png", 1, MAP_OBJECT_SOLID);
  // Gazon
  map_object_add ("images/grass.png", 1, MAP_OBJECT_SEMI_SOLID);
  // Marbre
  map_object_add ("images/marble.png", 1, MAP_OBJECT_SOLID | MAP_OBJECT_DESTRUCTIBLE);
  // Fleurs
  map_object_add ("images/flower.png", 1, MAP_OBJECT_AIR);
  // Pièce
  map_object_add ("images/coin.png", 20, MAP_OBJECT_AIR | MAP_OBJECT_COLLECTIBLE);

  map_object_end ();
}

void map_save (char *filename){
	int mapSave = open(filename, O_WRONLY|O_CREAT, 0666);

	int width = map_width();
	int height = map_height();
	int nbObjects = map_objects();

	write(mapSave, &width, sizeof(int));
	write(mapSave, &height, sizeof(int));
	write(mapSave, &nbObjects, sizeof(int));

	int object, endOfMap = -1;
	for(int i=0 ; i<width ; i++)
		for(int j=0 ; j<height ; j++){
			object = map_get(i,j);
			if(object!=MAP_OBJECT_NONE){
				write(mapSave, &i, sizeof(int));
				write(mapSave, &j, sizeof(int));
				write(mapSave, &object, sizeof(int));				
			}
		}
	write(mapSave, &endOfMap, sizeof(int));
	
	int length, framesObj, solidityObj, destructibleObj, collectibleObj, generatorObj;
	for(int i=0 ; i<nbObjects ; i++){
		length = strlen(map_get_name(i));
		framesObj = map_get_frames(i);
		solidityObj = map_get_solidity(i);
		destructibleObj = map_is_destructible(i);
		collectibleObj = map_is_collectible(i);
		generatorObj = map_is_generator(i);

		write(mapSave, &length, sizeof(int));
		write(mapSave, map_get_name(i), strlen(map_get_name(i))*sizeof(char));
		write(mapSave, &framesObj, sizeof(int));
		write(mapSave, &solidityObj, sizeof(int));
		write(mapSave, &destructibleObj, sizeof(int));
		write(mapSave, &collectibleObj, sizeof(int));
		write(mapSave, &generatorObj, sizeof(int));
	}

	printf("Map save performed to the end\n");
}

void map_load (char *filename){
	int mapLoad = open(filename, O_RDONLY, 0666);
	
	int width, height, nbObjects;
	read(mapLoad, &width, sizeof(int));
	read(mapLoad, &height, sizeof(int));
	read(mapLoad, &nbObjects, sizeof(int));
	map_allocate (width, height);
	int x = 0, y, nameObj;
	while(x!=-1){
		read(mapLoad, &x, sizeof(int));
		if(x!=-1){
			read(mapLoad, &y, sizeof(int));
			read(mapLoad, &nameObj, sizeof(int));
			map_set(x, y, nameObj);
		}
	}
	map_object_begin (nbObjects);
	int length, frame, solidity, destructible, collectible, generator;	

	char * name = NULL;
	for(int i=0 ; i<nbObjects ; i++){
		read(mapLoad, &length, sizeof(int));

		name = realloc(name, (length+1)*sizeof(char));
		name[length]='\0';
		read(mapLoad, name, length*sizeof(char));

		read(mapLoad, &frame, sizeof(int));
		read(mapLoad, &solidity, sizeof(int));
		read(mapLoad, &destructible, sizeof(int));
		read(mapLoad, &collectible, sizeof(int));
		read(mapLoad, &generator, sizeof(int));
		map_object_add(name, frame, solidity | ((destructible)?MAP_OBJECT_DESTRUCTIBLE:solidity) | ((collectible)?MAP_OBJECT_COLLECTIBLE:solidity) | ((generator)?MAP_OBJECT_GENERATOR:solidity));
	}
	free(name);
	map_object_end ();
}

#endif
