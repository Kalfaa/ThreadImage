#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "lib/bitmap.h"
#include <stdint.h>
#include <dirent.h>
#include <pthread.h>
#include "example/edge-detect.c"
// ARG DIRECTORYNAME THREAD NUMBER
int* GLOBAL_THREAD_WORKING;
int GLOBAL_IMAGE_REMAINING;
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


void* treatment(ImageToModify image){
    apply_effect(&image.start, &image.end);
}

int init(ImageToModify** listImage,char* path){
    int count = count_images(path);
    GLOBAL_IMAGE_REMAINING = count;
    getListImage(path,count,listImage);
    return count ;
}




void consumer(ImageToModify* imageList,int imageNumber,int threadNumber,pthread_t * threadList){
    int imageSent=0;
    while(GLOBAL_IMAGE_REMAINING>0){
        for(int i =0;i<threadNumber;i++){
            if(GLOBAL_THREAD_WORKING[i]==0){
                pthread_create(&threadList[i], NULL, treatment, &imageList[imageSent]);
                imageSent++;
                GLOBAL_THREAD_WORKING[i]=1;
            }
        }
    }
}



int main(int argc, char** argv)
{
    char* path = "/home/theo/CLionProjects/ThreadImage/TestBitmap";
    ImageToModify* listImage;
    int imageNumber = init(&listImage,path);
    pthread_t ids[8];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);


    for(int i = 0, thread_id = 0; i < 8; i+=2) {
        pthread_join(ids[i], NULL);
    }
    consumer(listImage,imageNumber,8,ids);
    return 0;
}
