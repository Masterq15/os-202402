# 📝 Laporan Tugas Akhir



**Mata Kuliah**: Sistem Operasi

**Semester**: Genap / Tahun Ajaran 2024–2025

**Nama**: `<Risky Dimas Nugroho>`

**NIM**: `<240202882>`

**Modul yang Dikerjakan**:

`(Contoh: Modul 1 – System Call dan Instrumentasi Kernel)`



---



## 📌 Deskripsi Singkat Tugas



Tuliskan deskripsi singkat dari modul yang Anda kerjakan. Misalnya:



* **Modul 1 – System Call dan Instrumentasi Kernel**:

  Menambahkan dua system call baru, yaitu `getpinfo()` untuk melihat proses yang aktif dan `getReadCount()` untuk menghitung jumlah pemanggilan `read()` sejak boot.

---



## 🛠️ Rincian Implementasi



Tuliskan secara ringkas namun jelas apa yang Anda lakukan:



### Contoh untuk Modul 1:



* Menambahkan dua system call baru di file `sysproc.c` dan `syscall.c`

* Mengedit `user.h`, `usys.S`, dan `syscall.h` untuk mendaftarkan syscall

* Menambahkan struktur `struct pinfo` di `proc.h`

* Menambahkan counter `readcount` di kernel

* Membuat dua program uji: `ptest.c` dan `rtest.c`

---



## ✅ Uji Fungsionalitas



Tuliskan program uji apa saja yang Anda gunakan, misalnya:



* `ptest`: untuk menguji `getpinfo()`

* `rtest`: untuk menguji `getReadCount()`

* `cowtest`: untuk menguji fork dengan Copy-on-Write

* `shmtest`: untuk menguji `shmget()` dan `shmrelease()`

* `chmodtest`: untuk memastikan file `read-only` tidak bisa ditulis

* `audit`: untuk melihat isi log system call (jika dijalankan oleh PID 1)



---



## 📷 Hasil Uji



Lampirkan hasil uji berupa screenshot atau output terminal. Contoh:



### 📍 Contoh Output `cowtest`:



```

Child sees: Y

Parent sees: X

```



### 📍 Contoh Output `shmtest`:



```

Child reads: A

Parent reads: B

```



### 📍 Contoh Output `chmodtest`:



```

Write blocked as expected

```



Jika ada screenshot:



```

![hasil cowtest](./screenshots/cowtest_output.png)

```



---



## ⚠️ Kendala yang Dihadapi



Tuliskan kendala (jika ada), misalnya:



* Salah implementasi `page fault` menyebabkan panic

* Salah memetakan alamat shared memory ke USERTOP

* Proses biasa bisa akses audit log (belum ada validasi PID)



---



## 📚 Referensi



Tuliskan sumber referensi yang Anda gunakan, misalnya:



