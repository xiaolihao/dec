#include <stdio.h>
#include <time.h>

int main(int argc, char *argv[]){
	
	FILE *fp=fopen(argv[1], "w+");
	srand(time(0));
	int i=rand()%100;
	int j=rand()%100;
        fwrite(&i, 4, 1, fp);
	fwrite(&j, 4, 1,fp);
	fclose(fp);
	exit(0);
	return 0;
}
