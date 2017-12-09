/********************************************************************************
* File:			Step3.c
* 
* Description:	This microbenchmark answers the question:
*				Is there a global component that uses 
*				at least 2 bits of global history?
*				(if previous steps detected a local component)
*
* Date:			April 2002
* 
* Author:		M.Milenkovic - milena@computer.org
* Last Modified: Vikram -vsm2@illinois.edu
**********************************************************************************/

//#define L1 5	/* pattern length */
//#define L2 2	/* pattern length */

#include <stdio.h>
#include <stdlib.h>
#include "m5op.h"

int main(int argc, const char *argv[])
{ 
   int L1 =5;
   int L2 =2;
   if (argc <2){
      printf("Define L1 and L2 patterns length");
   }
   else{ 
       L1 =  strtol(argv[1], NULL, 10);
       L2 =  strtol(argv[1], NULL, 10);
   }
   
   int long unsigned	i;					/* loop index */
   int					a,b,c;				/* variables with conditional assignment*/
   int long unsigned	liter = 10000000;	/* number of iterations */
   
   m5_reset_stats(0,0);
   for (i=1;i<=liter;++i)
   { 
	   if ((i%L1) == 0) a=1;
       else a=0;
       
	   if ((i%L2) == 0) b=1;
       else b=0;
    
	   if ( (a*b) == 1) c=1; /* spy branch */
   }
return 0;
} 
