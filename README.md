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

---

## 2. Các tính năng nổi bật của phiên bản GUI (`gui.exe`)
- **Cửa sổ ứng dụng Desktop thật:** Hiển thị trực quan với font chữ Segoe UI ClearType sắc nét.
- **Bảng dữ liệu ListView 8 cột:** Hiển thị đầy đủ STT, Mã SV, Họ tên, Điểm CC, Điểm GK, Điểm CK, Điểm TK, Xếp loại. Tích hợp chế độ chống giật (`LVS_EX_DOUBLEBUFFER`) và chọn cả dòng (`LVS_EX_FULLROWSELECT`).
- **Tương tác 2 chiều thông minh:** Nhấp chuột vào bất kỳ dòng nào trong bảng sẽ tự động nạp thông tin sinh viên lên Form bên trái để xem hoặc sửa điểm.
- **Form nhập liệu đầy đủ kiểm tra:** 
  - Tự động cắt khoảng trắng thừa (`TrimW`).
  - Hỗ trợ nhập cả dấu chấm `.` hoặc dấu phẩy `,` (ví dụ `8.5` hoặc `8,5`).
  - Chặn triệt để mã SV để trống, trùng lặp mã SV, điểm ngoài khoảng `[0.0; 10.0]`, ký tự chữ cái, `NaN` và `Inf`.
  - Hỗ trợ phím **Tab** chuyển nhanh giữa các ô nhập liệu (`WS_TABSTOP` + `IsDialogMessageW`).
- **Lưu trữ dữ liệu vĩnh viễn (File I/O Persistence):**
  - Tự động lưu và nạp dữ liệu từ tệp `sinhvien.csv` chuẩn UTF-8 có BOM (mở trực tiếp bằng Microsoft Excel không bao giờ lỗi font tiếng Việt).
  - Tự động nạp khi mở ứng dụng và tự động lưu mỗi khi Thêm / Sửa / Xóa / Sắp xếp.
  - Có 2 nút riêng biệt: `[Lưu tệp tin (CSV)]` và `[Nạp từ tệp tin]`.
  - Đồng bộ dữ liệu hai chiều trong suốt với phiên bản dòng lệnh `main.exe`.
- **Nghiệp vụ nâng cao:** 
  - Nút **Sắp xếp giảm dần theo Điểm TK**: Sắp xếp danh sách tức thì và làm mới bảng.
  - Nút **Báo cáo thống kê & Danh sách học lại**: Bật hộp thoại tổng hợp phân bố tỷ lệ Giỏi/Khá/TB/Yếu/Kém, thủ khoa và danh sách học lại.
  - Nút **Xóa SV**: Hộp thoại hỏi xác nhận `Yes/No` trước khi xóa.
  - Nút **Nạp lại dữ liệu mẫu**: Tải lại 6 sinh viên mẫu ban đầu theo case study của Cô Lan.

---

## 3. Câu hỏi và câu trả lời bảo vệ đồ án gợi ý

### Câu 1: Vì sao hàm nhập danh sách phải nhận tham số `int *n` mà không phải `int n`? Nếu đổi thành `int n` thì điều gì xảy ra?
- **Trả lời:** Trong C, tham số truyền vào hàm mặc định theo cơ chế **truyền theo giá trị (Pass-by-value)**. Nếu dùng `int n`, hàm sẽ tạo một bản sao độc lập của `n` trong ngăn xếp (Stack). Mọi thay đổi tăng giảm số lượng phần tử bên trong hàm chỉ tác động lên bản sao này. Khi hàm kết thúc, bản sao bị hủy và biến `n` ở `main()` vẫn không thay đổi (gây lỗi danh sách rỗng).
- Khi truyền `int *n` (truyền con trỏ/địa chỉ), hàm nhận trực tiếp địa chỉ ô nhớ của biến `n` từ hàm gọi. Thông qua toán tử giải tham chiếu `*n` (ví dụ `(*n)++`), hàm trực tiếp cập nhật giá trị mới vào ô nhớ của biến `n` tại `main()`.

### Câu 2: Khi người dùng nhập điểm 11 hoặc -2, chương trình xử lý thế nào? Hãy chỉ ra đoạn code tương ứng.
- **Trả lời:** Chương trình kiểm tra chặt chẽ điều kiện số thực trong khoảng $[0.0; 10.0]$:
  - Trên CLI: Vòng lặp `do - while` phát hiện số $< 0$ hoặc $> 10$, in thông báo lỗi màu đỏ và bắt buộc người dùng nhập lại ngay tại chỗ.
  - Trên GUI: Hàm `ParseDiemW` phát hiện điểm không hợp lệ và hiển thị `MessageBoxW` cảnh báo, tự động focus con trỏ chuột về đúng ô nhập điểm bị lỗi để người dùng sửa.

### Câu 3: Khi xóa một sinh viên ở giữa mảng, các phần tử phía sau được dịch chuyển như thế nào? Biến `n` thay đổi ra sao?
- **Trả lời:** Khi xóa một phần tử tại vị trí `viTri` trong mảng, ta dùng vòng lặp `for (int i = viTri; i < *n - 1; i++) { a[i] = a[i + 1]; }`. Mỗi phần tử đứng sau sẽ được sao chép đè lên phần tử đứng trước, dịch chuyển toàn bộ các phần tử phía sau sang trái 1 vị trí. Sau đó, số lượng phần tử thực tế của danh sách giảm đi 1 bằng lệnh `(*n)--`.

### Câu 4: Phân biệt cách truy cập `sv.diemTK` và `sv->diemTK`. Khi nào dùng cách nào?
- **Trả lời:**
  - `sv.diemTK`: Dùng toán tử chấm `.` khi `sv` là một **biến cấu trúc trực tiếp** (ví dụ: `SinhVien sv;` hoặc `a[i].diemTK`).
  - `sv->diemTK`: Dùng toán tử mũi tên `->` khi `sv` là một **con trỏ trỏ tới cấu trúc** (ví dụ: `SinhVien *sv;`). Toán tử `->` tương đương với `(*sv).diemTK`.
  - Cách dùng: Dùng `.` khi thao tác với biến thông thường hoặc duyệt mảng; dùng `->` trong các hàm nhận tham số con trỏ (như `void tinhDiem(SinhVien *sv)`) để thao tác trực tiếp trên vùng nhớ gốc và tiết kiệm chi phí sao chép dữ liệu.

### Câu 5: Nhóm dùng thuật toán sắp xếp nào? Với `n` sinh viên, vòng lặp so sánh thực hiện tối đa bao nhiêu lần?
- **Trả lời:**
  - Nhóm dùng thuật toán **Sắp xếp Đổi chỗ trực tiếp (Interchange Sort)** duyệt 2 vòng lặp lồng nhau.
  - Vòng ngoài `i` chạy từ `0` đến `n - 2`, vòng trong `j` chạy từ `i + 1` đến `n - 1`. Nếu `a[i].diemTK < a[j].diemTK` thì hoán vị.
  - Tổng số lần so sánh tối đa là:
    $$S = (n - 1) + (n - 2) + \dots + 1 = \frac{n(n - 1)}{2}$$
  - Với lớp học gồm $n = 40$ sinh viên của Cô Lan, số lần so sánh là $\frac{40 \times 39}{2} = 780$ lần. Độ phức tạp thuật toán là $O(n^2)$.
