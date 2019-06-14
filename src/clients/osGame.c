#include "game/osGame/osGameInst.h"

int main(int argc, char** argv) {
	OsGameInst instance;
	
	OsGameInst_Init(&instance, argc, argv, 12, 512);
	OsGameInst_Run(&instance, NULL);
	OsGameInst_Destroy(&instance);	
	return 0;
} // End main

