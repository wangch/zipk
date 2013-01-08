/*
 * picex.cc
 */

#include "stdafx.h"
#include "picex.h"
#include "button.h"
#include "resource.h"

PicEx::PicEx() {
}

PicEx::~PicEx() {
}

struct tool_bar {
   int w, h;
   Button btns[7];
} g_tb;

int btn_click0(Button* btn) {
   return 0;
}

int btn_click1(Button* btn) {
   return 0;
}

int btn_click2(Button* btn) {
   return 0;
}

int btn_click3(Button* btn) {
   return 0;
}

int btn_click4(Button* btn) {
   return 0;
}

int btn_click5(Button* btn) {
   return 0;
}

int btn_click6(Button* btn) {
   return 0;
}

int btn_click7(Button* btn) {
   return 0;
}

static void set_imgs() {
   g_tb.btns[0].SetImg(L"res\\zoom.png", 0, 0, 0, 37, 25); 
   g_tb.btns[0].SetImg(L"res\\zoom1.png", 1, 0, 0, 37, 25);
   g_tb.btns[0].SetImg(L"res\\zoom2.png", 2, 0, 0, 37, 25);

   g_tb.btns[1].SetImg(L"res\\fit.png", 0, 0, 0, 27, 25); 
   g_tb.btns[1].SetImg(L"res\\fit1.png", 1, 0, 0, 27, 25);
   g_tb.btns[1].SetImg(L"res\\fit2.png", 2, 0, 0, 27, 25);

   g_tb.btns[2].SetImg(L"res\\back.png", 0, 0, 0, 52, 25); 
   g_tb.btns[2].SetImg(L"res\\back1.png", 1, 0, 0, 52, 25);
   g_tb.btns[2].SetImg(L"res\\back2.png", 2, 0, 0, 52, 25);

   g_tb.btns[3].SetImg(L"res\\full.png", 0, 0, 0, 42, 48); 
   g_tb.btns[3].SetImg(L"res\\full1.png", 1, 0, 0, 42, 48);
   g_tb.btns[3].SetImg(L"res\\full2.png", 2, 0, 0, 42, 48);

   g_tb.btns[4].SetImg(L"res\\next.png", 0, 0, 0, 52, 25); 
   g_tb.btns[4].SetImg(L"res\\next1.png", 1, 0, 0, 52, 25);
   g_tb.btns[4].SetImg(L"res\\next2.png", 2, 0, 0, 52, 25);

   g_tb.btns[5].SetImg(L"res\\counterclockwise.png", 0, 0, 0, 25, 25); 
   g_tb.btns[5].SetImg(L"res\\counterclockwise1.png", 1, 0, 0, 25, 25);
   g_tb.btns[5].SetImg(L"res\\counterclockwise2.png", 2, 0, 0, 25, 25);

   g_tb.btns[6].SetImg(L"res\\clockwise.png", 0, 0, 0, 25, 25); 
   g_tb.btns[6].SetImg(L"res\\clockwise1.png", 1, 0, 0, 25, 25);
   g_tb.btns[6].SetImg(L"res\\clockwise2.png", 2, 0, 0, 25, 25);

   //g_tb.btns[7].SetImg(L"res\\del.png", 0, 0, 0, 25, 25); 
   //g_tb.btns[7].SetImg(L"res\\del1.png", 1, 0, 0, 25, 25);
   //g_tb.btns[7].SetImg(L"res\\del2.png", 2, 0, 0, 25, 25);
}

