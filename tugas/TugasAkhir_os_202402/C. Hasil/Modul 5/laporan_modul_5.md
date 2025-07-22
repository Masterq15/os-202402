# 📝 Laporan Tugas Akhir

**Mata Kuliah**: Sistem Operasi  
**Semester**: Genap / Tahun Ajaran 2024–2025  
**Nama**: Risky Dimas Nugroho                     
**NIM**: 240202882  
**Modul yang Dikerjakan**:  
Modul 5 – Audit dan Keamanan Sistem  

---

## 📌 Deskripsi Singkat Tugas

* **Modul 5 – Audit dan Keamanan Sistem**:  
  Pada Modul 5 ini, saya mengimplementasikan sistem audit log untuk sistem operasi xv6. Tujuan utamanya adalah mencatat setiap system call yang dipanggil oleh proses dan menyediakan mekanisme akses log melalui system call get_audit_log(), dengan pembatasan akses hanya untuk proses dengan PID 1 (init process). Hal ini penting untuk mendukung prinsip keamanan dan akuntabilitas sistem operasi.

---

## 🛠️ Rincian Implementasi

* Menambahkan struktur audit_entry untuk menyimpan log system call
* Mengedit syscall() di syscall.c untuk mencatat semua system call ke log
* Menambahkan system call baru get_audit_log() untuk membaca isi log (khusus PID 1)
* Melakukan perubahan pada file:
  * syscall.c, sysproc.c, user.h, usys.S, defs.h, syscall.h
* Membuat program uji baru audit.c untuk menampilkan log
* Menambahkan entri _audit di Makefile agar bisa dibangun bersama xv6

---

## ✅ Uji Fungsionalitas

Program uji:
* audit: Untuk menguji system call get_audit_log() dan melihat apakah log tercatat dengan benar

Pengujian dilakukan dalam dua kondisi:
* Saat dijalankan sebagai proses biasa (akan ditolak)
* Saat dijalankan sebagai PID 1 (dapat mengakses log)
  
```
## 📷 Hasil Uji

### 📍 Output `audit`:
```
=== Audit Log ===

[0] PID=1 SYSCALL=7 TICK=1
[1] PID=1 SYSCALL=15 TICK=2
[2] PID=1 SYSCALL=17 TICK=2
```
...



### 📸 Screenshot:
<img width="1255" height="584" alt="Screenshot 2025-07-19 031006" src="https://github.com/user-attachments/assets/b0083abe-4118-4db0-8a50-4ab7ccc4cea4" />


---


## ⚠️ Kendala yang Dihadapi

* Awalnya system call get_audit_log() dapat dipanggil oleh proses mana pun karena belum ada validasi PID → diselesaikan dengan menambahkan pengecekan proc->pid != 1 di implementasi syscall.
* Salah pemetaan buffer audit antara kernel-space ke user-space → diatasi dengan argptr() dan memmove() secara hati-hati.
* Terbatasnya jumlah log (maksimum 128) mengharuskan pembatasan jumlah data yang disalin ke user agar tidak buffer overflow.

---

## 📚 Referensi

* Buku xv6 MIT: [https://pdos.csail.mit.edu/6.828/2018/xv6/book-rev11.pdf](https://pdos.csail.mit.edu/6.828/2018/xv6/book-rev11.pdf)  
* Repositori xv6-public: [https://github.com/mit-pdos/xv6-public](https://github.com/mit-pdos/xv6-public)  
* Diskusi praktikum, GitHub Issues, Stack Overflow

