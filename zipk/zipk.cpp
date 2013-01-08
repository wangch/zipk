// zipk.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "zipk.h"
#include "button.h"
#include "picex.h"
#include "resource.h"
#include "Commdlg.h"
#include "unzip.h"
#include "utf8conv.h"
#include <string>
#include <vector>


#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: 在此放置代码。
	MSG msg;
	HACCEL hAccelTable;

   ULONG_PTR gdiplusToken = 0;
   Gdiplus::GdiplusStartupInput gdiplusStartupInput;
   Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_ZIPK, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ZIPK));

	// 主消息循环:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

   Gdiplus::GdiplusShutdown(gdiplusToken);

	return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ZIPK));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_ZIPK);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

struct LV {
   HWND pw;
   HWND toolbar;
   HWND lv;
   Gdiplus::Font* f;
   Gdiplus::Font* f1;
   bool moving;
   Button btns[4];
   std::vector<unzFile> ufs;
   PicEx pe;
} g_lv;

static int dsp_content(unzFile uf) {
   return 0;
}

static int btn_click0(Button* btn) {
	OPENFILENAME ofn;       // common dialog box structure
	wchar_t szFile[260];       // buffer for file name

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
   ofn.hwndOwner = g_lv.pw;
	ofn.lpstrFile = szFile;
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	ofn.lpstrFile[0] = L'\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"zip file\0*.zip\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Display the Open dialog box. 
	if (::GetOpenFileName(&ofn) == TRUE) {
      std::string path = utf8util::UTF8FromUTF16(ofn.lpstrFile);
      unzFile uf = unzOpen(path.c_str());
      if (!uf) {
         ::MessageBox(0, L"zip 文件错误", L"错误", 0);
         return -1;
      }
      g_lv.ufs.push_back(uf);
      dsp_content(uf);
   }
   return 0;
}

static int btn_click1(Button* btn) {
   int n = g_lv.ufs.size();
   if (n <= 1) {
      return 0;
   }
   g_lv.ufs.pop_back();
   unzFile uf = g_lv.ufs[n-1];
   dsp_content(uf);
    
   return 0;
}

static int btn_click2(Button* btn) {
   return 0;
}

static int btn_click3(Button* btn) {
   int style = ::SendMessage(g_lv.lv, LVM_GETVIEW, 0, 0);
   if (style == LV_VIEW_DETAILS) {
      ::SendMessage(g_lv.lv, LVM_SETVIEW, LV_VIEW_ICON, 0);
   } else if (style == LV_VIEW_ICON) {
      ::SendMessage(g_lv.lv, LVM_SETVIEW, LVS_SMALLICON, 0);
   } else if (style == LVS_SMALLICON) {
      ::SendMessage(g_lv.lv, LVM_SETVIEW, LV_VIEW_LIST, 0);
   } else if (style == LV_VIEW_LIST) {
      ::SendMessage(g_lv.lv, LVM_SETVIEW, LV_VIEW_DETAILS, 0);
   }
   return 0;
}

static int init(HWND pw) {
   InitCommonControls();
   HWND hw = ::CreateWindowEx(0, WC_LISTVIEW, TEXT(""),
      WS_CHILD | WS_VISIBLE | TVS_HASLINES, 
      18, 84, 400, 225, pw, (HMENU)IDC_TV, hInst, 0);
   if (!hw) {
      return -1;
   }

   Gdiplus::FontFamily ff(L"宋体");
   Gdiplus::Font* f = new Gdiplus::Font(&ff, 9);
   Gdiplus::Font* f1 = new Gdiplus::Font(&ff, 12, 4);

   g_lv.btns[0].Init(0, pw, hInst, btn_click0);
   g_lv.btns[1].Init(1, pw, hInst, btn_click1);
   g_lv.btns[2].Init(2, pw, hInst, btn_click2);
   g_lv.btns[3].Init(3, pw, hInst, btn_click3);
           
   g_lv.btns[0].SetTxtFont(f);
   g_lv.btns[1].SetTxtFont(f);
   g_lv.btns[2].SetTxtFont(f);
   g_lv.btns[3].SetTxtFont(f);

   g_lv.btns[0].SetBkColor(Gdiplus::Color((69+96)/2, (110+141)/2, (149+198)/2));
   g_lv.btns[1].SetBkColor(Gdiplus::Color((69+96)/2, (110+141)/2, (149+198)/2));
   g_lv.btns[2].SetBkColor(Gdiplus::Color((69+96)/2, (110+141)/2, (149+198)/2));
   g_lv.btns[3].SetBkColor(Gdiplus::Color((69+96)/2, (110+141)/2, (149+198)/2));
           
   g_lv.btns[0].SetImg(L"res\\folder_open.png", 0, 0, 0, 24, 24);
   g_lv.btns[1].SetImg(L"res\\up.png", 0, 0, 0, 24, 24);
   g_lv.btns[2].SetImg(L"res\\home.png", 0, 0, 0, 24, 24);
   g_lv.btns[3].SetImg(L"res\\group.png", 0, 0, 0, 24, 24);

   g_lv.btns[0].SetImg(L"res\\folder_open.png", 1, 0, 0, 24, 24);
   g_lv.btns[1].SetImg(L"res\\up.png", 1, 0, 0, 24, 24);
   g_lv.btns[2].SetImg(L"res\\home.png", 1, 0, 0, 24, 24);
   g_lv.btns[3].SetImg(L"res\\group.png", 1, 0, 0, 24, 24);

   g_lv.btns[0].SetText(L"打開", 30, 6);
   g_lv.btns[1].SetText(L"向上", 30, 6);
   g_lv.btns[2].SetText(L"主页", 30, 6);
   g_lv.btns[3].SetText(L"视图", 30, 6);

   g_lv.pw = pw;
   g_lv.lv = hw;
   g_lv.f = f;
   g_lv.f1 = f1;
   g_lv.moving = false;

   //g_lv.pe.Init(pw, hInst, IDC_PICEX);
   return 0;
}

