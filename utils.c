/*
 * ----------------------------------------------------------------------------------------------
 * Date       Author        Description
 * ----------------------------------------------------------------------------------------------
 * 10/17/23   A. Hout       Original source
 * ----------------------------------------------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>                                                        //usleep()
#include "utils.h"

//--------------------------------------------------------------------
//Function to initalize the screen and print the display headers
//--------------------------------------------------------------------
void dspHdr(void){
   printf(ESC CSI CLS);                                                    //Clear the screen 
   printf(ESC CSI HME);                                                    //Set cursor to line 1; column 1
   puts("Time     Indice Open     High     Low      Close    Change   %Change");  
   puts("--------------------------------------------------------------------"); 
}

//--------------------------------------------------------------------
//Function to print the current indice data
//--------------------------------------------------------------------
void dspIndice(int idx, struct indice *pInd){
   char   *pEnd;                                                           //For strtod()                    
   double close;                                                           //Current close price
   double change;                                                          //Change up to the present close
   double pctchg;                                                          //Percent change
   
   //Set the cursor line nbr and add the character data to stdout
   printf(ESC CSI "%d" C01, HDRS + idx);                                   //Derive appropriate line # for this indice
   printf(ESC CSI EL);                                                     //Erase the line
   printf("%s %-6s %-8s %-8s %-8s ",pInd[idx].time,pInd[idx].ticker,
                      pInd[idx].open,pInd[idx].high,pInd[idx].low);
   
   //Convert closing price to binary and compute change from start of trading
   close = strtod(pInd[idx].close,&pEnd);
   change = strtod(pInd[idx].close,&pEnd) - pInd[idx].begopen;
   
   //Set background color to reflect the price change and print closing price
   if (close > pInd[idx].prvcls)
      printf(ESC CSI BKGREEN);
   else if (close < pInd[idx].prvcls)
      printf(ESC CSI BKRED);
   else 
      printf(ESC CSI BKWHITE);
      
   printf("%-8s",pInd[idx].close);                                         //Print closing price for current minute                              
   pInd[idx].prvcls = strtod(pInd[idx].close,&pEnd);                       //Save close for the next transaction 
   printf(ESC CSI BKDEFAULT);                                              //Restore background for remainder of the line
   
   //Set foreground color to reflect days change to present
   if (change > 0)
      printf(ESC CSI GREEN);
   else if (change < 0)
      printf(ESC CSI RED);
   else 
      printf(ESC CSI DEFAULT);
      
   printf(" %-8.2lf",change);
   
   //Compute/print the percent change from start of trading to present
   pctchg = (close - pInd[idx].begopen) / pInd[idx].begopen * 100;
   printf(" %.2lf%%\n",pctchg);
   
   //Reset the closing price background color
   usleep(300000);                                                          //100ms delay
   printf(ESC CSI "%d" C44, HDRS + idx);                                    //Column 44 of our current indicie line
   printf(ESC CSI DEFAULT);                                                 //Turn off background color
   printf("%-8s",pInd[idx].close);                                          //Reprint the closing price
   fflush(stdout);
}

//--------------------------------------------------------------------
//Parse financial indice data pulled from MQ
//--------------------------------------------------------------------
int parseCSV(char *pMsgBuf, struct indice *pInd){
   
   char *pDate, *pTime, *pTicker, *pOpen, *pHigh, *pLow, *pClose, *pEnd;
   int  idx;
   
   //Parse indice data from CSV line via strtok()
   pDate = strtok(pMsgBuf," ");
   pTime = strtok(NULL,","); 
   pTicker = strtok(NULL,","); 
   pOpen = strtok(NULL,",");
   pHigh = strtok(NULL,",");
   pLow = strtok(NULL,",");
   pClose = strtok(NULL,"\n");
   
   //Derive the indice structure array index - based on display order
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
   
   //(Re)Populate the appropriate Indice structure
   memcpy(pInd[idx].time,pTime,sizeof(pInd[idx].time));
   memcpy(pInd[idx].ticker,pTicker,sizeof(pInd[idx].ticker));
   memcpy(pInd[idx].open,pOpen,sizeof(pInd[idx].open));
   memcpy(pInd[idx].high,pHigh,sizeof(pInd[idx].high));
   memcpy(pInd[idx].low,pLow,sizeof(pInd[idx].low));
   memcpy(pInd[idx].close,pClose,sizeof(pInd[idx].close));

   //Set the begining open price for the 1st transaction of each indice
   if (pInd[idx].begopen == 0)
      pInd[idx].begopen = strtod(pOpen,&pEnd);
      
   return idx;
}