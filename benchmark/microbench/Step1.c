/********************************************************************************
* File:			Step1.c
* 
* Description:	This microbenchmark answers the question:
*				What is maximum length of the "spy" branch pattern 
*				that would be correctly predicted when the spy branch 
*				is the only branch in a loop? 
*
* Date:			April 2002
* 
* Author:		M.Milenkovic - milena@computer.org
* Last Modified: Vikram -vsm2@illinois.edu
*
 **********************************************************************************/

//#define L 10								/* pattern length */
#include <stdio.h>
#include <stdlib.h>
#include "m5op.h"

int main(int argc, const char *argv[])
{ 
   int L =10;
   if (argc !=1){
      printf("Define patterns length");
   }
   else{ 
       L =  strtol(argv[1], NULL, 10);
   }

   int long unsigned	i;					/* loop index */
   int					a=1;				/* variable with conditional assignment*/
   int long unsigned	liter = 10000000;	/* number of iterations */

   m5_reset_stats(0,0);
   for (i=0; i<liter; ++i){
      if ((i%L) == 0) a=0;					/* spy branch */
   }
   return 0;
}
