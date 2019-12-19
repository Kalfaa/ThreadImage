#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "lib/bitmap.h"
#include <stdint.h>
#include <dirent.h>
#include <pthread.h>
#include "example/edge-detect.c"
// ARG DIRECTORYNAME THREAD NUMBER

int GLOBAL_IMAGE_REMAINING;
typedef struct imageToModify {
    Image start;
    Image end;
    char * name;
    int i;
}ImageToModify;

struct ThreadArg{
    struct ImageToModify* structImageToModify;
    int imageRemaining;
    int threadWorking;
};


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
            temp[count].i=12;
            count++;
        }
    }
    closedir(dirp);
    *imageToModify=temp;
}


void* treatment(void* image){
    ImageToModify *temp = (ImageToModify*) image;
    printf("ee %s",temp->name);
    apply_effect(&temp->start, &temp->end);
}

int init(ImageToModify** listImage,char* path){
    int count = count_images(path);
    GLOBAL_IMAGE_REMAINING = count;
    getListImage(path,count,listImage);
    return count ;
}

void arrayToZero(int** array,int size){
    for(int i = 0; i<size-1;i++){
        array[i]=0;
    }
}

void * consumer(){

}


void displayWork(int imageRemaining,int imageNumber){
    printf("\rProgress ... %d / %d ",imageNumber-imageRemaining,imageNumber);
    fflush(stdout);
}

void start(ImageToModify* imageList, int imageNumber, int threadNumber){
    //printf("ee %s",imageList[0].name);
    pthread_t threadList[threadNumber-1];
    pthread_t consumer;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    int threadWorking[threadNumber];
    arrayToZero(&threadWorking,threadNumber);
    int imageSent=0;
    pthread_create(&threadList[0], NULL, treatment, (void *)(imageList+imageSent));
    pthread_join(threadList[0],NULL);
    /*while(GLOBAL_IMAGE_REMAINING>0){
        usleep(200);
        //displayWork(GLOBAL_IMAGE_REMAINING,imageNumber);
        for(int i =0;i<threadNumber;i++){
            if(threadWorking[i]==0){
                pthread_create(&threadList[i], &attr, treatment, &imageList[imageSent]);

                imageSent++;
                threadWorking[i]=1;
            }
        }
    }*/
}



int main(int argc, char** argv)
{
    char* path = "/home/theo/CLionProjects/ThreadImage/TestBitmap";
    ImageToModify* listImage;
    printf("Init...");
    int imageNumber = init(&listImage,path);
    start(listImage, imageNumber, 8);
    return 0;
}

