#include "game/test/instance.h"

int main(int argc, char** argv) {
	Instance instance;
	
	Instance_Init(&instance, argc, argv, 12, 512);
	Instance_Run(&instance, NULL);
	
	return 0;
} // End main

