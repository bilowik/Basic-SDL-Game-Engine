#ifndef SMPENG_FILEPATHHANDLER_H
#define SMPENG_FILEPATHHANDLER_H
/* FilePathHandler.h
 * Stores a stem of a path (the "root" directory) and can append different
 * paths top the end of it (different files in the root diretory). This 
 * makes opening files more efficiently that concatenation of strings for
 * every file path.
 *
 * NOTE: _path should NOT have a trailing slash, all slashes should be 
 * prepended onto each directory or file appended to _path.
 *
 * EXAMPLE:
 *
 * _path = "content/resources"
 * '_pathToAppend' = "/images"
 * '_pathToAppend' = "/character.png"
 * 
 */
#include <stdint.h>

#define DEFAULT_SIZE_OF_PATH 512

typedef struct FilePathHandler FilePathHandler;

struct FilePathHandler {
	char* _path;

	int _lengthOfStem;
	// The length of the original stem of '_path' or the 'root' directory

	int _sizeOfPath;
	// The amount of memory allocated to '_path' 
	 
	uint16_t*  _folderIndexArray;
	// Holds the indices of where each new folder begins to allow to 
	// roll back to specified folders.
	// The size of this array is determinted by initialization with the 
	// constructor
	
	uint8_t _currFolderDepth;
	// Holds the current folder depth (not counting any depth within the
	// stem). Incremented when 'appendToPath' is called.
	// Set to 0 when 'removeToStem' is called.
	// This is also an index into '_folderNameLengthArray', as each index
	// corresponds to that folder's string length in the path
	
	int _currIndex;
	// The current index of the path.
	// It is the end of the last appended string, pointing to it's null
	// terminator




	// METHODS
};


void FilePathHandler_Init(FilePathHandler* this, const char* pathStem, 
		int lengthOfStem, int maxLength, int maxFolderDepth);
// 'pathStem'	:	The path of the directory that is to act as the root
// 			directory to be appended to. 
// 'lengthOfStem': 	The length of the stem (not the size)
//
// 'maxLength'	:	The maximum length of the '_path' buffer to accomadate
// 			different lengths of paths to files.
// 			If == NULL, the default is used (512)

void FilePathHandler_Destroy(FilePathHandler* this);

char* FilePathHandler_GetCurrWorkingDir(char* argv);
// Returns a mallocated string of the current working directory parsed from
// argv[0] (cross platform)



void  FilePathHandler_RemoveToStem(FilePathHandler* this);
// Purpose	:	'Clears' '_path' back to it's stem.	
// Pre		:	FilePathHandler has been initialized by a
// 			constructor. 
// Post		:	'_currFolderDepth' is set to 0.
// 			Sets '_path[_lengthOfStem]' to '\0'

char* FilePathHandler_AppendToPath(FilePathHandler* this, const char* path);
// Purpose	:	Appends strings to _path and returns a pointer
// 			to the whole path.
// Pre		:	FilePathHandler has been intialized by a 
// 			constructor
// Post		:	'path' is appended to _path and a null term
// 			is added.
// 			'_currFolderDepth' is incremented.
// 			currIndex is updated, points to the last null
// 			terminator.
// 			
// Params:
// path		:	The pointer to the array of strings to be
// 			appended.

char* FilePathHandler_AppendToPathMultiple(FilePathHandler* this, const char* path);
// Purpose	:	Appens multiple directories/files and returns 
// 			a pointer to the whole path.
// 			EX: /files/images/pic.png
// Pre		:	FilePathHandler has been initialized by a 
// 			constructor.
// Post		:	'_currFolderDpeth' is incremented by the number
// 			of directories in 'path' (for every / or \, 
// 			'_currFolderDepth' is incremented.
//			
void FilePathHandler_GoUpDirectories(FilePathHandler* this, int numDirs);
// Purpose	:	Goes back up 'numDirs' directory levels.
// Pre		:	FilePathHandler has been initialized by 
// 			a constructor.
// Post		:	'_currIndex' is decremented using the values
// 			contained in '_folderNameLengthArray'
// 			'_currFolderDepth' is decremented by 'numDirs'
//


char* FilePathHandler_GetPath(FilePathHandler* this);
// Returns '_path' 

#endif
