#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void copyStringExceptNewline(char* destination, char* src) {
	int lenOfSrc = strlen(src);
	int i;
	for(i = 0; src[i] != '\n'; i++) {
		destination[i] = src[i];
	}
	destination[i] = '\0';

} // End copyStringExceptNewline



// Whut dksfsd
int main(int argc, char** argv) {
	const char strInc[] = "#include \"";	
	
	char buf[1024];
	char* sprite_inc_files[1024];
	char* sprite_header_files[1024];
	FILE* fp;
	FILE* pp;
	int incCount = 0;
	int headerCount = 0;

	system("rm src/game/sprites/sprite_hdr_db.h"); // Deletes the old one so
						   // it is ignored
	system("rm src/game/sprites/spritedb.inc"); // Deletes the old one so it
						// is ignored.
	pp = popen("cd src && find game/sprites/ -name '*.inc' ", "r");
		
	fgets(buf, sizeof(buf), pp);
	while(buf[0] != '\0') {
		sprite_inc_files[incCount] = malloc(strlen(buf)/* + 1 - 1*/);
		// +1 for the null term, but -1 to ignore the trailing newline.
		copyStringExceptNewline(sprite_inc_files[incCount], buf);	
		incCount++;
		buf[0] = '\0';	
		fgets(buf, sizeof(buf), pp);
	}

	fp = fopen("src/game/sprites/spritedb.inc", "w");
	for(int i = 0; i < incCount; i++) {
		fputs(strInc, fp);
		fputs(sprite_inc_files[i], fp);
		fputs("\"\n", fp);
	}

	fclose(fp);
	pclose(pp);

	// Create a .h file that includes the headers of all the sprites.
	
	pp = popen("cd src && find game/sprites/ -name '*.h'", "r");
	fgets(buf, sizeof(buf), pp);
	while(buf[0] != '\0') {
		sprite_header_files[headerCount] = malloc(strlen(buf)/* +1 - 1*/);
		copyStringExceptNewline(sprite_header_files[headerCount], buf);
		headerCount++;
		buf[0] = '\0';
		fgets(buf, sizeof(buf), pp);
	}
	fp = fopen("src/game/sprites/sprite_hdr_db.h", "w");
	for(int i = 0; i < headerCount; i++) {
		fputs(strInc, fp);
		fputs(sprite_header_files[i], fp);
		fputs("\"\n", fp);
	}
	fclose(fp);
	pclose(pp);


}



