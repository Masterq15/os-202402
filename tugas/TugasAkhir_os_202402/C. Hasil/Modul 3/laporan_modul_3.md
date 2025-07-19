# 📝 Laporan Tugas Akhir Sistem Operasi

**Mata Kuliah**: Sistem Operasi  
**Semester**: Genap / Tahun Ajaran 2024–2025  
**Nama**: Risky Dimas Nugroho
**NIM**: 240202882  
**Modul yang Dikerjakan**:  
Modul 5 – Manajemen Memori Lanjutan  
(A. Copy-on-Write Fork, B. Shared Memory ala System V)

---

## 📌 Deskripsi Singkat Tugas

Tugas ini merupakan bagian dari Modul 5, yang berfokus pada implementasi fitur manajemen memori tingkat lanjut pada sistem operasi xv6, meliputi:

- **A. Copy-on-Write (CoW) Fork**:  
  Mengoptimalkan `fork()` agar tidak langsung menggandakan seluruh memori anak, melainkan menyalin halaman hanya saat proses menulis ke halaman tersebut.

- **B. Shared Memory ala System V**:  
  Menyediakan fitur memori bersama antar proses menggunakan antarmuka mirip System V (`shmget`, `shmrelease`).

---

## 🛠️ A. Implementasi Copy-on-Write Fork (CoW)

### ✏️ Perubahan yang Dilakukan

- Menambahkan bit flag `PTE_COW` di `mmu.h`
- Membuat fungsi `cowuvm()` di `vm.c` untuk duplikasi page table CoW
- Menyesuaikan `fork()` di `proc.c` untuk menggunakan `cowuvm`
- Menangani page fault `T_PGFLT` di `trap.c` dengan memeriksa `PTE_COW`, mengalokasikan halaman baru jika perlu
- Mengatur referensi dengan `incref()` dan `decref()` di `kalloc.c`

### 🧪 Program Uji `cowtest.c`

```c
char *shm = (char*) shmget(42);
shm[0] = 'X';

if(fork() == 0){
  shm[0] = 'Y';
  printf(1, "Child sees: %c\n", shm[0]);
  exit();
} else {
  wait();
  printf(1, "Parent sees: %c\n", shm[0]);
}
```

### ✅ Output:
```
Child sees: Y
Parent sees: X
```
### 📸 Screenshot:
![hasil ptest](./screenshot/cowtest.png)

---

## 🛠️ B. Implementasi Shared Memory ala System V

### ✏️ Perubahan yang Dilakukan

- Menambahkan struktur `shmtab[]` di `vm.c`:
  ```c
  #define MAX_SHM 16
  struct {
    int key;
    char *frame;
    int refcount;
  } shmtab[MAX_SHM];
  ```

- Menambahkan dua system call:
  - `void* sys_shmget(int key)`
  - `int sys_shmrelease(int key)`

- Register syscall di:
  - `syscall.h`, `user.h`, `usys.S`, `syscall.c`

- Alamat shared memory dipetakan dari `USERTOP` ke bawah

### 🧪 Program Uji `shmtest.c`

```c
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
```

### ✅ Output:
```
Child reads: A
Parent reads: B
```
### 📸 Screenshot:
![hasil ptest](./screenshot/shmtest.png)

---

## ⚠️ Kendala yang Dihadapi

- CoW:
  - Penanganan TLB flush (`lcr3`) setelah duplikasi halaman
  - Validasi halaman `PTE_P | PTE_COW` saat page fault

- Shared Memory:
  - Alokasi frame tunggal yang dipetakan ulang di dua proses
  - Sinkronisasi alokasi memori dan pembebasan (`refcount`)
  - Penempatan mapping di `USERTOP` – (i+1) * `PGSIZE` untuk menghindari konflik

---

## 📚 Referensi

- [xv6-public - MIT](https://github.com/mit-pdos/xv6-public)
- [Buku xv6 (MIT 6.828)](https://pdos.csail.mit.edu/6.828/2018/xv6/book-rev11.pdf)
- Diskusi praktikum & Stack Overflow

---
