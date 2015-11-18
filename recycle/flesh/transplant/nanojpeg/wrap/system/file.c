#include<stdio.h>
#include<stdlib.h>
void initfile (void) __attribute__ ((constructor));
void destoryfile (void) __attribute__ ((destructor));


unsigned char* myfile;
int size;


void initfile()
{
	FILE* fp=fopen("jarvis.jpg","rb");

	fseek(fp,0,SEEK_END);
	size=ftell(fp);

	fseek(fp,0,SEEK_SET);
	myfile=(unsigned char*)malloc(size);
	fread(myfile,size,1,fp);

	fclose(fp);

	//printf("@%llx,%llx\n",myfile,*(unsigned long long*)myfile);
}
void destoryfile()
{
	free(myfile);
}







unsigned long long fileaddr()
{
	//printf("file@%llx\n",myfile);
	return (unsigned long long)myfile;
}
int filesize()
{
	//printf("size=%d\n",size);
	return size;
}
