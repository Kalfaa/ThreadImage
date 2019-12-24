#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "lib/bitmap.h"
#include <stdint.h>
#include <dirent.h>
#include <pthread.h>
#include "example/edge-detect.c"
// ARG DIRECTORYNAME THREAD NUMBER

pthread_cond_t condition = PTHREAD_COND_INITIALIZER; /* Création de la condition */
pthread_mutex_t mutex =  PTHREAD_MUTEX_INITIALIZER ;
typedef struct imageToModify {
    Image start;
    Image end;
    char * name;
    int* isTreated;
    int isWrote;
}ImageToModify;

typedef struct threadArg{
    struct ImageToModify * structImageToModify;
    int* imageFinished;
    int* threadWorking;
    pthread_mutex_t mutex;
    pthread_cond_t condFinish;
}ThreadArg;

typedef struct consumerArg{
    struct ImageToModify* imageList;
    int* imageFinished;
    int imageNumber;
    pthread_mutex_t mutex;
    pthread_cond_t condFinish;
}ConsumerArg;


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
            char* pathImage = malloc(strlen(path)+1+strlen(entry->d_name));
            strcpy(pathImage, path);
            strcat(pathImage, "/");
            strcat(pathImage, entry->d_name);
            Image testimage = open_bitmap(pathImage);
            temp[count].start= testimage;
            temp[count].name= malloc(sizeof(char)*240);
            strcpy(temp[count].name, entry->d_name);
            count++;
            free(pathImage);
        }
    }
    closedir(dirp);
    *imageToModify=temp;
}


void* treatment(void* arg){
    ThreadArg *threadArg = (ThreadArg*) arg;
    ImageToModify* image = threadArg->structImageToModify;
    apply_effect(&image->start, &image->end);
    //TODO ALOOO?
    image->isTreated=1;
    pthread_mutex_lock(&threadArg->mutex);
    pthread_cond_signal(&condition);
    *threadArg->imageFinished = *threadArg->imageFinished +1;
    *threadArg->threadWorking = *threadArg->threadWorking -1;
    pthread_mutex_unlock(&threadArg->mutex);
    printf("FINISHHH");
}

int init(ImageToModify** listImage,char* path){
    int count = count_images(path);
    getListImage(path,count,listImage);
    return count ;
}

void arrayToZero(int** array,int size){
    for(int i = 0; i<size-1;i++){
        array[i]=0;
    }
}

void * writeImage(ImageToModify image){
    char* path = "/home/theo/CLionProjects/ThreadImage/result";
    char* writeImage = malloc(strlen(path)+1+strlen(image.name)+7);
    strcpy(writeImage, path);
    strcat(writeImage, "/");
    strcat(writeImage, image.name);
    save_bitmap(image.end, writeImage);
}

int findImageToWrite(ImageToModify * list , int imageNumber){
    for(int i = 0 ;i<imageNumber-1;i++){
        if(*list[i].isTreated == 1 && list[i].isWrote==0  ){
            return i;
        }
    }
}

void * consumeImage(void * arg){
    //ConsumerArg *Consumer
    int count = 0;
    int imageIndex = 0;
    ConsumerArg *consumerArgs = (ConsumerArg*) arg;
    ImageToModify* list = consumerArgs->imageList;
    ImageToModify image = list[0];
    writeImage(image);
    while(count<consumerArgs->imageNumber)
    {
        pthread_mutex_lock(&consumerArgs->mutex);
        printf("%d",*consumerArgs->imageFinished);
        while(*consumerArgs->imageFinished == 0) {
            printf("%d",*consumerArgs->imageFinished);
        }
        pthread_mutex_lock(&consumerArgs->mutex);
        *consumerArgs->imageFinished = *consumerArgs->imageFinished -1;
        imageIndex=findImageToWrite(list,consumerArgs->imageNumber);
        writeImage(list[imageIndex]);
        list[imageIndex].isWrote = 1;
        count++;
    }
}


void displayWork(int imageFinished,int imageNumber){
    printf("\rProgress ... %d / %d ",imageFinished,imageNumber);
    fflush(stdout);
}

void start(ImageToModify* imageList, int imageNumber, int threadNumber){
    int imageFinished = 0;
    int threadWorking=0;
    int imageSent=0;
    int imageRemaining = imageNumber;
    pthread_t threadList[imageNumber];
    pthread_t consumer;
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    //arrayToZero(&threadWorking,threadNumber);
    ThreadArg tArgs;
    tArgs.structImageToModify = &imageList[0];
    tArgs.imageFinished = &imageFinished;
    tArgs.threadWorking = &threadWorking;
    tArgs.mutex = mutex;

    ConsumerArg cArgs;
    cArgs.mutex = mutex;
    cArgs.imageFinished = &imageFinished;
    cArgs.imageList = imageList;
    cArgs.imageNumber= imageNumber;
    pthread_cond_init(&cArgs.condFinish, NULL);



    //pthread_create(&threadList[0], NULL, treatment, (void *)&tArgs);
    //pthread_join(threadList[0],NULL);
    //pthread_create(&consumer, NULL, consumeImage, (void *)&cArgs);
   // pthread_create(&consumer, NULL, consumeImage, (void *)&cArgs);

   while(imageSent<imageNumber){
        displayWork(imageFinished,imageNumber);
        while(threadWorking<threadNumber-1 && imageSent<imageNumber) {
            ThreadArg tArgs;
            tArgs.structImageToModify = &imageList[imageSent];
            tArgs.imageFinished = &imageFinished;
            tArgs.threadWorking = &threadWorking;
            tArgs.condFinish = cArgs.condFinish;
            threadWorking++;
            pthread_create(&threadList[imageSent],NULL, treatment, (void *)&tArgs);
            imageSent++;
        }
    }
    pthread_join(threadList[0],NULL);


    printf("%d",imageFinished);
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

