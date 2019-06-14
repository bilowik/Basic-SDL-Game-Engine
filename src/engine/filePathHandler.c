#include "filePathHandler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



void FilePathHandler_Init(FilePathHandler* this, const char* pathStem,
		int lengthOfStem, int maxLength, int maxFolderDepth) {
	
	this->_lengthOfStem = lengthOfStem;
	
	if(maxLength != 0) this->_sizeOfPath = maxLength;
	else this->_sizeOfPath = DEFAULT_SIZE_OF_PATH;

	this->_path = malloc(this->_sizeOfPath);
	int i = 0;
	while(i < lengthOfStem + 1) {
		// + 1 for the null terminator
		this->_path[i] = pathStem[i];
		i++;
	}
	
	this->_currFolderDepth = 0;
	this->_currIndex = this->_lengthOfStem;
	this->_folderIndexArray = malloc(maxFolderDepth * sizeof(uint16_t));
} // End FilePathHandler_Init




void FilePathHandler_Destroy(FilePathHandler* this) {
	free(this->_path);
	free(this->_folderIndexArray);

} // End FilePathHandler_Destroy




void FilePathHandler_RemoveToStem(FilePathHandler* this) {
	this->_currFolderDepth = 0; // Reset the curr folder depth
	this->_currIndex = this->_lengthOfStem;
	// Set the current index to the beginning of the original stem
	
	this->_path[this->_currIndex] = '\0';
	// Append a null terminator to the original stem

} // End _removeToStem




char* FilePathHandler_AppendToPath(FilePathHandler* this, const char* path) {
	this->_folderIndexArray[this->_currFolderDepth] = this->_currIndex;	
	this->_currFolderDepth++;

	int currChar = -1;
	this->_currIndex--;
	do {
		this->_currIndex++;
		currChar++;
		this->_path[this->_currIndex] = 
			path[currChar]; // Copy the char
	
	} while(path[currChar] != '\0');
	// Goes until the last char added into '_path' is the null
	// terminator
	
	return this->_path;	
} // End _appendToPath




char* FilePathHandler_AppendToPathMultiple(FilePathHandler* this, const char* path) {
	
	this->_currIndex--;
	int currChar = -1;
	do {
		this->_currIndex++;
		currChar++;
		this->_path[this->_currIndex] = 
			path[currChar];
		if((path[currChar] == '/') || path[currChar] == '\\') {
			this->_folderIndexArray[this->_currFolderDepth] = 
				this->_currIndex;	
			this->_currFolderDepth++;
		}

	} while(path[currChar] != '\0');
	return this->_path;
} // End _appendToPathMultiple




void FilePathHandler_GoUpDirectories(FilePathHandler* this, int numDirs) {
	this->_currFolderDepth -= numDirs;
	this->_currIndex = this->_folderIndexArray[this->_currFolderDepth];
	this->_path[this->_currIndex] = '\0';

} // End _goUpDirectories




char* FilePathHandler_GetPath(FilePathHandler* this) {
	return this->_path;
} // End _getPath




char* FilePathHandler_GetCurrWorkingDir(char* argv) {
	// Pass this argv[0]
	char* currWorkingDir;
	int lenOfPath = strlen(argv);
	int currIndex = lenOfPath - 1;
	while(argv[currIndex] != '/' && argv[currIndex] != '\\') {
		currIndex--;
	}
	currWorkingDir = malloc(currIndex + 1);
	int i = 0;
	while(i < currIndex) {
		currWorkingDir[i] = argv[i];
		i++;
	}
	currWorkingDir[i] = '\0'; // Append null term
	return currWorkingDir;
} // End FilePathHandler_getCurrWorkingDir


