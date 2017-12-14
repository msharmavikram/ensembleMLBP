/* TournamentBP Killer Attempt 1
 * Andrew Pensinger
 */

#include "m5/m5op.h"

int main() {
  int initial[] = { 2, 1, 2, 1, 2, 1,
		    2, 1, 2, 1, 2, 1,
		    2, 1, 2, 1, 2, 1,
		    2, 1, 2, 1, 2, 1,
		    2, 1, 2, 1, 2, 1,
		    2, 1, 2, 1, 2, 1
  };
  int pi[] = { 3, 1, 4, 1, 5, 9,
	       2, 6, 5, 3, 5, 8,
	       9, 7, 9, 3, 2, 3,
	       8, 4, 6, 2, 6, 4,
	       3, 3, 8, 3, 2, 7,
	       9, 5, 0, 2, 8, 8
  };
  
  int (*values)[36] = &initial;
  int initializing = 1;
  int dummy = 1;
  int i;

  m5_reset_stats(0, 0);

 main_loop:
  for (i = 0; i < 36; i++) {
    if ((*values)[i] % 2 == 0) {
      dummy = 0;
    } else {
      dummy = 1;
    }
  }

  if (initializing) {
    values = &pi;
    initializing = 0;
    goto main_loop;
  }
  
  return 0;
}
