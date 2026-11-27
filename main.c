/**
 * ============================================================================
 * ĐỀ 01: QUẢN LÝ ĐIỂM SINH VIÊN CỦA LỚP HỌC (LỚP CNTT-K18 - CÔ LAN)
 * Học phần: Nhập môn lập trình
 * Ngôn ngữ: C (Chuẩn C99 / C11)
 * Sinh viên thực hiện: Nhóm sinh viên CNTT-K18
 * Giảng viên / Cố vấn học tập: Cô Lan
 * Tích hợp lưu trữ dữ liệu vĩnh viễn (File I/O - sinhvien.csv)
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#ifdef _WIN32
#include <windows.h>
#endif

#define MAX 100 // Số lượng sinh viên tối đa trong danh sách
#define DEFAULT_FILE "sinhvien.csv" // Tệp tin lưu trữ dữ liệu mặc định (tương thích Microsoft Excel)

/**
 * Cấu trúc lưu trữ thông tin sinh viên theo đúng yêu cầu đề bài
 */
typedef struct {
    char maSV[10];      // Mã sinh viên (VD: SV001)
    char hoTen[50];     // Họ và tên sinh viên
    float diemCC;       // Điểm chuyên cần (hệ số 10%)
    float diemGK;       // Điểm giữa kỳ (hệ số 30%)
    float diemCK;       // Điểm cuối kỳ (hệ số 60%)
    float diemTK;       // Điểm tổng kết
    char xepLoai[25];   // Xếp loại học lực
} SinhVien;

/* ================= KHAI BÁO CÁC NGUYÊN MẪU HÀM (PROTOTYPES) ================= */

// Nhóm tiện ích xử lý chuỗi và bộ đệm
void xoaBoDem(void);
void nhapChuoi(char *str, int maxLen);
float nhapDiemHopLe(const char *tenDiem);
void xoaKhoangTrangThua(char *s);
int demDoRongHienThi(const char *s);
void inChuoiCanLe(const char *s, int doRong, int canGiua);
int kiemTraTrungMa(const SinhVien a[], int n, const char *ma, int excludeIndex);

// Nhóm hàm bắt buộc theo yêu cầu đề bài
void nhapDS(SinhVien a[], int *n);
void tinhDiem(SinhVien *sv);
SinhVien* timKiemTheoMa(SinhVien a[], int n, const char *ma);

// Nhóm hàm chức năng quản lý, xử lý và hiển thị
void hienThiDS(const SinhVien a[], int n);
void tinhDiemToanBo(SinhVien a[], int n);
void suaDiem(SinhVien a[], int n);
void xoaSinhVien(SinhVien a[], int *n);
void sapXepGiamDanTheoDiemTK(SinhVien a[], int n);
void thongKe(const SinhVien a[], int n);

// Nhóm hàm giao diện và dữ liệu mẫu
void inDongKeBang(void);
void inTieuDeBang(void);
void inDongSinhVien(const SinhVien *sv, int stt);
void khoiTaoDuLieuMau(SinhVien a[], int *n);
void hienThiMenu(void);

// Nhóm hàm lưu trữ và đọc tệp tin (File I/O)
void luuFile(const SinhVien a[], int n, const char *tenFile);
void docFile(SinhVien a[], int *n, const char *tenFile);

/* ================= CHƯƠNG TRÌNH CHÍNH (MAIN) ================= */

