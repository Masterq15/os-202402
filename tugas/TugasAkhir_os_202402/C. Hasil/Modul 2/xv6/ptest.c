#include "types.h"
#include "stat.h"
#include "user.h"

void busy() {
  for (volatile int i = 0; i < 100000000; i++);
}

int main() {
  int pid1 = fork();
  if (pid1 == 0) {
    set_priority(90); // prioritas rendah
    sleep(1);         // biar child2 jalan dulu
    busy();
    printf(1, "Child 1 selesai\n");
    exit();
  }

  int pid2 = fork();
  if (pid2 == 0) {
    set_priority(10); // prioritas tinggi
    busy();
    printf(1, "Child 2 selesai\n");
    exit();
  }

  wait(); wait();
  printf(1, "Parent selesai\n");
  exit();
}