* Buku xv6 MIT: [https://pdos.csail.mit.edu/6.828/2018/xv6/book-rev11.pdf](https://pdos.csail.mit.edu/6.828/2018/xv6/book-rev11.pdf)

* Repositori xv6-public: [https://github.com/mit-pdos/xv6-public](https://github.com/mit-pdos/xv6-public)

* Stack Overflow, GitHub Issues, diskusi praktikum



---# 🧪 Modul 3 — Manajemen Memori Tingkat Lanjut (xv6-public x86)



## 🎯 Tujuan



1. **Copy-on-Write (CoW) Fork:**



   * Fork tidak lagi menyalin semua memori secara langsung.

   * Halaman berbagi read-only; salin dilakukan hanya saat write (via page fault).

   * Gunakan **reference count** untuk manajemen memori fisik.



2. **Shared Memory ala System V:**



   * `void* shmget(int key)`

   * `int shmrelease(int key)`

   * Memungkinkan antar proses berbagi 1 halaman dengan sistem refcount.



---



## 🗂️ Daftar File yang Dimodifikasi



| File                                         | Keterangan                                        |

| -------------------------------------------- | ------------------------------------------------- |

| `vm.c`                                       | Tambah refcount array, modifikasi `copyuvm` → CoW |

| `trap.c`                                     | Tangani page fault untuk salin CoW saat write     |

| `proc.c`                                     | Ubah `fork()` untuk gunakan `cowuvm()`            |

| `defs.h`                                     | Tambah fungsi `incref`, `decref`                  |

| `mmu.h`                                      | Tambah flag `PTE_COW`                             |

| `sysproc.c`                                  | Tambah syscall `shmget`, `shmrelease`             |

| `user.h`, `usys.S`, `syscall.c`, `syscall.h` | Registrasi syscall                                |



---



## 🔧 A. Implementasi Copy-on-Write Fork (CoW)



---



### 1. Tambah `ref_count[]` dan fungsi `incref/decref`



Di **`vm.c`**, bagian global:



```c

#define PHYSTOP 0xE000000   // default xv6

#define NPAGE (PHYSTOP >> 12)



int ref_count[NPAGE];  // satu counter per physical page



void incref(char *pa) {

  ref_count[V2P(pa) >> 12]++;

}



void decref(char *pa) {

  int idx = V2P(pa) >> 12;

  if (--ref_count[idx] == 0)

    kfree(pa);

}

```



> ⚠️ Pastikan semua `kalloc()` → `incref()` dan `kfree()` → `decref()` dipanggil secara konsisten nanti.



---



### 2. Tambahkan `PTE_COW` di `mmu.h`



```c

#define PTE_COW 0x200  // custom flag untuk CoW

```



---



### 3. Modifikasi `copyuvm()` → `cowuvm()` di `vm.c`



```c

pde_t* cowuvm(pde_t *pgdir, uint sz) {

  pde_t *d = setupkvm();

  if(d == 0)

    return 0;



  for(uint i = 0; i < sz; i += PGSIZE){

    pte_t *pte = walkpgdir(pgdir, (void*)i, 0);

    if(!pte || !(*pte & PTE_P))

      continue;



    uint pa = PTE_ADDR(*pte);

    char *v = P2V(pa);

    uint flags = PTE_FLAGS(*pte);



    // Hilangkan flag tulis, tambahkan COW

    flags &= ~PTE_W;

    flags |= PTE_COW;



    // Tambah refcount

    incref(v);



    if(mappages(d, (void*)i, PGSIZE, pa, flags) < 0){

      freevm(d);

      return 0;

    }



    *pte = (*pte & ~PTE_W) | PTE_COW;

  }

  return d;

}

```



---



### 4. Ubah `fork()` di `proc.c`



Cari `np->pgdir = copyuvm(...)` → ganti:



```c

np->pgdir = cowuvm(curproc->pgdir, curproc->sz);

```



---



### 5. Tangani Page Fault `T_PGFLT` di `trap.c`



Di dalam `trap()` → tambahkan:



```c

if(r->trapno == T_PGFLT){

  uint va = rcr2();

  pte_t *pte = walkpgdir(proc->pgdir, (void*)va, 0);



  if(!pte || !(*pte & PTE_P) || !(*pte & PTE_COW)){

    cprintf("Invalid page fault at %x\n", va);

    kill(proc->pid);

    return;

  }



  uint pa = PTE_ADDR(*pte);

  char *mem = kalloc();

  if(mem == 0){

    cprintf("kalloc failed\n");

    kill(proc->pid);

    return;

  }



  memmove(mem, (char*)P2V(pa), PGSIZE);

  decref((char*)P2V(pa)); // free lama



  *pte = V2P(mem) | PTE_W | PTE_U | PTE_P;

  *pte &= ~PTE_COW;



  lcr3(V2P(proc->pgdir));

  return;

}

```



---



## 🧪 Program Uji `cowtest.c`



```c

#include "types.h"

#include "stat.h"

#include "user.h"



int main() {

  char *p = sbrk(4096); // alokasikan mem

  p[0] = 'X';



  int pid = fork();

  if(pid == 0){

    p[0] = 'Y';

    printf(1, "Child sees: %c\n", p[0]);

    exit();

  } else {

    wait();

    printf(1, "Parent sees: %c\n", p[0]);

  }

  exit();

}

```



**Output:**



```

Child sees: Y

Parent sees: X

```



---



## 🔁 B. Implementasi Shared Memory ala System V



---



### 1. Tambah Struktur di `vm.c`



```c

#define MAX_SHM 16



struct {

  int key;

  char *frame;

  int refcount;

} shmtab[MAX_SHM];

```



---



### 2. Implementasi `shmget()` di `sysproc.c`



```c

void* sys_shmget(void) {

  int key;

  if(argint(0, &key) < 0)

    return (void*)-1;



  for(int i = 0; i < MAX_SHM; i++){

    if(shmtab[i].key == key){

      shmtab[i].refcount++;

      mappages(proc->pgdir, (char*)(USERTOP - (i+1)*PGSIZE), PGSIZE,

               V2P(shmtab[i].frame), PTE_W|PTE_U);

      return (void*)(USERTOP - (i+1)*PGSIZE);

    }

  }



  for(int i = 0; i < MAX_SHM; i++){

    if(shmtab[i].key == 0){

      shmtab[i].key = key;

      shmtab[i].frame = kalloc();

      shmtab[i].refcount = 1;

      memset(shmtab[i].frame, 0, PGSIZE);

      mappages(proc->pgdir, (char*)(USERTOP - (i+1)*PGSIZE), PGSIZE,

               V2P(shmtab[i].frame), PTE_W|PTE_U);

      return (void*)(USERTOP - (i+1)*PGSIZE);

    }

  }



  return (void*)-1;

}

```



---



### 3. Implementasi `shmrelease()` di `sysproc.c`



```c

int sys_shmrelease(void) {

  int key;

  if(argint(0, &key) < 0)

    return -1;



  for(int i = 0; i < MAX_SHM; i++){

    if(shmtab[i].key == key){

      shmtab[i].refcount--;

      if(shmtab[i].refcount == 0){

        kfree(shmtab[i].frame);

        shmtab[i].key = 0;

      }

      return 0;

    }

  }

  return -1;

}

```



---



### 4. Registrasi syscall



#### Di `syscall.h`:



```c

#define SYS_shmget 25

#define SYS_shmrelease 26

```



#### Di `user.h`:



```c

void* shmget(int);

int shmrelease(int);

```



#### Di `usys.S`:



```asm

SYSCALL(shmget)

SYSCALL(shmrelease)

```



#### Di `syscall.c`:



```c

extern int sys_shmget(void);

extern int sys_shmrelease(void);

...

[SYS_shmget]     sys_shmget,

[SYS_shmrelease] sys_shmrelease,

```



---



## 🧪 Program Uji `shmtest.c`



```c

#include "types.h"

#include "stat.h"

#include "user.h"



int main() {

  char *shm = (char*) shmget(42);

  shm[0] = 'A';



  if(fork() == 0){

    char *shm2 = (char*) shmget(42);

    printf(1, "Child reads: %c\n", shm2[0]);

    shm2[1] = 'B';

    shmrelease(42);

    exit();

  } else {

    wait();

    printf(1, "Parent reads: %c\n", shm[1]);

    shmrelease(42);

  }

  exit();

}

```



**Output:**



```

Child reads: A

Parent reads: B

```



---



## 📚 Referensi



* [MIT xv6 Book (x86)](https://pdos.csail.mit.edu/6.828/2018/xv6/book-rev11.pdf)

* `vm.c`, `proc.c`, `trap.c` dalam xv6-public



---



## ✅ Kesimpulan



Dengan modul ini, Anda:



* Mengimplementasikan **Copy-on-Write Fork** untuk efisiensi `fork()`

* Mengembangkan **Shared Memory antar proses** seperti `System V`

* Menguasai teknik **page fault handling** dan **reference counting**



---
