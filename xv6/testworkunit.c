#include "types.h"
#include "stat.h"
#include "user.h"

volatile int sink = 0;

void
burn(void)
{
  int i;
  for (i = 0; i < 2000000000; i++) {
    sink += i;
  }
}

int
main(void)
{
  int pid1, pid2, pid3;
  int start, end1 = 0, end2 = 0, end3 = 0;
  int pid;

  printf(1, "Lottery test: 3 children with tickets 1, 2, 3\n");

  start = uptime();

  pid1 = fork();
  if (pid1 == 0) {
    if (settickets(1) < 0) {
      printf(1, "child1: settickets failed\n");
      exit();
    }
    burn();
    exit();
  }

  pid2 = fork();
  if (pid2 == 0) {
    if (settickets(2) < 0) {
      printf(1, "child2: settickets failed\n");
      exit();
    }
    burn();
    exit();
  }

  pid3 = fork();
  if (pid3 == 0) {
    if (settickets(3) < 0) {
      printf(1, "child3: settickets failed\n");
      exit();
    }
    burn();
    exit();
  }

  // Wait for children and record finish times
  while ((pid = wait()) > 0) {
    int now = uptime();
    if (pid == pid1) end1 = now;
    else if (pid == pid2) end2 = now;
    else if (pid == pid3) end3 = now;
  }

  // Print elapsed ticks (approximate, since children started slightly after 'start')
  printf(1, "Child 1 (tickets=1) finished at %d ticks (elapsed %d)\n", end1, end1 - start);
  printf(1, "Child 2 (tickets=2) finished at %d ticks (elapsed %d)\n", end2, end2 - start);
  printf(1, "Child 3 (tickets=3) finished at %d ticks (elapsed %d)\n", end3, end3 - start);

  // Compute ratios: if tickets are proportional to CPU share,
  // elapsed times should be inversely proportional.
  if (end2 - start > 0 && end3 - start > 0) {
    printf(1, "Ratio of elapsed times (1/2): %d / %d = %d.%d\n",
           end1 - start, end2 - start, (end1 - start) / (end2 - start),
           ((end1 - start) * 10 / (end2 - start)) % 10);
    printf(1, "Ratio of elapsed times (1/3): %d / %d = %d.%d\n",
           end1 - start, end3 - start, (end1 - start) / (end3 - start),
           ((end1 - start) * 10 / (end3 - start)) % 10);
    printf(1, "Ratio of elapsed times (2/3): %d / %d = %d.%d\n",
           end2 - start, end3 - start, (end2 - start) / (end3 - start),
           ((end2 - start) * 10 / (end3 - start)) % 10);
  }

  exit();
}