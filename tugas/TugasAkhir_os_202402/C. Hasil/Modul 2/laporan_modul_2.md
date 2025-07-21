# 📝 Laporan Tugas Akhir

**Mata Kuliah**: Sistem Operasi  
**Semester**: Genap / Tahun Ajaran 2024–2025  
**Nama**: Risky Dimas Nugroho  
**NIM**: 240202882  
**Modul yang Dikerjakan**: Modul 2 — Penjadwalan CPU Lanjutan (Priority Scheduling Non-Preemptive)

---

## 📌 Deskripsi Singkat Tugas

Pada modul ini, dilakukan modifikasi terhadap algoritma penjadwalan proses di sistem operasi xv6-public. Algoritma default Round Robin diubah menjadi Non-Preemptive Priority Scheduling. Modifikasi ini bertujuan untuk meningkatkan kontrol eksekusi proses berdasarkan prioritas numerik (semakin kecil nilai, semakin tinggi prioritasnya).

---

## 🛠️ Rincian Implementasi

Berikut adalah langkah-langkah implementasi yang dilakukan:

1. **Menambahkan Field `priority` pada struct proc**  
   - File: `proc.h`  
   - Deskripsi: Field `int priority;` ditambahkan untuk menyimpan nilai prioritas setiap proses.

2. **Inisialisasi Default Prioritas Proses Baru**  
   - File: `proc.c`, fungsi `allocproc()`  
   - Nilai default ditetapkan ke `60`.

3. **Menambahkan System Call Baru `set_priority(int)`**  
   - File yang diubah:  
     - `syscall.h` → Tambah `#define SYS_set_priority 24`  
     - `user.h` → Tambah `int set_priority(int);`  
     - `usys.S` → Tambah `SYSCALL(set_priority)`  
     - `syscall.c` → Daftarkan syscall pada array `syscalls[]`  
     - `sysproc.c` → Implementasi fungsi `sys_set_priority()` untuk menetapkan nilai prioritas proses pemanggil

4. **Modifikasi Scheduler**  
   - File: `proc.c`, fungsi `scheduler()`  
   - Logika dijalankan untuk memilih proses RUNNABLE dengan nilai prioritas terkecil  
   - Scheduler bersifat **non-preemptive** dan **deterministik**

5. **Program Uji `ptest.c`**  
   - Simulasi dua proses anak dengan prioritas berbeda:  
     - Child 1: Prioritas 90 (rendah)  
     - Child 2: Prioritas 10 (tinggi)  
   - Output menunjukkan Child 2 selesai lebih dulu

6. **Modifikasi Makefile**  
   - Menambahkan entri `_ptest` ke dalam bagian `UPROGS`

---

## ✅ Uji Fungsionalitas

Program pengujian:

- `ptest`: Untuk menguji fungsi `set_priority()` dan memastikan penjadwalan berdasarkan prioritas telah berjalan sesuai ekspektasi.

---

## 📷 Hasil Uji

### 📍 `ptest`:

```
Child 2 selesai
Child 1 selesai
Parent selesai
```

### 📸 Screenshot:
<img width="1901" height="672" alt="modul 2" src="https://github.com/user-attachments/assets/99321a8f-1055-4d97-8620-2df437551967" />

---

## ⚠️ Kendala yang Dihadapi

- Proses dengan prioritas sama tetap akan dipilih secara linear dari array proses, sehingga masih berpotensi tidak adil (no tie-breaking improvement).
- Jika semua proses memiliki prioritas sama dan tidak selesai, starvation dapat terjadi pada proses dengan prioritas lebih rendah.
- Tidak ada mekanisme aging yang diimplementasikan untuk meningkatkan fairness.


Output menunjukkan bahwa proses dengan prioritas lebih tinggi (Child 2) dieksekusi terlebih dahulu meskipun dibuat setelah proses lainnya.

---
## 📚 Referensi

* Buku xv6 MIT: [https://pdos.csail.mit.edu/6.828/2018/xv6/book-rev11.pdf](https://pdos.csail.mit.edu/6.828/2018/xv6/book-rev11.pdf)  
* Repositori xv6-public: [https://github.com/mit-pdos/xv6-public](https://github.com/mit-pdos/xv6-public)  
* Diskusi praktikum, GitHub Issues, Stack Overflow
  
---

