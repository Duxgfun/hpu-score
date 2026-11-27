/**
 * ============================================================================
 * ĐỀ 01: QUẢN LÝ ĐIỂM SINH VIÊN CỦA LỚP HỌC (LỚP CNTT-K18 - CÔ LAN)
 * PHIÊN BẢN GIAO DIỆN ĐỒ HỌA WINDOWS NGUYÊN BẢN (WIN32 DESKTOP GUI)
 * TÍCH HỢP LƯU TRỮ DỮ LIỆU VĨNH VIỄN (FILE I/O - sinhvien.csv)
 * Ngôn ngữ: C thuần chuẩn (Pure C - Win32 API / Common Controls v6)
 * Sinh viên thực hiện: Nhóm sinh viên CNTT-K18
 * Giảng viên / Cố vấn học tập: Cô Lan
 * ============================================================================
 */

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <wchar.h>

#define MAX 100 // Số lượng sinh viên tối đa
#define DEFAULT_FILE L"sinhvien.csv" // Tệp tin lưu trữ dữ liệu vĩnh viễn

// Định danh các thành phần điều khiển (Control IDs)
#define ID_EDT_MASV       1001
#define ID_EDT_HOTEN      1002
#define ID_EDT_DIEMCC     1003
#define ID_EDT_DIEMGK     1004
#define ID_EDT_DIEMCK     1005
#define ID_BTN_ADD        1006
#define ID_BTN_UPDATE     1007
#define ID_BTN_DELETE     1008
#define ID_BTN_CLEAR      1009
#define ID_BTN_SEARCH     1010
#define ID_BTN_SORT       1011
#define ID_BTN_STATS      1012
#define ID_BTN_RELOAD     1013
#define ID_LISTVIEW       1014
#define ID_ST_STATUS      1015
#define ID_BTN_SAVEFILE   1016
#define ID_BTN_LOADFILE   1017

/**
 * Cấu trúc sinh viên phiên bản GUI (Unicode UTF-16)
 */
typedef struct {
    wchar_t maSV[16];      // Mã sinh viên (VD: SV001)
    wchar_t hoTen[64];     // Họ và tên
    float diemCC;          // Điểm chuyên cần (10%)
    float diemGK;          // Điểm giữa kỳ (30%)
    float diemCK;          // Điểm cuối kỳ (60%)
    float diemTK;          // Điểm tổng kết
    wchar_t xepLoai[32];   // Xếp loại học lực
} SinhVienGUI;

// Biến toàn cục quản lý dữ liệu và cửa sổ
SinhVienGUI g_ds[MAX];
int g_n = 0;

HWND g_hMainWnd = NULL;
HWND g_hListView = NULL;
HWND g_hEdtMaSV = NULL;
HWND g_hEdtHoTen = NULL;
HWND g_hEdtDiemCC = NULL;
HWND g_hEdtDiemGK = NULL;
HWND g_hEdtDiemCK = NULL;
HWND g_hStStatus = NULL;
HFONT g_hFontNormal = NULL;
HFONT g_hFontBold = NULL;
HFONT g_hFontTitle = NULL;

/* ================= NGUYÊN MẪU HÀM (PROTOTYPES) ================= */
void TrimW(wchar_t *s);
int ParseDiemW(const wchar_t *strInput, float *outDiem);
void TinhDiemGUI(SinhVienGUI *sv);
void KhoiTaoDuLieuMauGUI(void);
void CapNhatListView(void);
void CapNhatThanhTrangThai(void);
void XoaTrangForm(void);
int KiemTraTrungMaGUI(const wchar_t *ma, int excludeIndex);
SinhVienGUI* TimKiemTheoMaGUI(const wchar_t *ma, int *outIndex);
void XuLyThemSinhVien(void);
void XuLyCapNhatDiem(void);
void XuLyXoaSinhVien(void);
void XuLySapXepGiamDan(void);
void XuLyThongKe(void);
void XuLyTimKiem(void);
void XuLyChonItemListView(int itemIndex);
void LuuFileGUI(const wchar_t *tenFile);
void DocFileGUI(const wchar_t *tenFile);
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

/* ================= HÀM KHỞI CHẠY ỨNG DỤNG WIN32 (WINMAIN) ================= */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    (void)hPrevInstance;
    (void)lpCmdLine;

    // Khởi tạo các điều khiển Common Controls (ListView, Button,...)
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_LISTVIEW_CLASSES | ICC_STANDARD_CLASSES;
    InitCommonControlsEx(&icex);

    // Đăng ký lớp cửa sổ chính
    const wchar_t CLASS_NAME[] = L"HpuScoreWindowClass";
    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassExW(&wc)) {
        MessageBoxW(NULL, L"Không thể đăng ký lớp cửa sổ ứng dụng!", L"Lỗi khởi động", MB_ICONERROR);
        return 1;
    }

    // Tạo cửa sổ chính (kích thước chuẩn 1040 x 740 pixels, canh giữa màn hình)
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int winWidth = 1040;
    int winHeight = 740;
    int posX = (screenWidth - winWidth) / 2;
    int posY = (screenHeight - winHeight) / 2;

    g_hMainWnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"Hệ thống Quản lý Điểm Sinh viên - HPU Score (CNTT-K18 - Cô Lan)",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        posX, posY, winWidth, winHeight,
        NULL, NULL, hInstance, NULL
    );

    if (!g_hMainWnd) {
        MessageBoxW(NULL, L"Không thể khởi tạo cửa sổ chính!", L"Lỗi", MB_ICONERROR);
        return 1;
    }

    ShowWindow(g_hMainWnd, nCmdShow);
    UpdateWindow(g_hMainWnd);

    // Vòng lặp nhận và điều phối thông điệp (Hỗ trợ phím Tab chuyển ô bằng IsDialogMessageW)
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        if (!IsDialogMessageW(g_hMainWnd, &msg)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    return (int)msg.wParam;
}

