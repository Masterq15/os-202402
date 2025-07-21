// random.c
#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "fs.h"

// Generator acak sederhana
static uint seed = 123456;

int randomread(struct inode *ip, char *dst, int n) {
  for(int i = 0; i < n; i++){
    seed = seed * 1664525 + 1013904223;
    dst[i] = seed & 0xFF;
  }
  return n;
}