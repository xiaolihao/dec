#include <stdio.h>
#include <time.h>

int main(int argc, char* argv[]){
	printf("%s\n", argv[1]);
	
	char file[1024];
	int i, j;
	i=rand()%100;
	j=rand()%100;
	sprintf(file, "%s/%d", argv[2], time(0));
	
	FILE *fp=NULL;
	fp=fopen(file, "a+");
	fwrite(&i, 1, 4, fp);
	fwrite(&j, 1, 4, fp);
	fclose(fp);
	exit(0);
	return 0;
}
