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

	char n = '\n';
	int endOfMap = -1;
	int width = map_width();
	int height = map_height();
	int nbObjects = map_objects();

	int object, nameObj;//, framesObj, solidityObj, destructibleObj, collectibleObj, generatorObj;

	write(mapSave, &width, sizeof(int));
	write(mapSave, &height, sizeof(int));
	write(mapSave, &nbObjects, sizeof(int));
	write(mapSave, &n, sizeof(char));

	for(int i=0 ; i<width ; i++)
		for(int j=0 ; j<height ; j++){
			object = map_get(i,j);
			if(object!=MAP_OBJECT_NONE){
				if(!strcmp(map_get_name(object), "images/ground.png"))
					nameObj = 0;
				else if(!strcmp(map_get_name(object), "images/wall.png"))
					nameObj = 1;
				else if(!strcmp(map_get_name(object), "images/grass.png"))
					nameObj = 2;
				else if(!strcmp(map_get_name(object), "images/marble.png"))
					nameObj = 3;
				else if(!strcmp(map_get_name(object), "images/flower.png"))
					nameObj = 4;
				else if(!strcmp(map_get_name(object), "images/coin.png"))
					nameObj = 5;
				/*framesObj = map_get_frames(object);
				solidityObj = map_get_solidity(object);
				destructibleObj = map_is_destructible(object);
				collectibleObj = map_is_collectible(object);
				generatorObj = map_is_generator(object);*/

				write(mapSave, &i, sizeof(int));
				write(mapSave, &j, sizeof(int));
				write(mapSave, &nameObj, sizeof(int));
				/*write(mapSave, &framesObj, sizeof(int));
				write(mapSave, &solidityObj, sizeof(int));
				write(mapSave, &destructibleObj, sizeof(int));
				write(mapSave, &collectibleObj, sizeof(int));
				write(mapSave, &generatorObj, sizeof(int));*/
				write(mapSave, &n, sizeof(char));				
			}
		}
	write(mapSave, &endOfMap, sizeof(int));
	//write(mapSave, ,);

	printf("Map save performed to the end\n");
}

void map_load (char *filename){
	int mapLoad = open(filename, O_RDONLY, 0666);
	
	char n;
	int width=0;
	int height=0;
	int nbObjects = 0;
	read(mapLoad, &width, sizeof(int));
	read(mapLoad, &height, sizeof(int));
	read(mapLoad, &nbObjects, sizeof(int));
	read(mapLoad, &n, sizeof(char));
	
	if(width!=0 && height!=0)
		map_allocate (width, height);

	// Chargement map
	int x, y, nameObj;
	while(read(mapLoad, &x, sizeof(int))!=-1){
		read(mapLoad, &y, sizeof(int));
		read(mapLoad, &nameObj, sizeof(int));
		read(mapLoad, &n, sizeof(char));
		map_set(x, y, nameObj);
	}

	if(nbObjects!=0)
		map_object_begin (nbObjects);

	for(int i=0 ; i<nbObjects ; i++){
		read(mapLoad, ,);
		/*map_object_add ("images/ground.png", 1, MAP_OBJECT_SOLID);
		map_object_add ("images/wall.png", 1, MAP_OBJECT_SOLID);
		map_object_add ("images/grass.png", 1, MAP_OBJECT_SEMI_SOLID);
		map_object_add ("images/marble.png", 1, MAP_OBJECT_SOLID | MAP_OBJECT_DESTRUCTIBLE);
		map_object_add ("images/flower.png", 1, MAP_OBJECT_AIR);
		map_object_add ("images/coin.png", 20, MAP_OBJECT_AIR | MAP_OBJECT_COLLECTIBLE);*/
	}

	map_object_end ();
}

#endif
