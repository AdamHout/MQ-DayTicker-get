

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

void parseCSV(char *pMsgBuf, struct indice *pInd){
   
   char *pDate, *pTime, *pTicker, *pOpen, *pHigh, *pLow, *pClose, *pEnd;
   int  idx;
   
   pDate = strtok(pMsgBuf," ");
   pTime = strtok(NULL,","); 
   pTicker = strtok(NULL,","); 
   pOpen = strtok(NULL,",");
   pHigh = strtok(NULL,",");
   pLow = strtok(NULL,",");
   pClose = strtok(NULL,",");
   
   if (strncmp(pTicker,"DJI",sizeof(pInd[0].ticker)) == 0)
      idx = 0;
   else if (strncmp(pTicker,"SPX",sizeof(pInd[0].ticker)) == 0)
      idx = 1; 
   else if (strncmp(pTicker,"NDX",sizeof(pInd[0].ticker)) == 0)
      idx = 2;
   else if (strncmp(pTicker,"GC",sizeof(pInd[0].ticker)) == 0)
      idx = 3;
   else
      idx = 4;
   
   //Populate the appropriate Indice structure
   pInd[idx].prvcls = strtod(pClose,&pEnd);
   strncpy(pInd[idx].time,pTime,sizeof(pInd[idx].time));
   strncpy(pInd[idx].ticker,pTicker,sizeof(pInd[idx].ticker));
   strncpy(pInd[idx].open,pOpen,sizeof(pInd[idx].open));
   strncpy(pInd[idx].high,pHigh,sizeof(pInd[idx].high));
   strncpy(pInd[idx].low,pLow,sizeof(pInd[idx].low));
   strncpy(pInd[idx].close,pClose,sizeof(pInd[idx].close));

   if (pInd[idx].begopen == 0)
      pInd[idx].begopen = strtod(pOpen,&pEnd);
}