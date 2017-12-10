/********************************************************************************
* File:			Step8.c
* 
* Description:	This microbenchmark answers the question:
*				Is there a local component that uses at least n bits of local history? 
*				(if previous experiments detected a global component)
*				--not Taken/taken - surprise taken and not taken. rand() 
* Date:			April 2002
* 
* Author:		M.Milenkovic - milena@computer.org
* Last Modified: Vikram -vsm2@illinois.edu
**********************************************************************************/

//#define LSpy 4	/* pattern length */

#include <stdio.h>
#include <stdlib.h>
#include "m5op.h"

int main(int argc, const char *argv[])
{ 
   int LSpy =rand() % 20;
   if (argc !=1){
      printf("Define patterns length");
   }
   else{ 
       LSpy =  strtol(argv[1], NULL, 10);
   }
   
   int long unsigned	i;					/* loop index */
   int					a;					/* variable with conditional assignment*/
   int long unsigned	liter = 10000000;	/* number of iterations */
  
   m5_reset_stats(0,0);
   for(i=0; i< liter; i +=(rand() % 10)){
      /* 2*(L-1) dummy branches */
	  if (i<0) a=1;
	  /* spy bracnh */
	  if ((i%LSpy)==0) a=0;
   } 
   
   for (i=0; i<liter; ++i){
      
      /* 2*(L-1) dummy branches */
	  if (i<0) a=1;
	  /* spy bracnh */
	  if ((i%LSpy)==0) a=0;

  }
   return 0;
} 