int main(void) {
    // Thiết lập hiển thị mã UTF-8 trên Windows console để tiếng Việt hiển thị đẹp
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif

    SinhVien ds[MAX]; // Mảng 1 chiều lưu trữ danh sách sinh viên
    int n = 0;        // Số lượng sinh viên thực tế hiện có trong danh sách
    int luaChon = -1; // Lựa chọn chức năng từ menu

    // Tự động kiểm tra và tải dữ liệu đã lưu từ tệp tin sinhvien.csv
    printf("=========================================================================\n");
    printf("           ĐANG KHỞI ĐỘNG HỆ THỐNG QUẢN LÝ ĐIỂM HPU-SCORE...             \n");
    printf("=========================================================================\n");
    docFile(ds, &n, DEFAULT_FILE);

    // Nếu tệp chưa tồn tại hoặc rỗng, tự động nạp dữ liệu mẫu lớp CNTT-K18 ban đầu
    if (n == 0) {
        printf("[KHỞI TẠO] Chưa tìm thấy dữ liệu cũ, tự động nạp dữ liệu mẫu ban đầu...\n");
        khoiTaoDuLieuMau(ds, &n);
        luuFile(ds, n, DEFAULT_FILE); // Tự động tạo tệp lưu trữ lần đầu
    }
    printf(">> Đã sẵn sàng làm việc với %d sinh viên trong danh sách.\n\n", n);

    // Vòng lặp do - while duy trì hoạt động menu liên tục cho đến khi chọn 0
    do {
        hienThiMenu();
        printf(">> Vui lòng nhập lựa chọn của bạn (0 - 10): ");
        int resScan = scanf("%d", &luaChon);

        // Kiểm tra an toàn khi kết thúc file hoặc luồng dữ liệu (EOF handling)
        if (resScan == EOF) {
            printf("\n[THÔNG BÁO] Đã nhận tín hiệu kết thúc nhập liệu (EOF). Tự động lưu và thoát.\n");
            luuFile(ds, n, DEFAULT_FILE);
            break;
        }

        if (resScan != 1) {
            printf("\n[CẢNH BÁO] Lựa chọn không hợp lệ! Vui lòng nhập một số nguyên từ 0 đến 10.\n");
            xoaBoDem();
            luaChon = -1;
            continue;
        }
        xoaBoDem(); // Xóa ký tự xuống dòng sau khi nhập số

        printf("\n");
        // Cấu trúc switch - case điều hướng từng chức năng của hệ thống
        switch (luaChon) {
            case 1:
                printf("================= CHỨC NĂNG 1: NHẬP DANH SÁCH SINH VIÊN =================\n");
                nhapDS(ds, &n);
                luuFile(ds, n, DEFAULT_FILE); // Tự động lưu sau khi nhập
                break;
            case 2:
                printf("================= CHỨC NĂNG 2: HIỂN THỊ DANH SÁCH BẢNG ĐIỂM =============\n");
                hienThiDS(ds, n);
                break;
            case 3:
                printf("================= CHỨC NĂNG 3: TÍNH ĐIỂM TỔNG KẾT & XẾP LOẠI ============\n");
                tinhDiemToanBo(ds, n);
                luuFile(ds, n, DEFAULT_FILE);
                break;
            case 4:
                printf("================= CHỨC NĂNG 4: TÌM KIẾM VÀ SỬA ĐIỂM SINH VIÊN ===========\n");
                suaDiem(ds, n);
                luuFile(ds, n, DEFAULT_FILE); // Tự động lưu sau khi sửa
                break;
            case 5:
                printf("================= CHỨC NĂNG 5: XÓA SINH VIÊN THEO MÃ ====================\n");
                xoaSinhVien(ds, &n);
                luuFile(ds, n, DEFAULT_FILE); // Tự động lưu sau khi xóa
                break;
            case 6:
                printf("================= CHỨC NĂNG 6: SẮP XẾP GIẢM DẦN THEO ĐIỂM TK ============\n");
                sapXepGiamDanTheoDiemTK(ds, n);
                luuFile(ds, n, DEFAULT_FILE); // Tự động lưu sau khi sắp xếp
                break;
            case 7:
                printf("================= CHỨC NĂNG 7: THỐNG KÊ TỔNG HỢP ========================\n");
                thongKe(ds, n);
                break;
            case 8:
                printf("================= CHỨC NĂNG 8: LƯU DỮ LIỆU RA TỆP TIN (CSV) =============\n");
                luuFile(ds, n, DEFAULT_FILE);
                break;
            case 9:
                printf("================= CHỨC NĂNG 9: NẠP DỮ LIỆU TỪ TỆP TIN (CSV) =============\n");
                docFile(ds, &n, DEFAULT_FILE);
                hienThiDS(ds, n);
                break;
            case 10:
                printf("================= CHỨC NĂNG 10: TẢI LẠI DỮ LIỆU MẪU BAN ĐẦU =============\n");
                khoiTaoDuLieuMau(ds, &n);
                luuFile(ds, n, DEFAULT_FILE);
                printf("[THÀNH CÔNG] Đã nạp lại dữ liệu mẫu lớp CNTT-K18 (gồm %d sinh viên).\n", n);
                break;
            case 0:
                printf("=========================================================================\n");
                printf("  Đang lưu dữ liệu trước khi thoát...\n");
                luuFile(ds, n, DEFAULT_FILE);
                printf("  Cảm ơn Cô Lan và các bạn đã sử dụng chương trình HPU-Score!\n");
                printf("  Chúc Cô Lan và tập thể lớp CNTT-K18 một học kỳ thành công rực rỡ!\n");
                printf("=========================================================================\n");
                break;
            default:
                printf("[LỖI] Lựa chọn %d không tồn tại! Vui lòng chọn trong khoảng từ 0 đến 10.\n", luaChon);
                break;
        }

        if (luaChon != 0 && !feof(stdin)) {
            printf("\nNhấn Enter để quay lại menu chính...");
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {}
            printf("\n");
        }

    } while (luaChon != 0);

    return 0;
}

