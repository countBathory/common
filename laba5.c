#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

char** GetFiles(char* dirPath, int *length)
{
    DIR *dir = NULL;
    struct dirent *dirEntry = NULL;
    struct stat fileStat;
    char pathName[256] = "",ext[5];
    int i = 0;
    char **name = NULL;
    name = (char**) calloc(sizeof(char*), 10);

    dir = opendir(dirPath);

    dirEntry = readdir(dir);
    while (dirEntry != NULL)
    {
        pathName[0] = '\0';
        strcat(pathName, dirPath);
        strcat(pathName, "/");
        strcat(pathName, dirEntry->d_name);
        stat(pathName, &fileStat);
        if(S_ISREG(fileStat.st_mode))
        {
            strncpy(ext,&(dirEntry->d_name)[strlen(dirEntry->d_name) - 4],4);
            if (!strcmp(ext,".bmp")){
                i++;
                (name)[i-1] = (char*) calloc(sizeof(char), strlen(dirEntry->d_name)+1);
                strcpy((name)[i-1], dirEntry->d_name);
                printf("%s\n",(name)[i-1]);
            }
        }
        dirEntry = readdir(dir);
    }
    *length = i;
    return name;
}

int filesize( FILE *fp )
{
    int save_pos, size_of_file;
    save_pos = ftell( fp );
    fseek( fp, 0L, SEEK_END );
    size_of_file = ftell( fp );
    fseek( fp, save_pos, SEEK_SET );
    return( size_of_file );
}

int main() {
    FILE *f;
    char dir[256]="/root/pic/",filePath[256]="", **fileNames = NULL;
    int fileCount=0,p[2], pid;
    //read directory
    printf("Founded files\n");
    fileNames = GetFiles(dir,&fileCount);

    if (!pipe(p)){
        pid = fork();
        if (pid > 0){
            close(p[0]);
            for (int i=0;i<fileCount;i++){
		strcpy(filePath, dir);
                strcat(filePath,(fileNames)[i]);
                f = fopen(filePath,"rb");

                int size = filesize(f);
                fseek(f, 0L, SEEK_SET);

                write(p[1],(char *)&size,sizeof(int));

                char *buf = malloc(size);
                fread(buf,size,1,f);
                write(p[1],buf,size);

                free(buf);
                fclose(f);
            }
            close(p[1]);
            wait(NULL);
	    printf("Done\n");
        }
        else {
	    sleep(1);
            close(p[1]);
            for (int i=0;i<fileCount;i++){

		//create modified picture file
		strcpy(filePath, dir);
		strcat(filePath,"blue_");
                strcat(filePath,(fileNames)[i]);

                f = fopen(filePath,"w+");

                int size = 0;

                read(p[0],&size,sizeof(int));

                char *buf = malloc(size);
                read(p[0],buf,size);

		//Get info from BITMAPINFO (32-bit)

                int height=*(int*)&buf[22];
                int width=*(int*)&buf[18];
                int offset=*(int*)&buf[10];

		//put 0-value to red and green channel
                for (long int i = 1; i <= width * 3 * height; i++){
                    if (i % 3 != 0){
                        buf[offset + i]=0;
                    }
                }

                fwrite(buf, size, 1, f);

                free(buf);

                fclose(f);
            }
            close(p[0]);
        }
    } else exit(EXIT_FAILURE);
	
    return 0;
}
