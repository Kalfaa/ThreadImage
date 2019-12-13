#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "lib/bitmap.h"
#include <stdint.h>
#include <dirent.h>


// ARG DIRECTORYNAME THREAD NUMBER


typedef struct imageToModify {
    Image start;
    Image end;
    char * name;
}ImageToModify;



void find_images(char* path, ImageToModify** imageList)
{
    int file_count = 0;
    DIR * dirp;
    struct dirent * entry;

    dirp = opendir(path); /* There should be error handling after this */
    while ((entry = readdir(dirp)) != NULL) {
        if (entry->d_type == DT_REG) { /* If the entry is a regular file */
            file_count++;
        }
    }
    ImageToModify imageToModify[file_count];
    int count =0;
    while ((entry = readdir(dirp)) != NULL) {
        if (entry->d_type == DT_REG) {
            imageToModify[count].start=open_bitmap(entry->d_name);
            imageToModify[count].name= entry->d_name;
            count++;
        }
    }

    closedir(dirp);
    imageList = imageToModify;
    return ;
}

int main(int argc, char** argv)
{
    ImageToModify** listImage;
    find_images("/home/theo/CLionProjects/ThreadImage/TestBitmap",listImage);
    printf("Hello, World!\n");
    return 0;
}


void consumer(Image** image,int imageNumber){
    int count = 0;
}


