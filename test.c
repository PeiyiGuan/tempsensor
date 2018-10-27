#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>
#include "ifttt.h"

float temp_diff(float a,float b);


int main (int argc, char *argv[]) {
 int firstMsg;
 float tempC,tempHolder,lowestTemp,highestTemp;
 firstMsg=1;
 DIR *dir;
 struct dirent *dirent;
 char dev[16];      /* Dev ID*/
 char devPath[128]; /* Path to device*/
 char buf[256];     /* Data from device*/
 char tmpData[6];   /* Temp C * 1000 reported by device */
 char path[] = "/sys/bus/w1/devices"; 
 char buf_low[64];
 char buf_high[64];
 char buf_new[64];

if(argc!=2){
  printf("Usage ./temp serialNumber\n");
  exit(0);
}

 dir = opendir (path);
 if (dir != NULL)
 {
  while ((dirent = readdir (dir)))
   /* 1-wire devices are links beginning with 28-*/
   if (dirent->d_type == DT_LNK && 
     strstr(dirent->d_name, "28-") != NULL) { 
    strcpy(dev, dirent->d_name);
    printf("\nDevice: %s\n", dev);
   }
        (void) closedir (dir);
        }
 else
 {
  printf ("System error\n");
  exit(0);
 }

 sprintf(devPath, "%s/%s/w1_slave", path, dev);

 while(1) {
/* if file is readable*/
  int fd = open(devPath, O_RDONLY);  /* returns -1 if fails otherwise return positive integer*/
  /* if fd equal -1, no such device found*/
  if(fd == -1) {
   printf ("Temperature sensor not found.\n");
   exit(0);   
  }
/* reading fd to buffer, return -1 when fails*/
   read(fd, buf, 256);
   strncpy(tmpData, strstr(buf, "t=") + 2, 5);  /* strstr returns the second string found to the end of the first argument)*/
   tempC = strtof(tmpData, NULL);  /* casting string to float*/
   if(firstMsg){
     
     lowestTemp=tempC;
     highestTemp=tempC;
     tempHolder=tempC;

     gcvt(tempC/1000,6,buf_new);
     gcvt(lowestTemp/1000,6,buf_low);
     gcvt(highestTemp/1000,6,buf_high);
     /*first msg
      cast float to str
     */
     ifttt("https://maker.ifttt.com/trigger/temp_changed/with/key/d5Vq1kWG7H9tlVCzsvxpzT",buf_new,buf_high,buf_low);
     firstMsg=0;/*set to false*/
   }
   if(tempC<lowestTemp)lowestTemp=tempC;
   if(tempC>highestTemp)highestTemp=tempC;
  printf("tempc : %f\n",tempC/1000);
  printf("tempholder : %f\n",tempHolder/1000);
  float offset = temp_diff(tempC/1000,tempHolder/1000);
   if(offset>1.0){
     
     gcvt(tempC/1000,6,buf_new);
     gcvt(lowestTemp/1000,6,buf_low);
     gcvt(highestTemp/1000,6,buf_high);
     /*first msg
      cast float to str
     */
     ifttt("https://maker.ifttt.com/trigger/temp_changed/with/key/d5Vq1kWG7H9tlVCzsvxpzT",buf_new,buf_high,buf_low);
     tempHolder = tempC;
     printf("difference since last msg %f\n",offset);
   }
   printf("\nDevice: %s  - ", dev); 
   printf("Temp: %.3f C low %.3f  high %.3f \n", tempC / 1000,lowestTemp/1000,highestTemp/1000);   
   delay(1000);
    
   close(fd);
 } 
        /* return 0; --never called due to loop */
}

float temp_diff(float a, float b){
  /*if(a>b) return a-b;
  return b-a;
*/
  return a>b ? (a-b):(b-a);
}