/* ================= HIỆN THỰC CÁC HÀM LƯU TRỮ TỆP TIN (FILE I/O) ================= */

/**
 * Lưu toàn bộ danh sách sinh viên vào tệp tin CSV (tương thích Excel & Notepad)
 * Tự động ghi mã UTF-8 BOM (\xEF\xBB\xBF) để mở trực tiếp trong Excel hiển thị chuẩn tiếng Việt
 */
void luuFile(const SinhVien a[], int n, const char *tenFile) {
    if (tenFile == NULL || strlen(tenFile) == 0) {
        tenFile = DEFAULT_FILE;
    }

    FILE *f = fopen(tenFile, "w");
    if (f == NULL) {
        printf("[LỖI] Không thể mở tệp tin '%s' để ghi dữ liệu!\n", tenFile);
        return;
    }

    // Ghi UTF-8 BOM để Excel hiển thị tiếng Việt có dấu chuẩn đẹp
    fputs("\xEF\xBB\xBF", f);

    // Ghi dòng tiêu đề cột
    fprintf(f, "STT,Mã SV,Họ và tên,Điểm CC,Điểm GK,Điểm CK,Điểm TK,Xếp loại\n");

    // Ghi từng dòng sinh viên
    for (int i = 0; i < n; i++) {
        fprintf(f, "%d,%s,%s,%.1f,%.1f,%.1f,%.1f,%s\n",
                i + 1, a[i].maSV, a[i].hoTen,
                a[i].diemCC, a[i].diemGK, a[i].diemCK, a[i].diemTK, a[i].xepLoai);
    }

    fclose(f);
    printf("[THÀNH CÔNG] Đã lưu an toàn %d sinh viên vào tệp tin '%s'!\n", n, tenFile);
}

/**
 * Đọc dữ liệu sinh viên từ tệp tin CSV vào mảng cấu trúc
 * Bỏ qua dòng tiêu đề, tự động kiểm tra định dạng và chuẩn hóa điểm số
 */
void docFile(SinhVien a[], int *n, const char *tenFile) {
    if (tenFile == NULL || strlen(tenFile) == 0) {
        tenFile = DEFAULT_FILE;
    }

    FILE *f = fopen(tenFile, "r");
    if (f == NULL) {
        // Tệp chưa tồn tại (lần đầu chạy chương trình)
        return;
    }

    // Đọc dòng tiêu đề (kèm bỏ qua BOM nếu có)
    char header[256];
    if (fgets(header, sizeof(header), f) == NULL) {
        fclose(f);
        return;
    }

    int count = 0;
    char line[512];
    while (fgets(line, sizeof(line), f) != NULL && count < MAX) {
        xoaKhoangTrangThua(line);
        if (strlen(line) == 0) continue;

        // Tách các trường dữ liệu theo dấu phẩy ','
        // Định dạng: STT, Mã SV, Họ và tên, Điểm CC, Điểm GK, Điểm CK, Điểm TK, Xếp loại
        char *tokenSTT = strtok(line, ",");
        char *tokenMa  = strtok(NULL, ",");
        char *tokenTen = strtok(NULL, ",");
        char *tokenCC  = strtok(NULL, ",");
        char *tokenGK  = strtok(NULL, ",");
        char *tokenCK  = strtok(NULL, ",");
        (void)tokenSTT;

        if (tokenMa && tokenTen && tokenCC && tokenGK && tokenCK) {
            xoaKhoangTrangThua(tokenMa);
            xoaKhoangTrangThua(tokenTen);

            if (strlen(tokenMa) > 0 && strlen(tokenTen) > 0) {
                strncpy(a[count].maSV, tokenMa, sizeof(a[count].maSV) - 1);
                a[count].maSV[sizeof(a[count].maSV) - 1] = '\0';

                strncpy(a[count].hoTen, tokenTen, sizeof(a[count].hoTen) - 1);
                a[count].hoTen[sizeof(a[count].hoTen) - 1] = '\0';

                a[count].diemCC = (float)atof(tokenCC);
                a[count].diemGK = (float)atof(tokenGK);
                a[count].diemCK = (float)atof(tokenCK);

                // Tự động tính lại điểm tổng kết và xếp loại để đảm bảo độ chính xác
                tinhDiem(&a[count]);
                count++;
            }
        }
    }

    fclose(f);
    *n = count;
    printf("[THÀNH CÔNG] Đã nạp %d sinh viên từ tệp tin '%s' vào bộ nhớ!\n", *n, tenFile);
}

/* ================= HIỆN THỰC CÁC HÀM TIỆN ÍCH NHẬP LIỆU & FORMAT ================= */

