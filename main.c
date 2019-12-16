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



int count_images(char* path)
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
    return file_count;
}

void getListImage(char* path ,int imageNumber,ImageToModify** imageToModify){
    ImageToModify* temp = malloc(sizeof(ImageToModify) * imageNumber);
    DIR * dirp;
    struct dirent * entry;
    dirp = opendir(path);

    int count =0;
    while ((entry = readdir(dirp)) != NULL) {
        if (entry->d_type == DT_REG) {
            temp[count].start=open_bitmap(entry->d_name);
            temp[count].name= entry->d_name;
            count++;
        }
    }
    closedir(dirp);
    *imageToModify=temp;
}

void init(ImageToModify** listImage,char* path){
    int count = count_images(path);
    getListImage(path,count,listImage);
}

int main(int argc, char** argv)
{
    char* path = "/home/theo/CLionProjects/ThreadImage/TestBitmap";
    ImageToModify* listImage;
    init(&listImage,path);
    printf("%s",listImage[0].name);
    return 0;
}


void consumer(Image** image,int imageNumber){
    int count = 0;
}