static void set_imgs1(HINSTANCE hInst) {
   g_tb.btns[0].SetImg(hInst, IDB_PNG_ZOOM, 0, 0, 0, 37, 25); 
   g_tb.btns[0].SetImg(hInst, IDB_PNG_ZOOM1, 1, 0, 0, 37, 25);
   g_tb.btns[0].SetImg(hInst, IDB_PNG_ZOOM1, 2, 0, 0, 37, 25);
                       
   g_tb.btns[1].SetImg(hInst, IDB_PNG_FIT, 0, 0, 0, 27, 25); 
   g_tb.btns[1].SetImg(hInst, IDB_PNG_FIT1, 1, 0, 0, 27, 25);
   g_tb.btns[1].SetImg(hInst, IDB_PNG_FIT2, 2, 0, 0, 27, 25);
                     
   g_tb.btns[2].SetImg(hInst, IDB_PNG_BACK, 0, 0, 0, 52, 25); 
   g_tb.btns[2].SetImg(hInst, IDB_PNG_BACK1, 1, 0, 0, 52, 25);
   g_tb.btns[2].SetImg(hInst, IDB_PNG_BACK2, 2, 0, 0, 52, 25);
                      
   g_tb.btns[3].SetImg(hInst, IDB_PNG_FULL, 0, 0, 0, 42, 48); 
   g_tb.btns[3].SetImg(hInst, IDB_PNG_FULL1, 1, 0, 0, 42, 48);
   g_tb.btns[3].SetImg(hInst, IDB_PNG_FULL2, 2, 0, 0, 42, 48);
                      
   g_tb.btns[4].SetImg(hInst, IDB_PNG_NEXT, 0, 0, 0, 52, 25); 
   g_tb.btns[4].SetImg(hInst, IDB_PNG_NEXT1, 1, 0, 0, 52, 25);
   g_tb.btns[4].SetImg(hInst, IDB_PNG_NEXT2, 2, 0, 0, 52, 25);
                        
   g_tb.btns[5].SetImg(hInst, IDB_PNG_CCW, 0, 0, 0, 25, 25); 
   g_tb.btns[5].SetImg(hInst, IDB_PNG_CCW1, 1, 0, 0, 25, 25);
   g_tb.btns[5].SetImg(hInst, IDB_PNG_CCW2, 2, 0, 0, 25, 25);
                       
   g_tb.btns[6].SetImg(hInst, IDB_PNG_CW, 0, 0, 0, 25, 25); 
   g_tb.btns[6].SetImg(hInst, IDB_PNG_CW1, 1, 0, 0, 25, 25);
   g_tb.btns[6].SetImg(hInst, IDB_PNG_CW2, 2, 0, 0, 25, 25);
                      
   //g_tb.btns[7].SetImg(hInst, IDB_PNG_DEL, 0, 0, 0, 25, 25); 
   //g_tb.btns[7].SetImg(hInst, IDB_PNG_DEL1, 1, 0, 0, 25, 25);
   //g_tb.btns[7].SetImg(hInst, IDB_PNG_DEL2, 2, 0, 0, 25, 25);
}

static void init(HWND pw, HINSTANCE hInst) {
   g_tb.btns[0].Init(0, pw, hInst, btn_click0);
   g_tb.btns[1].Init(1, pw, hInst, btn_click1);
   g_tb.btns[2].Init(2, pw, hInst, btn_click2);
   g_tb.btns[3].Init(3, pw, hInst, btn_click3);
   g_tb.btns[4].Init(4, pw, hInst, btn_click4);
   g_tb.btns[5].Init(5, pw, hInst, btn_click5);
   g_tb.btns[6].Init(6, pw, hInst, btn_click6);
   //g_tb.btns[7].Init(7, pw, hInst, btn_click7);

   Gdiplus::Color bk_color(193, 214, 235);
   g_tb.btns[0].SetBkColor(bk_color);
   g_tb.btns[1].SetBkColor(bk_color);
   g_tb.btns[2].SetBkColor(bk_color);
   g_tb.btns[3].SetBkColor(bk_color);
   g_tb.btns[4].SetBkColor(bk_color);
   g_tb.btns[5].SetBkColor(bk_color);
   g_tb.btns[6].SetBkColor(bk_color);
   //g_tb.btns[7].SetBkColor(bk_color);
   set_imgs1(hInst);
}