/**
 * Xóa sạch các ký tự còn lại trong bộ đệm bàn phím stdin
 */
void xoaBoDem(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        // Bỏ qua ký tự thừa
    }
}

/**
 * Tính số cột hiển thị thực tế (visual width) của chuỗi UTF-8 tiếng Việt
 */
int demDoRongHienThi(const char *s) {
    if (s == NULL) return 0;
    int visualLen = 0;
    for (int i = 0; s[i] != '\0'; i++) {
        if (((unsigned char)s[i] & 0xC0) != 0x80) {
            visualLen++;
        }
    }
    return visualLen;
}

/**
 * In chuỗi s căn lề trong ô có chiều rộng hiển thị doRong
 */
void inChuoiCanLe(const char *s, int doRong, int canGiua) {
    if (s == NULL) return;
    int vLen = demDoRongHienThi(s);

    if (vLen > doRong) {
        int targetVisual = (doRong > 2) ? (doRong - 2) : doRong;
        int curVisual = 0;
        int i = 0;
        while (s[i] != '\0') {
            int isLeading = (((unsigned char)s[i] & 0xC0) != 0x80);
            if (isLeading && curVisual >= targetVisual) {
                break;
            }
            if (isLeading) curVisual++;
            putchar(s[i]);
            i++;
        }
        if (doRong > 2) {
            printf("..");
            curVisual += 2;
        }
        for (int k = curVisual; k < doRong; k++) putchar(' ');
        return;
    }

    int spaceCount = doRong - vLen;
    if (canGiua) {
        int leftSpaces = spaceCount / 2;
        int rightSpaces = spaceCount - leftSpaces;
        for (int i = 0; i < leftSpaces; i++) putchar(' ');
        printf("%s", s);
        for (int i = 0; i < rightSpaces; i++) putchar(' ');
    } else {
        printf("%s", s);
        for (int i = 0; i < spaceCount; i++) putchar(' ');
    }
}

/**
 * Cắt bỏ khoảng trắng dư thừa ở đầu và cuối chuỗi
 */
void xoaKhoangTrangThua(char *s) {
    if (s == NULL) return;
    int len = (int)strlen(s);

    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r' || s[len - 1] == ' ' || s[len - 1] == '\t')) {
        s[--len] = '\0';
    }

    int start = 0;
    while (s[start] == ' ' || s[start] == '\t') {
        start++;
    }

    if (start > 0) {
        int i = 0;
        while (s[start + i] != '\0') {
            s[i] = s[start + i];
            i++;
        }
        s[i] = '\0';
    }
}

/**
 * Nhập chuỗi an toàn với kích thước tối đa maxLen
 */
void nhapChuoi(char *str, int maxLen) {
    if (fgets(str, maxLen, stdin) != NULL) {
        char *nl = strchr(str, '\n');
        if (nl != NULL) {
            *nl = '\0';
        } else if (!feof(stdin)) {
            xoaBoDem();
        }
        xoaKhoangTrangThua(str);
    } else {
        str[0] = '\0';
    }
}

/**
 * Nhập điểm hợp lệ trong khoảng [0.0; 10.0]
 */
float nhapDiemHopLe(const char *tenDiem) {
    float diem = 0.0f;
    int hopLe = 0;
    do {
        printf("  - Nhập %s [0.0 - 10.0]: ", tenDiem);
        int res = scanf("%f", &diem);
        if (res == EOF) {
            return 0.0f;
        }
        if (res != 1) {
            printf("    -> [Lỗi] Giá trị nhập vào phải là một số thực! Vui lòng nhập lại.\n");
            xoaBoDem();
            if (feof(stdin)) return 0.0f;
            continue;
        }
        xoaBoDem();

        if (!isfinite(diem) || diem < 0.0f || diem > 10.0f) {
            printf("    -> [Lỗi] Điểm không hợp lệ! Điểm phải nằm trong đoạn [0; 10]. Vui lòng nhập lại.\n");
        } else {
            hopLe = 1;
        }
    } while (!hopLe);

    return diem;
}

/**
 * Kiểm tra mã sinh viên có bị trùng lặp trong mảng không
 */
int kiemTraTrungMa(const SinhVien a[], int n, const char *ma, int excludeIndex) {
    for (int i = 0; i < n; i++) {
        if (i == excludeIndex) continue;
        if (strcmp(a[i].maSV, ma) == 0) {
            return 1;
        }
    }
    return 0;
}

/* ================= HIỆN THỰC CÁC HÀM XỬ LÝ THEO ĐỀ BÀI ================= */

/**
 * Tính điểm tổng kết và xếp loại cho một sinh viên
 */