/* ================= XỬ LÝ SỰ KIỆN CỦA CỬA SỔ (WNDPROC) ================= */
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            // Khởi tạo các Font chữ hệ thống (Segoe UI sắc nét)
            g_hFontNormal = CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
            g_hFontBold = CreateFontW(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                      DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                      CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
            g_hFontTitle = CreateFontW(22, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                       DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                       CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

            HINSTANCE hInst = ((LPCREATESTRUCT)lParam)->hInstance;

            // 1. HEADER BANNER
            HWND hTitle = CreateWindowExW(0, L"STATIC", L"HỆ THỐNG QUẢN LÝ ĐIỂM SINH VIÊN - HPU SCORE",
                                          WS_CHILD | WS_VISIBLE | SS_CENTER,
                                          20, 12, 980, 28, hwnd, NULL, hInst, NULL);
            SendMessageW(hTitle, WM_SETFONT, (WPARAM)g_hFontTitle, TRUE);

            HWND hSubTitle = CreateWindowExW(0, L"STATIC", L"Học phần: Nhập môn lập trình | Lớp: CNTT-K18 | Cố vấn học tập: Cô Lan | Tự động lưu: sinhvien.csv",
                                             WS_CHILD | WS_VISIBLE | SS_CENTER,
                                             20, 42, 980, 20, hwnd, NULL, hInst, NULL);
            SendMessageW(hSubTitle, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);

            // 2. KHUNG NHẬP LIỆU (LEFT GROUPBOX)
            HWND hGroupInput = CreateWindowExW(0, L"BUTTON", L" Nhập && Hiệu chỉnh thông tin sinh viên ",
                                               WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
                                               20, 68, 360, 362, hwnd, NULL, hInst, NULL);
            SendMessageW(hGroupInput, WM_SETFONT, (WPARAM)g_hFontBold, TRUE);

            // Nhãn và ô nhập: Mã SV
            HWND hLblMaSV = CreateWindowExW(0, L"STATIC", L"Mã sinh viên:", WS_CHILD | WS_VISIBLE,
                                            40, 100, 110, 22, hwnd, NULL, hInst, NULL);
            SendMessageW(hLblMaSV, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
            g_hEdtMaSV = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
                                         WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
                                         160, 97, 200, 24, hwnd, (HMENU)ID_EDT_MASV, hInst, NULL);
            SendMessageW(g_hEdtMaSV, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
            SendMessageW(g_hEdtMaSV, EM_LIMITTEXT, 15, 0);

            // Nhãn và ô nhập: Họ và tên
            HWND hLblHoTen = CreateWindowExW(0, L"STATIC", L"Họ và tên:", WS_CHILD | WS_VISIBLE,
                                             40, 137, 110, 22, hwnd, NULL, hInst, NULL);
            SendMessageW(hLblHoTen, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
            g_hEdtHoTen = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
                                          WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
                                          160, 134, 200, 24, hwnd, (HMENU)ID_EDT_HOTEN, hInst, NULL);
            SendMessageW(g_hEdtHoTen, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
            SendMessageW(g_hEdtHoTen, EM_LIMITTEXT, 63, 0);

            // Nhãn và ô nhập: Điểm Chuyên cần (10%)
            HWND hLblCC = CreateWindowExW(0, L"STATIC", L"Chuyên cần (10%):", WS_CHILD | WS_VISIBLE,
                                          40, 174, 115, 22, hwnd, NULL, hInst, NULL);
            SendMessageW(hLblCC, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
            g_hEdtDiemCC = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
                                           WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
                                           160, 171, 200, 24, hwnd, (HMENU)ID_EDT_DIEMCC, hInst, NULL);
            SendMessageW(g_hEdtDiemCC, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
            SendMessageW(g_hEdtDiemCC, EM_LIMITTEXT, 10, 0);

            // Nhãn và ô nhập: Điểm Giữa kỳ (30%)
            HWND hLblGK = CreateWindowExW(0, L"STATIC", L"Giữa kỳ (30%):", WS_CHILD | WS_VISIBLE,
                                          40, 211, 115, 22, hwnd, NULL, hInst, NULL);
            SendMessageW(hLblGK, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
            g_hEdtDiemGK = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
                                           WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
                                           160, 208, 200, 24, hwnd, (HMENU)ID_EDT_DIEMGK, hInst, NULL);
            SendMessageW(g_hEdtDiemGK, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
            SendMessageW(g_hEdtDiemGK, EM_LIMITTEXT, 10, 0);

            // Nhãn và ô nhập: Điểm Cuối kỳ (60%)
            HWND hLblCK = CreateWindowExW(0, L"STATIC", L"Cuối kỳ (60%):", WS_CHILD | WS_VISIBLE,
                                          40, 248, 115, 22, hwnd, NULL, hInst, NULL);
            SendMessageW(hLblCK, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
            g_hEdtDiemCK = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
                                           WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
                                           160, 245, 200, 24, hwnd, (HMENU)ID_EDT_DIEMCK, hInst, NULL);
            SendMessageW(g_hEdtDiemCK, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
            SendMessageW(g_hEdtDiemCK, EM_LIMITTEXT, 10, 0);

            // Hàng nút bấm 1: Thêm, Cập nhật, Xóa
            HWND hBtnAdd = CreateWindowExW(0, L"BUTTON", L"+ Thêm mới",
                                           WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
                                           40, 285, 100, 32, hwnd, (HMENU)ID_BTN_ADD, hInst, NULL);
            SendMessageW(hBtnAdd, WM_SETFONT, (WPARAM)g_hFontBold, TRUE);

            HWND hBtnUpdate = CreateWindowExW(0, L"BUTTON", L"Sửa điểm",
                                              WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
                                              148, 285, 100, 32, hwnd, (HMENU)ID_BTN_UPDATE, hInst, NULL);
            SendMessageW(hBtnUpdate, WM_SETFONT, (WPARAM)g_hFontBold, TRUE);

            HWND hBtnDelete = CreateWindowExW(0, L"BUTTON", L"Xóa SV",
                                              WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
                                              256, 285, 104, 32, hwnd, (HMENU)ID_BTN_DELETE, hInst, NULL);
            SendMessageW(hBtnDelete, WM_SETFONT, (WPARAM)g_hFontBold, TRUE);

            // Hàng nút bấm 2: Làm mới Form, Tìm kiếm
            HWND hBtnClear = CreateWindowExW(0, L"BUTTON", L"Làm mới Form",
                                             WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
                                             40, 325, 154, 30, hwnd, (HMENU)ID_BTN_CLEAR, hInst, NULL);
            SendMessageW(hBtnClear, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);

            HWND hBtnSearch = CreateWindowExW(0, L"BUTTON", L"Tìm theo mã",
                                              WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
                                              206, 325, 154, 30, hwnd, (HMENU)ID_BTN_SEARCH, hInst, NULL);
            SendMessageW(hBtnSearch, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);

            HWND hHint = CreateWindowExW(0, L"STATIC", L"* Gợi ý: Nhấp chuột vào một dòng trong bảng bên phải để nạp thông tin lên Form.",
                                         WS_CHILD | WS_VISIBLE | SS_LEFT,
                                         40, 365, 320, 50, hwnd, NULL, hInst, NULL);
            SendMessageW(hHint, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);

            // 3. KHUNG TÁC VỤ NÂNG CAO (BOTTOM-LEFT GROUPBOX)
            HWND hGroupAdv = CreateWindowExW(0, L"BUTTON", L" Quản lý Tệp tin && Nghiệp vụ ",
                                             WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
                                             20, 440, 360, 245, hwnd, NULL, hInst, NULL);
            SendMessageW(hGroupAdv, WM_SETFONT, (WPARAM)g_hFontBold, TRUE);

            // Hàng nút Lưu & Nạp tệp tin (File I/O)
            HWND hBtnSave = CreateWindowExW(0, L"BUTTON", L"Lưu tệp tin (CSV)",
                                            WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
                                            40, 468, 154, 32, hwnd, (HMENU)ID_BTN_SAVEFILE, hInst, NULL);
            SendMessageW(hBtnSave, WM_SETFONT, (WPARAM)g_hFontBold, TRUE);

            HWND hBtnLoad = CreateWindowExW(0, L"BUTTON", L"Nạp từ tệp tin",
                                            WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
                                            206, 468, 154, 32, hwnd, (HMENU)ID_BTN_LOADFILE, hInst, NULL);
            SendMessageW(hBtnLoad, WM_SETFONT, (WPARAM)g_hFontBold, TRUE);

            HWND hBtnSort = CreateWindowExW(0, L"BUTTON", L"Sắp xếp giảm dần theo Điểm TK",
                                            WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
                                            40, 508, 320, 32, hwnd, (HMENU)ID_BTN_SORT, hInst, NULL);
            SendMessageW(hBtnSort, WM_SETFONT, (WPARAM)g_hFontBold, TRUE);

            HWND hBtnStats = CreateWindowExW(0, L"BUTTON", L"Báo cáo thống kê && Danh sách học lại",
                                             WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
                                             40, 546, 320, 32, hwnd, (HMENU)ID_BTN_STATS, hInst, NULL);
            SendMessageW(hBtnStats, WM_SETFONT, (WPARAM)g_hFontBold, TRUE);

            HWND hBtnReload = CreateWindowExW(0, L"BUTTON", L"Nạp lại dữ liệu mẫu lớp CNTT-K18",
                                              WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
                                              40, 584, 320, 32, hwnd, (HMENU)ID_BTN_RELOAD, hInst, NULL);
            SendMessageW(hBtnReload, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);

            HWND hFormula = CreateWindowExW(0, L"STATIC", L"Công thức: Điểm TK = 10% CC + 30% GK + 60% CK",
                                            WS_CHILD | WS_VISIBLE | SS_CENTER,
                                            40, 626, 320, 20, hwnd, NULL, hInst, NULL);
            SendMessageW(hFormula, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);

            HWND hFileNote = CreateWindowExW(0, L"STATIC", L"Tệp dữ liệu: sinhvien.csv (mở được bằng Microsoft Excel)",
                                             WS_CHILD | WS_VISIBLE | SS_CENTER,
                                             40, 650, 320, 20, hwnd, NULL, hInst, NULL);
            SendMessageW(hFileNote, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);

            // 4. BẢNG DANH SÁCH SINH VIÊN (RIGHT LISTVIEW)
            HWND hGroupTable = CreateWindowExW(0, L"BUTTON", L" Bảng tổng hợp điểm học kỳ lớp học ",
                                               WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
                                               395, 68, 615, 617, hwnd, NULL, hInst, NULL);
            SendMessageW(hGroupTable, WM_SETFONT, (WPARAM)g_hFontBold, TRUE);

            g_hListView = CreateWindowExW(
                0, WC_LISTVIEWW, L"",
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS,
                408, 96, 590, 532, hwnd, (HMENU)ID_LISTVIEW, hInst, NULL
            );
            SendMessageW(g_hListView, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
            ListView_SetExtendedListViewStyle(g_hListView, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);

            // 8 cột ListView (Tổng 570px, chừa ~20px cho thanh cuộn dọc)
            LVCOLUMNW lvc = {0};
            lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM | LVCF_FMT;

            struct {
                const wchar_t *title;
                int width;
                int fmt;
            } cols[] = {
                {L"STT", 32, LVCFMT_CENTER},
                {L"Mã SV", 62, LVCFMT_LEFT},
                {L"Họ và tên", 130, LVCFMT_LEFT},
                {L"Điểm CC", 64, LVCFMT_RIGHT},
                {L"Điểm GK", 64, LVCFMT_RIGHT},
                {L"Điểm CK", 64, LVCFMT_RIGHT},
                {L"Điểm TK", 62, LVCFMT_RIGHT},
                {L"Xếp loại", 92, LVCFMT_CENTER}
            };

            for (int i = 0; i < 8; i++) {
                lvc.iSubItem = i;
                lvc.cx = cols[i].width;
                lvc.fmt = cols[i].fmt;
                lvc.pszText = (LPWSTR)cols[i].title;
                ListView_InsertColumn(g_hListView, i, &lvc);
            }

            // Thanh trạng thái dưới đáy bảng
            g_hStStatus = CreateWindowExW(0, L"STATIC", L"Sẵn sàng.",
                                          WS_CHILD | WS_VISIBLE | SS_LEFT,
                                          412, 642, 580, 25, hwnd, (HMENU)ID_ST_STATUS, hInst, NULL);
            SendMessageW(g_hStStatus, WM_SETFONT, (WPARAM)g_hFontBold, TRUE);

            // Tự động kiểm tra và nạp tệp sinhvien.csv nếu đã có sẵn
            DocFileGUI(DEFAULT_FILE);
            if (g_n == 0) {
                KhoiTaoDuLieuMauGUI();
                LuuFileGUI(DEFAULT_FILE); // Tự động tạo tệp lưu trữ lần đầu
            }
            CapNhatListView();
            CapNhatThanhTrangThai();
            break;
        }

        case WM_COMMAND: {
            int wmId = LOWORD(wParam);
            switch (wmId) {
                case ID_BTN_ADD:
                    XuLyThemSinhVien();
                    break;
                case ID_BTN_UPDATE:
                    XuLyCapNhatDiem();
                    break;
                case ID_BTN_DELETE:
                    XuLyXoaSinhVien();
                    break;
                case ID_BTN_CLEAR:
                    XoaTrangForm();
                    break;
                case ID_BTN_SEARCH:
                    XuLyTimKiem();
                    break;
                case ID_BTN_SORT:
                    XuLySapXepGiamDan();
                    break;
                case ID_BTN_STATS:
                    XuLyThongKe();
                    break;
                case ID_BTN_SAVEFILE:
                    LuuFileGUI(DEFAULT_FILE);
                    MessageBoxW(hwnd, L"Đã lưu an toàn toàn bộ dữ liệu vào tệp tin 'sinhvien.csv'!\nBạn có thể mở tệp này bằng Microsoft Excel.",
                                L"Lưu tệp thành công", MB_OK | MB_ICONINFORMATION);
                    break;
                case ID_BTN_LOADFILE:
                    DocFileGUI(DEFAULT_FILE);
                    CapNhatListView();
                    CapNhatThanhTrangThai();
                    XoaTrangForm();
                    MessageBoxW(hwnd, L"Đã nạp lại dữ liệu thành công từ tệp tin 'sinhvien.csv'!",
                                L"Nạp tệp thành công", MB_OK | MB_ICONINFORMATION);
                    break;
                case ID_BTN_RELOAD:
                    KhoiTaoDuLieuMauGUI();
                    LuuFileGUI(DEFAULT_FILE);
                    CapNhatListView();
                    CapNhatThanhTrangThai();
                    XoaTrangForm();
                    MessageBoxW(hwnd, L"Đã nạp lại dữ liệu mẫu lớp CNTT-K18 gồm 6 sinh viên ban đầu và lưu vào tệp!",
                                L"Thông báo", MB_OK | MB_ICONINFORMATION);
                    break;
            }
            break;
        }

        case WM_NOTIFY: {
            LPNMHDR lpnmh = (LPNMHDR)lParam;
            if (lpnmh->idFrom == ID_LISTVIEW) {
                if (lpnmh->code == NM_CLICK || lpnmh->code == LVN_ITEMCHANGED) {
                    int selected = ListView_GetNextItem(g_hListView, -1, LVNI_SELECTED);
                    if (selected != -1) {
                        XuLyChonItemListView(selected);
                    }
                }
            }
            break;
        }

        case WM_CLOSE: {
            // Tự động lưu dữ liệu trước khi thoát
            LuuFileGUI(DEFAULT_FILE);
            DestroyWindow(hwnd);
            break;
        }

        case WM_DESTROY: {
            if (g_hFontNormal) DeleteObject(g_hFontNormal);
            if (g_hFontBold) DeleteObject(g_hFontBold);
            if (g_hFontTitle) DeleteObject(g_hFontTitle);
            PostQuitMessage(0);
            break;
        }

        default:
            return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    return 0;
}

/* ================= HIỆN THỰC CÁC HÀM LƯU & ĐỌC TỆP TIN GUI (FILE I/O) ================= */

/**
 * Lưu danh sách sinh viên GUI ra tệp tin CSV (UTF-8 có BOM)
 */
void LuuFileGUI(const wchar_t *tenFile) {
    if (!tenFile || wcslen(tenFile) == 0) {
        tenFile = DEFAULT_FILE;
    }

    FILE *f = _wfopen(tenFile, L"wb");
    if (!f) return;

    // Ghi UTF-8 BOM
    fputs("\xEF\xBB\xBF", f);

    // Ghi dòng tiêu đề
    fprintf(f, "STT,Mã SV,Họ và tên,Điểm CC,Điểm GK,Điểm CK,Điểm TK,Xếp loại\r\n");

    for (int i = 0; i < g_n; i++) {
        char maU8[64] = {0};
        char tenU8[256] = {0};
        char xlU8[128] = {0};

        WideCharToMultiByte(CP_UTF8, 0, g_ds[i].maSV, -1, maU8, sizeof(maU8), NULL, NULL);
        WideCharToMultiByte(CP_UTF8, 0, g_ds[i].hoTen, -1, tenU8, sizeof(tenU8), NULL, NULL);
        WideCharToMultiByte(CP_UTF8, 0, g_ds[i].xepLoai, -1, xlU8, sizeof(xlU8), NULL, NULL);

        fprintf(f, "%d,%s,%s,%.1f,%.1f,%.1f,%.1f,%s\r\n",
                i + 1, maU8, tenU8,
                g_ds[i].diemCC, g_ds[i].diemGK, g_ds[i].diemCK, g_ds[i].diemTK, xlU8);
    }

    fclose(f);
}

/**
 * Đọc danh sách sinh viên từ tệp tin CSV vào bộ nhớ GUI
 */
void DocFileGUI(const wchar_t *tenFile) {
    if (!tenFile || wcslen(tenFile) == 0) {
        tenFile = DEFAULT_FILE;
    }

    FILE *f = _wfopen(tenFile, L"rb");
    if (!f) return;

    char line[512];
    // Đọc bỏ qua dòng header
    if (!fgets(line, sizeof(line), f)) {
        fclose(f);
        return;
    }

    int count = 0;
    while (fgets(line, sizeof(line), f) && count < MAX) {
        // Cắt bỏ ký tự xuống dòng
        int len = (int)strlen(line);
        while (len > 0 && (line[len - 1] == '\r' || line[len - 1] == '\n' || line[len - 1] == ' ')) {
            line[--len] = '\0';
        }
        if (len == 0) continue;

        char *tokenSTT = strtok(line, ",");
        char *tokenMa  = strtok(NULL, ",");
        char *tokenTen = strtok(NULL, ",");
        char *tokenCC  = strtok(NULL, ",");
        char *tokenGK  = strtok(NULL, ",");
        char *tokenCK  = strtok(NULL, ",");
        (void)tokenSTT;

        if (tokenMa && tokenTen && tokenCC && tokenGK && tokenCK) {
            wchar_t wMa[16] = {0};
            wchar_t wTen[64] = {0};

            MultiByteToWideChar(CP_UTF8, 0, tokenMa, -1, wMa, 16);
            MultiByteToWideChar(CP_UTF8, 0, tokenTen, -1, wTen, 64);
            TrimW(wMa);
            TrimW(wTen);

            if (wcslen(wMa) > 0 && wcslen(wTen) > 0) {
                wcscpy(g_ds[count].maSV, wMa);
                wcscpy(g_ds[count].hoTen, wTen);
                g_ds[count].diemCC = (float)atof(tokenCC);
                g_ds[count].diemGK = (float)atof(tokenGK);
                g_ds[count].diemCK = (float)atof(tokenCK);

                TinhDiemGUI(&g_ds[count]);
                count++;
            }
        }
    }

    fclose(f);
    if (count > 0) {
        g_n = count;
    }
}

/* ================= HIỆN THỰC CÁC HÀM TIỆN ÍCH CHUỖI & DỮ LIỆU ================= */

void TrimW(wchar_t *s) {
    if (!s) return;
    int len = (int)wcslen(s);
    while (len > 0 && (s[len - 1] == L' ' || s[len - 1] == L'\t' || s[len - 1] == L'\r' || s[len - 1] == L'\n')) {
        s[--len] = L'\0';
    }
    int start = 0;
    while (s[start] == L' ' || s[start] == L'\t') {
        start++;
    }
    if (start > 0) {
        int i = 0;
        while (s[start + i] != L'\0') {
            s[i] = s[start + i];
            i++;
        }
        s[i] = L'\0';
    }
}

int ParseDiemW(const wchar_t *strInput, float *outDiem) {
    if (!strInput || !outDiem) return 0;

    wchar_t temp[32];
    wcsncpy(temp, strInput, 31);
    temp[31] = L'\0';
    TrimW(temp);

    if (wcslen(temp) == 0) return 0;

    for (int i = 0; temp[i] != L'\0'; i++) {
        if (temp[i] == L',') {
            temp[i] = L'.';
        }
    }

    wchar_t *endPtr = NULL;
    float val = wcstof(temp, &endPtr);

    if (endPtr == temp || *endPtr != L'\0') {
        return 0;
    }

    if (!isfinite(val) || val < 0.0f || val > 10.0f) {
        return 0;
    }

    *outDiem = val;
    return 1;
}

void TinhDiemGUI(SinhVienGUI *sv) {
    if (!sv) return;
    float tk = (0.10f * sv->diemCC) + (0.30f * sv->diemGK) + (0.60f * sv->diemCK);
    sv->diemTK = roundf(tk * 10.0f) / 10.0f;

    if (sv->diemTK >= 8.0f) {
        wcscpy(sv->xepLoai, L"Giỏi");
    } else if (sv->diemTK >= 6.5f) {
        wcscpy(sv->xepLoai, L"Khá");
    } else if (sv->diemTK >= 5.0f) {
        wcscpy(sv->xepLoai, L"Trung bình");
    } else if (sv->diemTK >= 4.0f) {
        wcscpy(sv->xepLoai, L"Yếu");
    } else {
        wcscpy(sv->xepLoai, L"Kém (Học lại)");
    }
}

void KhoiTaoDuLieuMauGUI(void) {
    SinhVienGUI mau[] = {
        {L"SV001", L"Nguyễn Văn An",       9.0f,  7.0f,  8.0f, 0.0f, L""}, // 7.8 -> Khá
        {L"SV002", L"Trần Thị Bình",      10.0f,  3.0f,  3.0f, 0.0f, L""}, // 3.7 -> Kém (học lại)
        {L"SV003", L"Lê Hoàng Cường",      9.5f,  9.0f,  8.5f, 0.0f, L""}, // 8.8 -> Giỏi
        {L"SV004", L"Phạm Minh Đức",       8.0f,  6.0f,  5.5f, 0.0f, L""}, // 5.9 -> Trung bình
        {L"SV005", L"Hoàng Ngọc Điệp",     7.0f,  4.5f,  4.0f, 0.0f, L""}, // 4.5 -> Yếu
        {L"SV006", L"Vũ Thị Hoa",         10.0f,  9.5f,  9.0f, 0.0f, L""}  // 9.3 -> Giỏi (Thủ khoa)
    };

    g_n = sizeof(mau) / sizeof(mau[0]);
    for (int i = 0; i < g_n; i++) {
        g_ds[i] = mau[i];
        TinhDiemGUI(&g_ds[i]);
    }
}

void CapNhatListView(void) {
    ListView_DeleteAllItems(g_hListView);

    for (int i = 0; i < g_n; i++) {
        LVITEMW lvi = {0};
        lvi.mask = LVIF_TEXT;
        lvi.iItem = i;
        lvi.iSubItem = 0;

        wchar_t buf[32];
        swprintf(buf, 32, L"%d", i + 1);
        lvi.pszText = buf;
        ListView_InsertItem(g_hListView, &lvi);

        ListView_SetItemText(g_hListView, i, 1, g_ds[i].maSV);
        ListView_SetItemText(g_hListView, i, 2, g_ds[i].hoTen);

        swprintf(buf, 32, L"%.1f", g_ds[i].diemCC);
        ListView_SetItemText(g_hListView, i, 3, buf);

        swprintf(buf, 32, L"%.1f", g_ds[i].diemGK);
        ListView_SetItemText(g_hListView, i, 4, buf);

        swprintf(buf, 32, L"%.1f", g_ds[i].diemCK);
        ListView_SetItemText(g_hListView, i, 5, buf);

        swprintf(buf, 32, L"%.1f", g_ds[i].diemTK);
        ListView_SetItemText(g_hListView, i, 6, buf);

        ListView_SetItemText(g_hListView, i, 7, g_ds[i].xepLoai);
    }
}

void CapNhatThanhTrangThai(void) {
    if (g_n == 0) {
        SetWindowTextW(g_hStStatus, L"[*] Danh sách hiện đang rỗng. Hãy thêm sinh viên mới!");
        return;
    }

    float maxTK = g_ds[0].diemTK;
    wchar_t thuKhoa[64] = L"";
    int countHocLai = 0;

    for (int i = 0; i < g_n; i++) {
        if (g_ds[i].diemTK > maxTK) {
            maxTK = g_ds[i].diemTK;
        }
        if (g_ds[i].diemTK < 4.0f) {
            countHocLai++;
        }
    }

    for (int i = 0; i < g_n; i++) {
        if (g_ds[i].diemTK == maxTK) {
            wcscpy(thuKhoa, g_ds[i].hoTen);
            break;
        }
    }

    wchar_t msg[256];
    swprintf(msg, 256, L"[*] Tổng số: %d SV  |  [Thủ khoa] %ls (%.1f điểm)  |  [Học lại] %d sinh viên  |  Đã lưu tệp CSV",
             g_n, thuKhoa, maxTK, countHocLai);
    SetWindowTextW(g_hStStatus, msg);
}

void XoaTrangForm(void) {
    SetWindowTextW(g_hEdtMaSV, L"");
    SetWindowTextW(g_hEdtHoTen, L"");
    SetWindowTextW(g_hEdtDiemCC, L"");
    SetWindowTextW(g_hEdtDiemGK, L"");
    SetWindowTextW(g_hEdtDiemCK, L"");
    SetFocus(g_hEdtMaSV);
}

int KiemTraTrungMaGUI(const wchar_t *ma, int excludeIndex) {
    for (int i = 0; i < g_n; i++) {
        if (i == excludeIndex) continue;
        if (_wcsicmp(g_ds[i].maSV, ma) == 0) {
            return 1;
        }
    }
    return 0;
}

SinhVienGUI* TimKiemTheoMaGUI(const wchar_t *ma, int *outIndex) {
    for (int i = 0; i < g_n; i++) {
        if (_wcsicmp(g_ds[i].maSV, ma) == 0) {
            if (outIndex) *outIndex = i;
            return &g_ds[i];
        }
    }
    if (outIndex) *outIndex = -1;
    return NULL;
}

void XuLyChonItemListView(int itemIndex) {
    if (itemIndex < 0 || itemIndex >= g_n) return;

    SetWindowTextW(g_hEdtMaSV, g_ds[itemIndex].maSV);
    SetWindowTextW(g_hEdtHoTen, g_ds[itemIndex].hoTen);

    wchar_t buf[32];
    swprintf(buf, 32, L"%.1f", g_ds[itemIndex].diemCC);
    SetWindowTextW(g_hEdtDiemCC, buf);

    swprintf(buf, 32, L"%.1f", g_ds[itemIndex].diemGK);
    SetWindowTextW(g_hEdtDiemGK, buf);

    swprintf(buf, 32, L"%.1f", g_ds[itemIndex].diemCK);
    SetWindowTextW(g_hEdtDiemCK, buf);
}

void XuLyThemSinhVien(void) {
    if (g_n >= MAX) {
        MessageBoxW(g_hMainWnd, L"Danh sách sinh viên đã đạt giới hạn tối đa (100 sinh viên)!",
                    L"Cảnh báo", MB_OK | MB_ICONWARNING);
        return;
    }

    wchar_t ma[16], hoTen[64], strCC[32], strGK[32], strCK[32];
    GetWindowTextW(g_hEdtMaSV, ma, 16);
    GetWindowTextW(g_hEdtHoTen, hoTen, 64);
    GetWindowTextW(g_hEdtDiemCC, strCC, 32);
    GetWindowTextW(g_hEdtDiemGK, strGK, 32);
    GetWindowTextW(g_hEdtDiemCK, strCK, 32);

    TrimW(ma);
    TrimW(hoTen);

    if (wcslen(ma) == 0) {
        MessageBoxW(g_hMainWnd, L"Vui lòng nhập Mã sinh viên!", L"Lỗi dữ liệu", MB_OK | MB_ICONWARNING);
        SetFocus(g_hEdtMaSV);
        return;
    }
    if (wcslen(hoTen) == 0) {
        MessageBoxW(g_hMainWnd, L"Vui lòng nhập Họ và tên sinh viên!", L"Lỗi dữ liệu", MB_OK | MB_ICONWARNING);
        SetFocus(g_hEdtHoTen);
        return;
    }

    if (KiemTraTrungMaGUI(ma, -1)) {
        wchar_t msg[128];
        swprintf(msg, 128, L"Mã sinh viên '%ls' đã tồn tại trong danh sách! Vui lòng chọn mã khác.", ma);
        MessageBoxW(g_hMainWnd, msg, L"Trùng mã sinh viên", MB_OK | MB_ICONERROR);
        SetFocus(g_hEdtMaSV);
        return;
    }

    float cc = 0.0f, gk = 0.0f, ck = 0.0f;
    if (!ParseDiemW(strCC, &cc)) {
        MessageBoxW(g_hMainWnd, L"Điểm Chuyên cần không hợp lệ! Điểm phải là số thực từ 0.0 đến 10.0.",
                    L"Lỗi điểm số", MB_OK | MB_ICONWARNING);
        SetFocus(g_hEdtDiemCC);
        return;
    }

    if (!ParseDiemW(strGK, &gk)) {
        MessageBoxW(g_hMainWnd, L"Điểm Giữa kỳ không hợp lệ! Điểm phải là số thực từ 0.0 đến 10.0.",
                    L"Lỗi điểm số", MB_OK | MB_ICONWARNING);
        SetFocus(g_hEdtDiemGK);
        return;
    }

    if (!ParseDiemW(strCK, &ck)) {
        MessageBoxW(g_hMainWnd, L"Điểm Cuối kỳ không hợp lệ! Điểm phải là số thực từ 0.0 đến 10.0.",
                    L"Lỗi điểm số", MB_OK | MB_ICONWARNING);
        SetFocus(g_hEdtDiemCK);
        return;
    }

    int index = g_n;
    wcscpy(g_ds[index].maSV, ma);
    wcscpy(g_ds[index].hoTen, hoTen);
    g_ds[index].diemCC = cc;
    g_ds[index].diemGK = gk;
    g_ds[index].diemCK = ck;

    TinhDiemGUI(&g_ds[index]);
    g_n++;

    // Tự động lưu dữ liệu vĩnh viễn vào tệp tin CSV
    LuuFileGUI(DEFAULT_FILE);

    CapNhatListView();
    CapNhatThanhTrangThai();
    XoaTrangForm();

    ListView_SetItemState(g_hListView, index, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
    ListView_EnsureVisible(g_hListView, index, FALSE);

    MessageBoxW(g_hMainWnd, L"Đã thêm sinh viên mới thành công và tự động lưu vào tệp sinhvien.csv!",
                L"Thành công", MB_OK | MB_ICONINFORMATION);
}

void XuLyCapNhatDiem(void) {
    wchar_t ma[16];
    GetWindowTextW(g_hEdtMaSV, ma, 16);
    TrimW(ma);

    if (wcslen(ma) == 0) {
        MessageBoxW(g_hMainWnd, L"Vui lòng chọn hoặc nhập Mã sinh viên cần cập nhật điểm!",
                    L"Thông báo", MB_OK | MB_ICONWARNING);
        return;
    }

    int viTri = -1;
    SinhVienGUI *sv = TimKiemTheoMaGUI(ma, &viTri);
    if (!sv) {
        MessageBoxW(g_hMainWnd, L"Không tìm thấy sinh viên có mã này trong danh sách!",
                    L"Không tìm thấy", MB_OK | MB_ICONERROR);
        return;
    }

    wchar_t hoTen[64], strCC[32], strGK[32], strCK[32];
    GetWindowTextW(g_hEdtHoTen, hoTen, 64);
    GetWindowTextW(g_hEdtDiemCC, strCC, 32);
    GetWindowTextW(g_hEdtDiemGK, strGK, 32);
    GetWindowTextW(g_hEdtDiemCK, strCK, 32);

    TrimW(hoTen);
    float cc = 0.0f, gk = 0.0f, ck = 0.0f;

    if (!ParseDiemW(strCC, &cc)) {
        MessageBoxW(g_hMainWnd, L"Điểm Chuyên cần mới không hợp lệ [0; 10]!", L"Lỗi", MB_OK | MB_ICONWARNING);
        SetFocus(g_hEdtDiemCC);
        return;
    }

    if (!ParseDiemW(strGK, &gk)) {
        MessageBoxW(g_hMainWnd, L"Điểm Giữa kỳ mới không hợp lệ [0; 10]!", L"Lỗi", MB_OK | MB_ICONWARNING);
        SetFocus(g_hEdtDiemGK);
        return;
    }

    if (!ParseDiemW(strCK, &ck)) {
        MessageBoxW(g_hMainWnd, L"Điểm Cuối kỳ mới không hợp lệ [0; 10]!", L"Lỗi", MB_OK | MB_ICONWARNING);
        SetFocus(g_hEdtDiemCK);
        return;
    }

    if (wcslen(hoTen) > 0) {
        wcscpy(sv->hoTen, hoTen);
    }
    sv->diemCC = cc;
    sv->diemGK = gk;
    sv->diemCK = ck;

    TinhDiemGUI(sv);

    // Tự động lưu dữ liệu cập nhật vào tệp tin CSV
    LuuFileGUI(DEFAULT_FILE);

    CapNhatListView();
    CapNhatThanhTrangThai();

    ListView_SetItemState(g_hListView, viTri, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
    ListView_EnsureVisible(g_hListView, viTri, FALSE);

    MessageBoxW(g_hMainWnd, L"Đã cập nhật lại điểm sinh viên và lưu vào tệp sinhvien.csv thành công!",
                L"Thành công", MB_OK | MB_ICONINFORMATION);
}

void XuLyXoaSinhVien(void) {
    wchar_t ma[16];
    GetWindowTextW(g_hEdtMaSV, ma, 16);
    TrimW(ma);

    if (wcslen(ma) == 0) {
        int selected = ListView_GetNextItem(g_hListView, -1, LVNI_SELECTED);
        if (selected != -1) {
            wcscpy(ma, g_ds[selected].maSV);
        } else {
            MessageBoxW(g_hMainWnd, L"Vui lòng chọn sinh viên cần xóa từ bảng hoặc nhập mã SV!",
                        L"Thông báo", MB_OK | MB_ICONWARNING);
            return;
        }
    }

    int viTri = -1;
    SinhVienGUI *sv = TimKiemTheoMaGUI(ma, &viTri);
    if (!sv) {
        MessageBoxW(g_hMainWnd, L"Không tìm thấy sinh viên có mã này để xóa!", L"Lỗi", MB_OK | MB_ICONERROR);
        return;
    }

    wchar_t confirmMsg[256];
    swprintf(confirmMsg, 256, L"Bạn có chắc chắn muốn xóa sinh viên sau không?\n\n- Mã SV: %ls\n- Họ tên: %ls\n- Điểm TK: %.1f (%ls)",
             sv->maSV, sv->hoTen, sv->diemTK, sv->xepLoai);

    if (MessageBoxW(g_hMainWnd, confirmMsg, L"Xác nhận xóa", MB_YESNO | MB_ICONQUESTION) == IDYES) {
        for (int i = viTri; i < g_n - 1; i++) {
            g_ds[i] = g_ds[i + 1];
        }
        g_n--;

        // Tự động lưu sau khi xóa
        LuuFileGUI(DEFAULT_FILE);

        CapNhatListView();
        CapNhatThanhTrangThai();
        XoaTrangForm();

        MessageBoxW(g_hMainWnd, L"Đã xóa sinh viên và cập nhật tệp tin thành công!", L"Đã xóa", MB_OK | MB_ICONINFORMATION);
    }
}

void XuLySapXepGiamDan(void) {
    if (g_n <= 1) {
        MessageBoxW(g_hMainWnd, L"Danh sách có ít hơn 2 sinh viên, không cần sắp xếp!", L"Thông báo", MB_OK);
        return;
    }

    for (int i = 0; i < g_n - 1; i++) {
        for (int j = i + 1; j < g_n; j++) {
            if (g_ds[i].diemTK < g_ds[j].diemTK) {
                SinhVienGUI temp = g_ds[i];
                g_ds[i] = g_ds[j];
                g_ds[j] = temp;
            }
        }
    }

    // Tự động lưu thứ tự đã sắp xếp vào tệp tin CSV
    LuuFileGUI(DEFAULT_FILE);

    CapNhatListView();
    CapNhatThanhTrangThai();

    MessageBoxW(g_hMainWnd, L"Đã sắp xếp danh sách sinh viên giảm dần theo điểm tổng kết và lưu vào tệp!",
                L"Sắp xếp thành công", MB_OK | MB_ICONINFORMATION);
}

void XuLyTimKiem(void) {
    wchar_t ma[16];
    GetWindowTextW(g_hEdtMaSV, ma, 16);
    TrimW(ma);

    if (wcslen(ma) == 0) {
        MessageBoxW(g_hMainWnd, L"Vui lòng nhập Mã sinh viên cần tìm vào ô 'Mã sinh viên'!",
                    L"Tìm kiếm", MB_OK | MB_ICONWARNING);
        SetFocus(g_hEdtMaSV);
        return;
    }

    int viTri = -1;
    SinhVienGUI *sv = TimKiemTheoMaGUI(ma, &viTri);
    if (!sv) {
        wchar_t msg[128];
        swprintf(msg, 128, L"Không tìm thấy sinh viên nào có mã '%ls'!", ma);
        MessageBoxW(g_hMainWnd, msg, L"Kết quả tìm kiếm", MB_OK | MB_ICONINFORMATION);
        return;
    }

    XuLyChonItemListView(viTri);
    ListView_SetItemState(g_hListView, -1, 0, LVIS_SELECTED);
    ListView_SetItemState(g_hListView, viTri, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
    ListView_EnsureVisible(g_hListView, viTri, FALSE);

    wchar_t msg[256];
    swprintf(msg, 256, L"Đã tìm thấy sinh viên!\n\n- Mã SV: %ls\n- Họ tên: %ls\n- Điểm TK: %.1f\n- Xếp loại: %ls",
             sv->maSV, sv->hoTen, sv->diemTK, sv->xepLoai);
    MessageBoxW(g_hMainWnd, msg, L"Tìm kiếm thành công", MB_OK | MB_ICONINFORMATION);
}

void XuLyThongKe(void) {
    if (g_n == 0) {
        MessageBoxW(g_hMainWnd, L"Danh sách hiện đang rỗng, không thể thống kê!", L"Thông báo", MB_OK);
        return;
    }

    int demGioi = 0, demKha = 0, demTB = 0, demYeu = 0, demKem = 0;
    float maxTK = g_ds[0].diemTK;

    for (int i = 0; i < g_n; i++) {
        if (g_ds[i].diemTK > maxTK) maxTK = g_ds[i].diemTK;

        if (wcscmp(g_ds[i].xepLoai, L"Giỏi") == 0) demGioi++;
        else if (wcscmp(g_ds[i].xepLoai, L"Khá") == 0) demKha++;
        else if (wcscmp(g_ds[i].xepLoai, L"Trung bình") == 0) demTB++;
        else if (wcscmp(g_ds[i].xepLoai, L"Yếu") == 0) demYeu++;
        else demKem++;
    }

    wchar_t baoCao[2048];
    int len = 0;
    int written = swprintf(baoCao, 2048,
        L"===== BÁO CÁO THỐNG KÊ LỚP CNTT-K18 =====\n\n"
        L"[*] Tổng số sinh viên hiện có: %d sinh viên\n\n"
        L"1. PHÂN BỐ XẾP LOẠI HỌC LỰC:\n"
        L"   - Giỏi (>= 8.0)        : %d sinh viên (%.1f%%)\n"
        L"   - Khá (>= 6.5)         : %d sinh viên (%.1f%%)\n"
        L"   - Trung bình (>= 5.0)  : %d sinh viên (%.1f%%)\n"
        L"   - Yếu (>= 4.0)         : %d sinh viên (%.1f%%)\n"
        L"   - Kém (< 4.0 - Học lại): %d sinh viên (%.1f%%)\n\n"
        L"2. DANH SÁCH THỦ KHOA (%.1f ĐIỂM):\n",
        g_n,
        demGioi, (demGioi * 100.0f) / g_n,
        demKha,  (demKha  * 100.0f) / g_n,
        demTB,   (demTB   * 100.0f) / g_n,
        demYeu,  (demYeu  * 100.0f) / g_n,
        demKem,  (demKem  * 100.0f) / g_n,
        maxTK
    );
    if (written > 0 && written < 2048) len += written;

    int demTK = 0;
    for (int i = 0; i < g_n; i++) {
        if (g_ds[i].diemTK == maxTK) {
            demTK++;
            written = swprintf(baoCao + len, 2048 - len, L"   [%d] %ls (%ls) - Điểm TK: %.1f\n",
                               demTK, g_ds[i].hoTen, g_ds[i].maSV, g_ds[i].diemTK);
            if (written > 0 && len + written < 2048) len += written;
            else break;
        }
    }

    written = swprintf(baoCao + len, 2048 - len, L"\n3. SINH VIÊN TRƯỢT MÔN (HỌC LẠI - TK < 4.0):\n");
    if (written > 0 && len + written < 2048) len += written;

    if (demKem == 0) {
        written = swprintf(baoCao + len, 2048 - len, L"   -> Tuyệt vời! Không có sinh viên nào phải học lại.\n");
        if (written > 0 && len + written < 2048) len += written;
    } else {
        int demHL = 0;
        for (int i = 0; i < g_n; i++) {
            if (g_ds[i].diemTK < 4.0f) {
                demHL++;
                written = swprintf(baoCao + len, 2048 - len, L"   [%d] %ls (%ls) - Điểm TK: %.1f [Học lại]\n",
                                   demHL, g_ds[i].hoTen, g_ds[i].maSV, g_ds[i].diemTK);
                if (written > 0 && len + written < 2048) len += written;
                else break;
            }
        }
    }

    MessageBoxW(g_hMainWnd, baoCao, L"Báo cáo thống kê tổng hợp", MB_OK | MB_ICONINFORMATION);
}
