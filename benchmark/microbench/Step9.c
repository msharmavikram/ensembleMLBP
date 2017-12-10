/********************************************************************************
* File:			Step5.c
* 
* Description:	This microbenchmark answers the question:
*				0 or 1 bit in global history register?
*				(if previous experiments detected a local component, 
*				 and no global component with >=2 history bits) 
*               --nested if 
* Date:			April 2002
* 
* Author:		M.Milenkovic - milena@computer.org
* Last Modified: Vikram -vsm2@illinois.edu
**********************************************************************************/

//#define L3 6	/* pattern length */
#include <stdio.h>
#include <stdlib.h>
#include "m5op.h"

int main(int argc, const char *argv[])
{ 
   int L3=6;
   if (argc !=1){
      printf("Define patterns length");
   }
   else{ 
       L3=  strtol(argv[1], NULL, 10);
   }

   int long unsigned	i;					/* loop index */
   int					a;					/* variable with conditional assignment*/
   int long unsigned	liter = 10000000;	/* number of iterations */
   
   m5_reset_stats(0,0);
   for (i=1;i<=liter;++i)
   { 
      if ((i%L3) == 0) 
          a=1;		/* L3 > L */
      else if ((rand()%i==0)) 
          a=1; 
      else
          a=0;

      if ((i%L3) == 0) a=1;		/* spy branch */
   
      switch(i%L3){
          case 0: 
                  a =1; 
                  break;
          case 1: 
                  a =2; 
                  break;
          default: 
                  a =0;
      }
   }
   return 0;
} 
