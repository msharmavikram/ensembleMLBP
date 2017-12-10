/********************************************************************************
* File:			Step2.c
* 
* Description:	This microbenchmark answers the question:
*				Are there (L - 1) bits of local component or 
*				(2*L - 1) bits of global component?
*
* Date:			April 2002
* 
* Author:		M.Milenkovic - milena@computer.org
* Last Modified: Vikram -vsm2@illinois.edu
**********************************************************************************/

//#define L 9		/* pattern length */
#include <stdio.h>
#include <stdlib.h>
#include "m5op.h"

int main(int argc, const char *argv[])
{ 
   int L =9;
   if (argc !=1){
      printf("Define patterns length");
   }
   else{ 
       L =  strtol(argv[1], NULL, 10);
   }
   
   int long unsigned	i;					/* loop index */
   int					a=1;				/* variable with conditional assignment*/
   int long unsigned	liter = 10000000;	/* number of iterations */	
   for (i=0; i<liter; ++i){
      
	  /* 2*(L-1) dummy branches */
	  
   m5_reset_stats(0,0);
	  if (i<0) a=1; 
	  if (i<0) a=1;
	  if (i<0) a=1;
	  if (i<0) a=1;
	  if (i<0) a=1;
	  if (i<0) a=1;
	  if (i<0) a=1;
	  if (i<0) a=1;
	  if (i<0) a=1; 
	  if (i<0) a=1;
	  if (i<0) a=1;
	  if (i<0) a=1;
	  if (i<0) a=1;
	  if (i<0) a=1;
	  if (i<0) a=1;
	  if (i<0) a=1;

	  /* spy branch */

      if ((i%L)==0) a=0;
   }
return 0;
}
