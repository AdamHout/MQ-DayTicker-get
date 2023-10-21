#ifndef UTILS_H
#define UTILS_H

#define IND_SIZE 5                                  //Number of indices structure arrays

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


//Function to parse csv data lines into a structure
void parseCSV(char *, struct indice *);



#endif //CSVUTIL_H