static void do_draw(HWND hWnd, HDC dc) {
   RECT rc;
   ::GetClientRect(hWnd, &rc);
   int x = rc.left;
   int y = rc.top;
   int w = rc.right - rc.left - 1;
   int h = rc.bottom - rc.top - 1;

   HBITMAP memmap = ::CreateCompatibleBitmap(dc, w, h);
   HDC mdc = ::CreateCompatibleDC(dc); 
   HBITMAP oldmap = (HBITMAP)::SelectObject(mdc, (HGDIOBJ)memmap);
   Gdiplus::Graphics* g = new Gdiplus::Graphics(mdc);

   Gdiplus::Color bk_color(185, 209, 234);
   Gdiplus::Color bar_bk_color(193, 214, 235);
   Gdiplus::Color line_color(133, 144, 160);

   int bottom_h = 64;
   int bar_h = 32;
   int bar_w = 340;

   Gdiplus::SolidBrush br(Gdiplus::Color::White);
   g->FillRectangle(&br, 0, 0, w, h);
   br.SetColor(bk_color);
   g->FillRectangle(&br, 0, h - bottom_h, w, bottom_h);

   g->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
   Gdiplus::GraphicsPath path;
   path.AddArc((w-bar_w)/2, (h-bottom_h+bar_h/2), 33, 32, -240, 120);
   path.AddArc((w+bar_w)/2, (h-bottom_h+bar_h/2), 33, 32, -60, 120);
   path.CloseFigure();

   Gdiplus::Pen pen(line_color, 1);
   br.SetColor(bar_bk_color);
   g->DrawPath(&pen, &path);
   g->FillPath(&br, &path);

   BitBlt(dc, 
      x, y, w, h, 
      mdc, x, y, SRCCOPY);

   delete g;
   ::SelectObject(mdc, oldmap);
   ::DeleteObject(memmap); 
   ::DeleteDC(mdc);
}

static void do_size(int w, int h) {
   int btn_top = h-64+16+3;
   int w0 = (w-340)/2+20;
   g_tb.btns[0].Show(w0, btn_top, 37, 26);
   g_tb.btns[1].Show(w0+57, btn_top, 27, 26);
   g_tb.btns[2].Show(w0+57+47, btn_top, 52, 26);
   g_tb.btns[3].Show(w0+57+47+52, btn_top-11, 42, 50);
   g_tb.btns[4].Show(w0+57+47+52+42, btn_top, 52, 26);
   g_tb.btns[5].Show(w0+57+47+52+42+72, btn_top, 25, 26);
   g_tb.btns[6].Show(w0+57+47+52+42+72+45, btn_top, 25, 26);
   //g_tb.btns[7].Show(w0+57+47+52+42+72+45+45, btn_top, 25, 26);
}


static LRESULT CALLBACK _WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
   switch(message) {
   case WM_PAINT:
      {
         PAINTSTRUCT ps;
         HDC dc = ::BeginPaint(hWnd, &ps);
         do_draw(hWnd, dc);
         ::EndPaint(hWnd, &ps);
      }
      break;
   case WM_SIZE: 
      {
         do_size(LOWORD(lParam), HIWORD(lParam));
         ::InvalidateRect(hWnd, NULL, TRUE);
      }
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

const LPTSTR pic_class_name = TEXT("picex");

ATOM reg_class(HINSTANCE hInstance, int idc) {
   WNDCLASSEX wcex;

   wcex.cbSize = sizeof(WNDCLASSEX);

   wcex.style			= CS_HREDRAW | CS_VREDRAW;
   wcex.lpfnWndProc	= _WndProc;
   wcex.cbClsExtra		= 0;
   wcex.cbWndExtra		= 0;
   wcex.hInstance		= hInstance;
   wcex.hIcon			= 0;//LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ZIPK));
   wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
   wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
   wcex.lpszMenuName	= MAKEINTRESOURCE(idc);
   wcex.lpszClassName	= pic_class_name;
   wcex.hIconSm		= 0;//LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

   ATOM r = RegisterClassEx(&wcex);
   if (!r) {
      int e = ::GetLastError();
      int n = 0;
   }
   return r;
}

int PicEx::Init(HWND pw, HINSTANCE hinst, int idc) {
   reg_class(hinst, 0);
   HWND hw = ::CreateWindowEx(0, pic_class_name, L"Í¼Æ¬ä¯ÀÀÆ÷", 
      WS_OVERLAPPEDWINDOW | WS_VISIBLE,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, 0, 0, hinst, NULL);
   if (!hw) {
      int e = ::GetLastError();
      return -1;
   }

   this->hw_ = hw;

   init(hw, hinst);
   ShowWindow(hw, SW_SHOW);
   UpdateWindow(hw);

   return 0;
}