static void do_draw(HWND hWnd, HDC dc) {
   RECT rc;
   ::GetClientRect(hWnd, &rc);
   int x = rc.left;
   int y = rc.top;
   int w = rc.right - rc.left;
   int h = rc.bottom - rc.top;

   HBITMAP memmap = ::CreateCompatibleBitmap(dc, w, h);
   HDC mdc = ::CreateCompatibleDC(dc); 
   HBITMAP oldmap = (HBITMAP)::SelectObject(mdc, (HGDIOBJ)memmap);

   Gdiplus::Graphics* g = new Gdiplus::Graphics(mdc);

   Gdiplus::RectF grc(x, y, w, h);
   Gdiplus::LinearGradientBrush lbr(grc, 
      Gdiplus::Color(69, 110, 149), Gdiplus::Color(96, 141, 198), 0);
   g->FillRectangle(&lbr, x, y, w, h);

   int top_h = 64;
   int bar_h = 32;
   int bar_w = 375;

   g->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
   Gdiplus::GraphicsPath path;
   path.AddArc(60, (top_h-bar_h)/2, 33, 32, -240, 120);
   path.AddArc((60+bar_w), (top_h-bar_h)/2, 33, 32, -60, 120);
   path.CloseFigure();

   Gdiplus::Color line_color(133, 144, 160);
   Gdiplus::Pen pen(line_color, 1);
   Gdiplus::SolidBrush sbr(Gdiplus::Color((69+96)/2, (110+141)/2, (149+198)/2));
   g->DrawPath(&pen, &path);
   g->FillPath(&sbr, &path);

   BitBlt(dc, 
      x, y, w, h, 
      mdc, x, y, SRCCOPY);
   delete g;
   ::SelectObject(mdc, oldmap);
   ::DeleteObject(memmap); 
   ::DeleteDC(mdc);
}

static void do_size(int w, int h) {
   ::MoveWindow(g_lv.lv, 18, 84, w - 18, h-102, TRUE);

   int btn_top = 16+3;
   int w0 = 100;

   g_lv.btns[0].Show(w0, btn_top, 60, 26);
   g_lv.btns[1].Show(w0+80, btn_top, 60, 26);
   g_lv.btns[2].Show(w0+80+80, btn_top, 60, 26);
   g_lv.btns[3].Show(w0+80+80+80, btn_top, 60, 26);
};

static void do_mm(int x, int y) {
   if ((x > 18 && x < 80) && (y > 18 && y < 48)) {
      g_lv.moving = true;
   } else {
      g_lv.moving = false;
   }
   //::UpdateWindow(g_lv.pw);
   RECT rc;
   ::GetClientRect(g_lv.pw, &rc);
   rc.bottom = 48;
   ::InvalidateRect(g_lv.pw, &rc, TRUE);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 将实例句柄存储在全局变量中

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }
   ::SetMenu(hWnd, 0);
   init(hWnd);
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: 处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此添加任意绘图代码...
      do_draw(hWnd, hdc);
		EndPaint(hWnd, &ps);
		break;
   case WM_LBUTTONDOWN:
      break;
   case WM_MOUSEMOVE:
      //do_mm(LOWORD(lParam), HIWORD(lParam));
      break;
   case WM_SIZE:
      do_size(LOWORD(lParam), HIWORD(lParam));
      ::InvalidateRect(hWnd, NULL, 1);
      break;
   case WM_ERASEBKGND:
      break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
