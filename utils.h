/*
 * ----------------------------------------------------------------------------------------------
 * Date       Author        Description
 * ----------------------------------------------------------------------------------------------
 * 10/17/23   A. Hout       Original source
 * ----------------------------------------------------------------------------------------------
*/

#ifndef UTILS_H
#define UTILS_H

#define NBR_IND 5                                   //Number of indices structure arrays
#define HDRS     3                                  //Number of header lines + 1

//ANSI escape sequence macros
#define ESC       "\033"                            //ASCII escape code - octal
#define CSI       "["                               //Control sequence introducer   
#define CLS       "2J"                              //Clear the screen 
#define EL        "K"                               //Erase line from cursor position
#define HME       "H"                               //Set cursor to home position
#define C01       ";1H"                             //2nd half of cursor position sequence. Line # will be set dynamically
#define C44       ";44H"                            //2nd half of cursor position sequence. Line # will be set dynamically
//Escape sequence foreground colors
#define RED       "31m"
#define GREEN     "32m"
#define DEFAULT   "39m"
//Escape sequence background colors
#define BKRED     "41m"
#define BKGREEN   "42m"
#define BKWHITE   "47m"
#define BKDEFAULT "49m"

//Structure to store csv's from the Indices dataset 
struct indice{
   char   time[9];                                  //Post time
   char   ticker[4];                                //Indice name
   char   open[10];                                 //Open price for this minute
   char   high[10];                                 //High price during this minute
   char   low[10];                                  //Low price during this minute
   char   close[10];                                //Closing price for this minute
   double prvcls;                                   //Previous minutes closing price
   double begopen;                                  //Open price for the day                                  
};


//-----------------------------------------------------------------
//Initialize the console screen and print display headers
//
//Input:   None
//Returns: None
//-----------------------------------------------------------------
void dspHdr(void);

//-----------------------------------------------------------------
//Update the display with a indice data line pulled via parseSCV()
//
//Input:   Indice structure array index Indice structure pointer
//         Indice structure pointer
//
//Returns: None 
//-----------------------------------------------------------------
void dspIndice(int, struct indice *);

//-----------------------------------------------------------------
//Parse csv data lines into an instance of the indice structure
//
//Input:  CSV data line in MQ message buffer
//        Array of indice structures
//
//Output: Indice structure array index
//-----------------------------------------------------------------
int parseCSV(char *, struct indice *);

#endif //UTILS_H