void tinhDiem(SinhVien *sv) {
    if (sv == NULL) return;

    float tk = (0.10f * sv->diemCC) + (0.30f * sv->diemGK) + (0.60f * sv->diemCK);
    sv->diemTK = roundf(tk * 10.0f) / 10.0f;

    if (sv->diemTK >= 8.0f) {
        strcpy(sv->xepLoai, "Giỏi");
    } else if (sv->diemTK >= 6.5f) {
        strcpy(sv->xepLoai, "Khá");
    } else if (sv->diemTK >= 5.0f) {
        strcpy(sv->xepLoai, "Trung bình");
    } else if (sv->diemTK >= 4.0f) {
        strcpy(sv->xepLoai, "Yếu");
    } else {
        strcpy(sv->xepLoai, "Kém (Học lại)");
    }
}

/**
 * Nhập danh sách sinh viên
 */
void nhapDS(SinhVien a[], int *n) {
    int m;
    printf(">> Nhập số lượng sinh viên cần thêm vào danh sách: ");
    if (scanf("%d", &m) != 1 || m <= 0) {
        printf("[CẢNH BÁO] Số lượng sinh viên phải là một số nguyên dương!\n");
        xoaBoDem();
        return;
    }
    xoaBoDem();

    if (*n + m > MAX) {
        printf("[LỖI] Danh sách chỉ có thể chứa tối đa %d sinh viên! Hiện có %d, không thể thêm %d sinh viên.\n", MAX, *n, m);
        return;
    }

    for (int i = 0; i < m; i++) {
        int index = *n;
        printf("\n--- Nhập thông tin sinh viên thứ %d (STT toàn lớp: %d) ---\n", i + 1, index + 1);

        char ma[10];
        do {
            if (feof(stdin)) return;
            printf("  - Nhập mã sinh viên (VD: SV001): ");
            nhapChuoi(ma, sizeof(ma));
            if (feof(stdin)) return;

            if (strlen(ma) == 0) {
                printf("    -> [Lỗi] Mã sinh viên không được để trống! Vui lòng nhập lại.\n");
                continue;
            }

            if (kiemTraTrungMa(a, *n, ma, -1)) {
                printf("    -> [Lỗi] Mã sinh viên '%s' đã tồn tại! Vui lòng nhập mã khác.\n", ma);
            } else {
                break;
            }
        } while (1);
        strcpy(a[index].maSV, ma);

        do {
            if (feof(stdin)) return;
            printf("  - Nhập họ và tên sinh viên: ");
            nhapChuoi(a[index].hoTen, sizeof(a[index].hoTen));
            if (feof(stdin)) return;

            if (strlen(a[index].hoTen) == 0) {
                printf("    -> [Lỗi] Họ tên không được để trống! Vui lòng nhập lại.\n");
            } else {
                break;
            }
        } while (1);

        a[index].diemCC = nhapDiemHopLe("điểm Chuyên cần (10%)");
        if (feof(stdin)) return;
        a[index].diemGK = nhapDiemHopLe("điểm Giữa kỳ    (30%)");
        if (feof(stdin)) return;
        a[index].diemCK = nhapDiemHopLe("điểm Cuối kỳ    (60%)");
        if (feof(stdin)) return;

        tinhDiem(&a[index]);

        (*n)++;
        printf("  => Thêm thành công: %s - %s | Điểm TK: %.1f | Xếp loại: %s\n",
               a[index].maSV, a[index].hoTen, a[index].diemTK, a[index].xepLoai);
    }

    printf("\n[HOÀN THÀNH] Đã thêm thành công %d sinh viên. Tổng số sinh viên hiện có: %d.\n", m, *n);
}

/**
 * Tìm kiếm sinh viên theo mã sinh viên
 */
SinhVien* timKiemTheoMa(SinhVien a[], int n, const char *ma) {
    if (ma == NULL || strlen(ma) == 0) return NULL;
    for (int i = 0; i < n; i++) {
        if (strcmp(a[i].maSV, ma) == 0) {
            return &a[i];
        }
    }
    return NULL;
}

/**
 * Tính điểm tổng kết và xếp loại cho toàn bộ danh sách sinh viên
 */
void tinhDiemToanBo(SinhVien a[], int n) {
    if (n == 0) {
        printf("[THÔNG BÁO] Danh sách sinh viên hiện đang rỗng!\n");
        return;
    }

    for (int i = 0; i < n; i++) {
        tinhDiem(&a[i]);
    }
    printf("[THÀNH CÔNG] Đã tính toán lại điểm tổng kết và xếp loại cho toàn bộ %d sinh viên!\n", n);
    hienThiDS(a, n);
}

