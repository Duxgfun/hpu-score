# HỆ THỐNG QUẢN LÝ ĐIỂM SINH VIÊN (HPU-SCORE)
**Học phần:** Nhập môn lập trình  
**Đề tài:** ĐỀ 01 - Quản lý điểm sinh viên của lớp học  
**Đối tượng:** Cố vấn học tập - Cô Lan (Lớp CNTT-K18, ~40 sinh viên)  
**Ngôn ngữ:** C thuần chuẩn (Chuẩn C99 / C11)  
**Tác giả:** Nhóm sinh viên CNTT-K18  

Dự án cung cấp **2 phiên bản hoàn chỉnh**:
1. **Phiên bản Giao diện Đồ họa Desktop (Win32 Native GUI):** [gui_main.c](file:///c:/Users/zippo/Documents/HocTap_2026/NhapMonLapTrinh/BaiKiemTra/Project%20HPU-Score/gui_main.c) $\to$ `gui.exe`
2. **Phiên bản Dòng lệnh (Console CLI):** [main.c](file:///c:/Users/zippo/Documents/HocTap_2026/NhapMonLapTrinh/BaiKiemTra/Project%20HPU-Score/main.c) $\to$ `main.exe`

---

## 1. Hướng dẫn biên dịch và khởi chạy

### Cách 1: Chạy trực tiếp file thực thi đã biên dịch sẵn
Trong thư mục dự án:
- **Khởi chạy Giao diện Desktop GUI:** Nhấp đúp chuột vào file `gui.exe` (hoặc gõ `.\gui.exe` trong terminal).
- **Khởi chạy Giao diện dòng lệnh Console:** Gõ `.\main.exe` trong terminal.

### Cách 2: Tự biên dịch lại bằng Clang / GCC trên Windows
```powershell
# Biên dịch phiên bản GUI Desktop (Win32 Native GUI)
clang -Wall -Wextra -std=c99 -mwindows -o gui.exe gui_main.c -lcomctl32
# (Hoặc dùng gcc: gcc -Wall -Wextra -std=c99 -mwindows -o gui.exe gui_main.c -lcomctl32)

# Biên dịch phiên bản Console CLI
clang -Wall -Wextra -std=c99 -o main.exe main.c
# (Hoặc dùng gcc: gcc -Wall -Wextra -std=c99 -o main.exe main.c -lm)
```

### Cách 3: Biên dịch trên Code::Blocks hoặc Dev-C++
- Với `main.c`: Tạo New Console Application (ngôn ngữ C), dán code và nhấn **F9**.
- Với `gui_main.c`: Tạo New Win32 GUI Application, thêm link library `-lcomctl32` trong Project Build Options $\to$ Linker Settings, dán code và nhấn **F9**.
