
#include <stdio.h>
#include <stdlib.h>
#include "m5op.h"

int main()
{ 
   int L =10;

   int long unsigned	i;					/* loop index */
   int					a=1;				/* variable with conditional assignment*/
   int long unsigned	liter = 10000000;	/* number of iterations */

   m5_reset_stats(0,0);
   for (i=0; i<liter; ++i){
      if ((i%2) == 0) 
      {
          if ((i%3) == 0) {
              if ((i%7) == 0){ 
                  a=0;	
              }
          }
      }
   }
}