/**
 * Tìm kiếm sinh viên theo mã và sửa điểm của sinh viên tìm được
 */
void suaDiem(SinhVien a[], int n) {
    if (n == 0) {
        printf("[THÔNG BÁO] Danh sách sinh viên hiện đang rỗng!\n");
        return;
    }

    char ma[10];
    printf(">> Nhập mã sinh viên cần tìm và sửa điểm: ");
    nhapChuoi(ma, sizeof(ma));

    SinhVien *sv = timKiemTheoMa(a, n, ma);
    if (sv == NULL) {
        printf("\n[KẾT QUẢ] Không tìm thấy sinh viên nào có mã '%s' trong danh sách!\n", ma);
        return;
    }

    printf("\n[TÌM THẤY THÔNG TIN SINH VIÊN]:\n");
    printf("  + Mã SV   : %s\n", sv->maSV);
    printf("  + Họ tên  : %s\n", sv->hoTen);
    printf("  + Điểm CC : %.1f\n", sv->diemCC);
    printf("  + Điểm GK : %.1f\n", sv->diemGK);
    printf("  + Điểm CK : %.1f\n", sv->diemCK);
    printf("  + Điểm TK : %.1f\n", sv->diemTK);
    printf("  + Xếp loại: %s\n", sv->xepLoai);

    printf("\n>> Bạn có muốn cập nhật lại điểm cho sinh viên này không? (1: Có | 0: Bỏ qua): ");
    int xacNhan;
    if (scanf("%d", &xacNhan) != 1 || xacNhan != 1) {
        xoaBoDem();
        printf("[HỦY BỎ] Đã hủy thao tác sửa điểm.\n");
        return;
    }
    xoaBoDem();

    printf("\n--- Nhập 3 đầu điểm mới cho sinh viên %s (%s) ---\n", sv->hoTen, sv->maSV);
    sv->diemCC = nhapDiemHopLe("điểm Chuyên cần mới (10%)");
    sv->diemGK = nhapDiemHopLe("điểm Giữa kỳ mới    (30%)");
    sv->diemCK = nhapDiemHopLe("điểm Cuối kỳ mới    (60%)");

    tinhDiem(sv);

    printf("\n[CẬP NHẬT THÀNH CÔNG] Điểm mới của sinh viên:\n");
    printf("  => CC: %.1f | GK: %.1f | CK: %.1f => Điểm TK mới: %.1f | Xếp loại mới: %s\n",
           sv->diemCC, sv->diemGK, sv->diemCK, sv->diemTK, sv->xepLoai);
}

/**
 * Xóa một sinh viên khỏi danh sách theo mã
 */
void xoaSinhVien(SinhVien a[], int *n) {
    if (*n == 0) {
        printf("[THÔNG BÁO] Danh sách sinh viên hiện đang rỗng!\n");
        return;
    }

    char ma[10];
    printf(">> Nhập mã sinh viên cần xóa: ");
    nhapChuoi(ma, sizeof(ma));

    int viTri = -1;
    for (int i = 0; i < *n; i++) {
        if (strcmp(a[i].maSV, ma) == 0) {
            viTri = i;
            break;
        }
    }

    if (viTri == -1) {
        printf("\n[KẾT QUẢ] Không tìm thấy sinh viên có mã '%s' để xóa!\n", ma);
        return;
    }

    printf("\n[XÁC NHẬN] Bạn có chắc chắn muốn xóa sinh viên sau khỏi danh sách?\n");
    printf("  Mã SV: %s | Họ tên: %s | Điểm TK: %.1f | Xếp loại: %s\n",
           a[viTri].maSV, a[viTri].hoTen, a[viTri].diemTK, a[viTri].xepLoai);
    printf(">> Nhập 1 để XÁC NHẬN XÓA, nhập số khác để HỦY BỎ: ");
    int xacNhan;
    if (scanf("%d", &xacNhan) != 1 || xacNhan != 1) {
        xoaBoDem();
        printf("[HỦY BỎ] Đã hủy thao tác xóa sinh viên.\n");
        return;
    }
    xoaBoDem();

    for (int i = viTri; i < *n - 1; i++) {
        a[i] = a[i + 1];
    }
    (*n)--;

    printf("\n[THÀNH CÔNG] Đã xóa sinh viên '%s'. Số sinh viên còn lại trong danh sách: %d.\n", ma, *n);
}

/**
 * Sắp xếp danh sách sinh viên giảm dần theo điểm tổng kết
 */
