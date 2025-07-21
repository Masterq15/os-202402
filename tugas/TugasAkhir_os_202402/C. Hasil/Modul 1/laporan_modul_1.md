# 📝 Laporan Tugas Akhir

**Mata Kuliah**: Sistem Operasi  
**Semester**: Genap / Tahun Ajaran 2024–2025  
**Nama**: Risky Dimas Nugroho  
**NIM**: 240202882  
**Modul yang Dikerjakan**: Modul 1 – *System Call dan Instrumentasi Kernel*

---

## 📌 Deskripsi Singkat Tugas

Pada Modul 1 ini, saya melakukan pengembangan terhadap kernel xv6 dengan menambahkan dua system call baru:

1. `getpinfo()` untuk menampilkan informasi proses-proses aktif (PID, ukuran memori, dan nama proses).
2. `getReadCount()` untuk menghitung total jumlah pemanggilan fungsi `read()` sejak sistem dinyalakan (boot).

Tujuan dari tugas ini adalah untuk memperdalam pemahaman terkait mekanisme *system call* dan bagaimana kernel dapat diinstrumentasi untuk melakukan monitoring sistem secara real-time.

---

## 🛠️ Rincian Implementasi

Langkah-langkah implementasi yang dilakukan:

- Menambahkan struktur `struct pinfo` di file `proc.h` untuk menyimpan informasi proses.
- Menambahkan variabel global `readcount` di `sysproc.c` untuk menghitung jumlah pemanggilan `read()`.
- Menambahkan nomor syscall baru `SYS_getpinfo` dan `SYS_getreadcount` di `syscall.h`.
- Menambahkan deklarasi syscall pada `user.h` dan `usys.S`.
- Mendaftarkan fungsi syscall baru di `syscall.c`.
- Mengimplementasikan dua fungsi syscall baru (`sys_getpinfo` dan `sys_getreadcount`) di `sysproc.c`.
- Memodifikasi fungsi `sys_read` di `sysfile.c` untuk menambahkan counter `readcount++`.
- Membuat dua program uji user-level (`ptest.c` dan `rtest.c`) serta mendaftarkannya di `Makefile`.

---

## ✅ Uji Fungsionalitas

Program pengujian yang digunakan:

- `ptest`: untuk menguji system call `getpinfo()`
- `rtest`: untuk menguji system call `getReadCount()`

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


<img width="1502" height="800" alt="modul 1" src="https://github.com/user-attachments/assets/acd046b2-7388-4bd2-adfd-ddc35fc65bd6" />


```

## ⚠️ Kendala yang Dihadapi

* Menangani pointer dari user space menggunakan `argptr()`
* Sinkronisasi akses ke `ptable` agar tidak terjadi race condition
* Kesalahan umum seperti:
  - Salah akses pointer (`.` vs `->`)
  - Lupa meng-include `spinlock.h`
  - Gagal membaca hasil syscall karena kesalahan definisi argumen

---

## 📚 Referensi

* Buku xv6 MIT: [https://pdos.csail.mit.edu/6.828/2018/xv6/book-rev11.pdf](https://pdos.csail.mit.edu/6.828/2018/xv6/book-rev11.pdf)
* Repositori xv6-public: [https://github.com/mit-pdos/xv6-public](https://github.com/mit-pdos/xv6-public)
* Diskusi praktikum dan dokumentasi di Stack Overflow dan GitHub Issues
