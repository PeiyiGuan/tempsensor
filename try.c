#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

int main (int argc, char *argv[]) {
 int firstMsg;
 float tempC,tempHolder,lowestTemp,highestTemp,newestTemp;
 firstMsg=1;
 DIR *dir;
 struct dirent *dirent;
 char dev[16];      /* Dev ID*/
 char devPath[128]; /* Path to device*/
 char buf[256];     /* Data from device*/
 char tmpData[6];   /* Temp C * 1000 reported by device */
 char path[] = "/sys/bus/w1/devices"; 
 ssize_t numRead;

if(argc<2){
  printf("Usage ./temp args\n");
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
   numRead = read(fd, buf, 256);
   strncpy(tmpData, strstr(buf, "t=") + 2, 5);  /* strstr returns the second string found to the end of the first argument)*/
   tempC = strtof(tmpData, NULL);  /* casting string to float*/
   if(firstMsg){
     lowestTemp=tempC;
     highestTemp=tempC;
     tempHolder=tempC;
     /*first msg*/
     firstMsg=0;/*set to false*/
   }
   if(tempC<lowestTemp)lowestTemp=tempC;
   if(tempC>highestTemp)highestTemp=tempC;

   if(abs(tempC-tempHolder)>1){
     /*send msg changed over 1 degree*/
    /*ifttt("https://maker.ifttt.com/trigger/temp_triggered/with/key/d5Vq1kWG7H9tlVCzsvxpzT",tempC,lowestTemp,highestTemp);*/
     tempHolder = tempC;
   }
   printf("\nDevice: %s  - ", dev); 
   printf("Temp: %.3f C  \n", tempC / 1000);   
   close(fd);
 } 
        /* return 0; --never called due to loop */
}