void sapXepGiamDanTheoDiemTK(SinhVien a[], int n) {
    if (n <= 1) {
        printf("[THÔNG BÁO] Danh sách có ít hơn 2 sinh viên, không cần sắp xếp!\n");
        return;
    }

    for (int i = 0; i < n; i++) {
        tinhDiem(&a[i]);
    }

    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (a[i].diemTK < a[j].diemTK) {
                SinhVien temp = a[i];
                a[i] = a[j];
                a[j] = temp;
            }
        }
    }

    printf("[THÀNH CÔNG] Đã sắp xếp danh sách sinh viên giảm dần theo điểm tổng kết!\n");
    hienThiDS(a, n);
}

/**
 * Thống kê phân loại, thủ khoa và danh sách học lại
 */
void thongKe(const SinhVien a[], int n) {
    if (n == 0) {
        printf("[THÔNG BÁO] Danh sách sinh viên hiện đang rỗng, không thể thống kê!\n");
        return;
    }

    int demGioi = 0, demKha = 0, demTB = 0, demYeu = 0, demKem = 0;
    float maxTK = a[0].diemTK;

    for (int i = 0; i < n; i++) {
        if (a[i].diemTK > maxTK) {
            maxTK = a[i].diemTK;
        }

        if (strcmp(a[i].xepLoai, "Giỏi") == 0) {
            demGioi++;
        } else if (strcmp(a[i].xepLoai, "Khá") == 0) {
            demKha++;
        } else if (strcmp(a[i].xepLoai, "Trung bình") == 0) {
            demTB++;
        } else if (strcmp(a[i].xepLoai, "Yếu") == 0) {
            demYeu++;
        } else {
            demKem++;
        }
    }

    printf("======================== BÁO CÁO THỐNG KÊ LỚP CNTT-K18 ========================\n");
    printf(" Tổng số sinh viên hiện có: %d sinh viên\n", n);
    printf("--------------------------------------------------------------------------------\n");
    printf(" 1. THỐNG KÊ THEO XẾP LOẠI HỌC LỰC:\n");
    printf("    + Giỏi       (Điểm TK >= 8.0) : %2d sinh viên (tỷ lệ: %5.1f%%)\n", demGioi, (demGioi * 100.0f) / n);
    printf("    + Khá        (Điểm TK >= 6.5) : %2d sinh viên (tỷ lệ: %5.1f%%)\n", demKha,  (demKha  * 100.0f) / n);
    printf("    + Trung bình (Điểm TK >= 5.0) : %2d sinh viên (tỷ lệ: %5.1f%%)\n", demTB,   (demTB   * 100.0f) / n);
    printf("    + Yếu        (Điểm TK >= 4.0) : %2d sinh viên (tỷ lệ: %5.1f%%)\n", demYeu,  (demYeu  * 100.0f) / n);
    printf("    + Kém        (Điểm TK <  4.0) : %2d sinh viên (tỷ lệ: %5.1f%%) [Học lại]\n", demKem,  (demKem  * 100.0f) / n);
    printf("--------------------------------------------------------------------------------\n");

    printf(" 2. DANH SÁCH SINH VIÊN CÓ ĐIỂM TỔNG KẾT CAO NHẤT (Thủ khoa: %.1f điểm):\n", maxTK);
    int demThuKhoa = 0;
    for (int i = 0; i < n; i++) {
        if (a[i].diemTK == maxTK) {
            demThuKhoa++;
            printf("    [%d] Mã: ", demThuKhoa);
            inChuoiCanLe(a[i].maSV, 8, 0);
            printf(" | Họ tên: ");
            inChuoiCanLe(a[i].hoTen, 24, 0);
            printf(" | CC: %4.1f | GK: %4.1f | CK: %4.1f | TK: %4.1f | Xếp loại: %s\n",
                   a[i].diemCC, a[i].diemGK, a[i].diemCK, a[i].diemTK, a[i].xepLoai);
        }
    }

    printf("--------------------------------------------------------------------------------\n");
    printf(" 3. DANH SÁCH SINH VIÊN BỊ TRƯỢT MÔN (PHẢI HỌC LẠI - Điểm TK < 4.0):\n");
    if (demKem == 0) {
        printf("    -> Tuyệt vời! Không có sinh viên nào trong lớp phải học lại môn này!\n");
    } else {
        int demHocLai = 0;
        for (int i = 0; i < n; i++) {
            if (a[i].diemTK < 4.0f) {
                demHocLai++;
                printf("    [%d] Mã: ", demHocLai);
                inChuoiCanLe(a[i].maSV, 8, 0);
                printf(" | Họ tên: ");
                inChuoiCanLe(a[i].hoTen, 24, 0);
                printf(" | CC: %4.1f | GK: %4.1f | CK: %4.1f | TK: %4.1f | %s\n",
                       a[i].diemCC, a[i].diemGK, a[i].diemCK, a[i].diemTK, a[i].xepLoai);
            }
        }
    }
    printf("================================================================================\n");
}

