/* TournamentBP Killer Attempt 2
 * Andrew Pensinger
 */

#include "m5/m5op.h"

int main() {
  int a = 0;
  int stage;
  int i;
  
  m5_reset_stats(0, 0);

  for (stage = 0; stage <= 2; stage++) {
    for (i = 0; i <= 13; i++) {
      if ((i < 1) != (stage < 2)) {
	a++;
      }
      if ((i < 2) != (stage < 2)) {
	a++;
      }
      if ((i < 3) != (stage < 2)) {
	a++;
      }
      if ((i < 4) != (stage < 2)) {
	a++;
      }
      if ((i < 5) != (stage < 2)) {
	a++;
      }
      if ((i < 6) != (stage < 2)) {
	a++;
      }
      if ((i < 7) != (stage < 2)) {
	a++;
      }
      if ((i < 8) != (stage < 2)) {
	a++;
      }
      if ((i < 9) != (stage < 2)) {
	a++;
      }
      if ((i < 10) != (stage < 2)) {
	a++;
      }
      if ((i < 11) != (stage < 2)) {
	a++;
      }
      if ((i < 12) != (stage < 2)) {
	a++;
      }
      if ((i < 13) != (stage < 2)) {
	a++;
      }
    }
  }
  return 0;
}
