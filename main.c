/*
 * mqremoteget - Demo program to retrieve messages sent from a remote queue
 * 
 * Summary:
 * Console program to 
 * 
 * MQ Configuration:
 * +IBM MQ Advanced for Developers on ubuntu machine E6410
 *   -Queue Manager:
 *     -QM_E6410
 * 
 *   -Queues:
 *     -DEV.Q1 - Local queue
 * 
 *   -Channels:
 *     -DEV.APP.SVRCONN   - Server-connection
 *     -QM_S1558.QM_E6410 - Receiver
 * 
 * ----------------------------------------------------------------------------------------------
 * Date       Author        Description
 * ----------------------------------------------------------------------------------------------
 * 10/17/23   A. Hout       Original source
 * ----------------------------------------------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmqc.h>
#include "utils.h"

#define BUF_SIZE 64                                //mqget msg buffer size
//#define IND_SIZE 5                                //Number of arrays of indices structures

int main(int argc, char **argv)
{
	MQCNO   cnxOpt = {MQCNO_DEFAULT};               //Connection options  
   MQCSP   cnxSec = {MQCSP_DEFAULT};               //Security parameters
   MQOD    objDsc = {MQOD_DEFAULT};                //Object Descriptor
   MQMD    msgDsc = {MQMD_DEFAULT};                //Message Descriptor
   MQGMO   getOpt = {MQGMO_DEFAULT};               //Get message options
   
   //MQ handles and variables
   MQHCONN  Hcnx;                                  //Connection handle
   MQHOBJ   Hobj;                                  //Object handle 
   MQLONG   opnOpt;                                //MQOPEN options  
   MQLONG   clsOpt;                                //MQCLOSE options 
   MQLONG   cmpCde;                                //MQCONNX completion code 
   MQLONG   opnCde;                                //MQOPEN completion code 
   MQLONG   resCde;                                //Reason code  
   MQLONG   msglen;                                //Message length received
   
   //Connection literals/variables
   char *pQmg = "QM_E6410";                        //Target queue manager
   char *pQue = "DEV.Q1";                          //Target queue
   char uid[10];                                   //User ID
   char pwd[10];                                   //User password
   
   //Miscellaneous variables
   char msgbuf[BUF_SIZE];                          //Input message buffer
   FILE *pFP;                                      //File handle pointer
   struct indice indices[IND_SIZE];                //Array of indice structures
   
   //-------------------------------------------------------
   //Connect to the queue manager
   //-------------------------------------------------------
   cnxOpt.SecurityParmsPtr = &cnxSec;
   cnxOpt.Version = MQCNO_VERSION_5;
   cnxSec.AuthenticationType = MQCSP_AUTH_USER_ID_AND_PWD;
   
   pFP = fopen("/home/adam/mqusers","r");
   if (pFP == NULL){
	   fprintf(stderr, "fopen() failed in file %s at line # %d", __FILE__,__LINE__);
	   return EXIT_FAILURE;
	}
   
   int scnt = fscanf(pFP,"%s %s",uid,pwd);
	fclose(pFP);
   if (scnt < 2){
      puts("Error pulling user credentials");
      return EXIT_FAILURE;
   }
   
   cnxSec.CSPUserIdPtr = uid;                                            
   cnxSec.CSPUserIdLength = strlen(uid);
   cnxSec.CSPPasswordPtr = pwd;
   cnxSec.CSPPasswordLength = strlen(pwd);
   MQCONNX(pQmg,&cnxOpt,&Hcnx,&cmpCde,&resCde);                            //Queue manager = QM_E6410
   
   if (cmpCde == MQCC_FAILED){
      printf("MQCONNX failed with reason code %d\n",resCde);
      return (int)resCde;
   }
   
   if (cmpCde == MQCC_WARNING){
     printf("MQCONNX generated a warning with reason code %d\n",resCde);
     printf("Continuing...\n");
   }
   
   //-------------------------------------------------------
   //Open queue DEV.Q1 for input
   //-------------------------------------------------------
   opnOpt = MQOO_INPUT_AS_Q_DEF | MQOO_FAIL_IF_QUIESCING;
   strncpy(objDsc.ObjectName,pQue,strlen(pQue)+1);                                                   
   MQOPEN(Hcnx,&objDsc,opnOpt,&Hobj,&opnCde,&resCde);
          
   if (resCde != MQRC_NONE)
      printf("MQOPEN ended with reason code %d\n",resCde);

   if (opnCde == MQCC_FAILED){
      printf("unable to open %s queue for output\n",pQue);
      printf("Disconnecting from %s\n",pQmg);
      MQDISC(&Hcnx,&cmpCde,&resCde);
      return((int)opnCde);
   }
   
   //-------------------------------------------------------
   //
   //-------------------------------------------------------
   getOpt.Version = MQGMO_CURRENT_VERSION;                                    //Use the most recent version of MQGMO
   getOpt.MatchOptions = MQMO_NONE;                                           //FIFO retreival                                            
   getOpt.Options = MQGMO_WAIT | MQGMO_NO_SYNCPOINT;   
   getOpt.WaitInterval = 10000;                                               //10 second timeout
   memset(indices,0x00,sizeof(indices[IND_SIZE]));                            //Init array of indice structures
   
   do{
      msgDsc.Encoding = MQENC_NATIVE;                                         
      msgDsc.CodedCharSetId = MQCCSI_Q_MGR;
      MQGET(Hcnx,Hobj,&msgDsc,&getOpt,BUF_SIZE,msgbuf,&msglen,&cmpCde,&resCde);
      parseCSV(msgbuf,indices);
      //dspIndice();
   }while(cmpCde != MQCC_FAILED && resCde == MQRC_NONE);
   
   //-------------------------------------------------------
   //Close the queue connection
   //-------------------------------------------------------
   clsOpt = MQCO_NONE;
   MQCLOSE(Hcnx,&Hobj,clsOpt,&cmpCde,&resCde);

   if (resCde != MQRC_NONE)
      printf("MQCLOSE ended with reason code %d\n",resCde);
     
   //Disconnect from the queue manager
   MQDISC(&Hcnx,&cmpCde,&resCde);

   if (resCde != MQRC_NONE)
      printf("MQDISC ended with reason code %d\n",resCde);
      
   puts("Process complete");
   return EXIT_SUCCESS;
}