/* ================= HIỆN THỰC CÁC HÀM HIỂN THỊ GIAO DIỆN ================= */

void inDongKeBang(void) {
    printf("+-----+----------+--------------------------+---------+---------+---------+---------+----------------+\n");
}

void inTieuDeBang(void) {
    inDongKeBang();
    printf("| STT |  Mã SV   |        Họ và tên         | Điểm CC | Điểm GK | Điểm CK | Điểm TK |    Xếp loại    |\n");
    inDongKeBang();
}

void inDongSinhVien(const SinhVien *sv, int stt) {
    printf("| %3d | ", stt);
    inChuoiCanLe(sv->maSV, 8, 0);
    printf(" | ");
    inChuoiCanLe(sv->hoTen, 24, 0);
    printf(" |  %5.1f  |  %5.1f  |  %5.1f  |  %5.1f  | ",
           sv->diemCC, sv->diemGK, sv->diemCK, sv->diemTK);
    inChuoiCanLe(sv->xepLoai, 14, 0);
    printf(" |\n");
}

void hienThiDS(const SinhVien a[], int n) {
    if (n == 0) {
        printf("[THÔNG BÁO] Danh sách sinh viên hiện đang rỗng! Vui lòng nhập dữ liệu.\n");
        return;
    }

    printf("\n                              BẢNG TỔNG HỢP ĐIỂM HỌC KỲ LỚP CNTT-K18\n");
    inTieuDeBang();
    for (int i = 0; i < n; i++) {
        inDongSinhVien(&a[i], i + 1);
    }
    inDongKeBang();
    printf(" Tổng số: %d sinh viên.\n", n);
}

void hienThiMenu(void) {
    printf("=========================================================================\n");
    printf("               HỆ THỐNG QUẢN LÝ ĐIỂM SINH VIÊN - HPU SCORE               \n");
    printf("               Cố vấn học tập: Cô Lan - Lớp: CNTT-K18                    \n");
    printf("=========================================================================\n");
    printf("  [1]  Nhập thêm danh sách sinh viên (kiểm tra điểm [0; 10], mã không trùng)\n");
    printf("  [2]  Hiển thị danh sách sinh viên dưới dạng bảng\n");
    printf("  [3]  Tính điểm tổng kết và xếp loại cho toàn bộ sinh viên\n");
    printf("  [4]  Tìm kiếm sinh viên theo mã và sửa điểm\n");
    printf("  [5]  Xóa sinh viên khỏi danh sách theo mã\n");
    printf("  [6]  Sắp xếp danh sách giảm dần theo điểm tổng kết\n");
    printf("  [7]  Thống kê (phân loại, sinh viên điểm cao nhất, danh sách học lại)\n");
    printf("  [8]  Lưu danh sách sinh viên vào tệp tin (sinhvien.csv)\n");
    printf("  [9]  Đọc lại danh sách sinh viên từ tệp tin (sinhvien.csv)\n");
    printf("  [10] Tải lại dữ liệu mẫu lớp CNTT-K18 (case study đề bài)\n");
    printf("  [0]  Lưu dữ liệu và Thoát chương trình\n");
    printf("=========================================================================\n");
}

void khoiTaoDuLieuMau(SinhVien a[], int *n) {
    SinhVien dsMau[] = {
        {"SV001", "Nguyễn Văn An",       9.0f,  7.0f,  8.0f, 0.0f, ""}, // Điểm TK: 7.8 -> Khá (đề bài)
        {"SV002", "Trần Thị Bình",      10.0f,  3.0f,  3.0f, 0.0f, ""}, // Điểm TK: 3.7 -> Kém (học lại, đề bài)
        {"SV003", "Lê Hoàng Cường",      9.5f,  9.0f,  8.5f, 0.0f, ""}, // Điểm TK: 8.8 -> Giỏi
        {"SV004", "Phạm Minh Đức",       8.0f,  6.0f,  5.5f, 0.0f, ""}, // Điểm TK: 5.9 -> Trung bình
        {"SV005", "Hoàng Ngọc Điệp",     7.0f,  4.5f,  4.0f, 0.0f, ""}, // Điểm TK: 4.5 -> Yếu
        {"SV006", "Vũ Thị Hoa",         10.0f,  9.5f,  9.0f, 0.0f, ""}  // Điểm TK: 9.3 -> Giỏi (Thủ khoa)
    };

    int soLuongMau = sizeof(dsMau) / sizeof(dsMau[0]);
    *n = soLuongMau;

    for (int i = 0; i < soLuongMau; i++) {
        a[i] = dsMau[i];
        tinhDiem(&a[i]);
    }
}
