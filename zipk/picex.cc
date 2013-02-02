/*
 * picex.cc
 */

#include "stdafx.h"
#include "picex.h"
#include "button.h"
#include "resource.h"
#include "utf8conv.h"
#include "ShowStyle.h"
#include "image.h"
#include <process.h>
#include <Windowsx.h>

PicEx::PicEx() : opened_(false) {
}

PicEx::~PicEx() {
}

static void do_size(HWND hw, int w, int h);

struct tool_bar {
   tool_bar() : 
      slider_show(false), zoom(1), status(FIT) {
   }

   Button btns[7];
   HWND zoom_bar0;
   HWND zoom_bar;
   HWND tb;
   ImageCtrl img;
   WNDPROC zoom_proc;
   WNDPROC stc_proc;
   bool slider_show;
   int zoom;
   WINDOWPLACEMENT wp;
   //HANDLE m;
   int status;
} g_tb;


int btn_click0(Button* btn) {
   HWND pw = ::GetParent(g_tb.tb);
   PicEx* pe = (PicEx*)::GetWindowLongPtr(pw, GWLP_USERDATA);
   if (!pe) {
      return -1;
   }
   ::ShowWindow(btn->hw(), SW_HIDE);
   ::EnableWindow(g_tb.img.hw(), FALSE);
   ::ShowWindow(g_tb.zoom_bar0, SW_SHOW);
   ::SendMessage(g_tb.zoom_bar0, TBM_SETPOS, TRUE, 11 - g_tb.zoom);
   ::SetFocus(g_tb.zoom_bar0);
   ::SetCapture(g_tb.zoom_bar0);

   return 0;
}

int btn_click1(Button* btn) {
   HWND pw = ::GetParent(g_tb.tb);
   PicEx* pe = (PicEx*)::GetWindowLongPtr(pw, GWLP_USERDATA);
   if (!pe) {
      return -1;
   }
   if (g_tb.status != FIT) {
      g_tb.status = FIT;
   } else {
      g_tb.status = ORI;
   }

   g_tb.img.Show(g_tb.status);
   return 0;
}

int btn_click2(Button* btn);
int btn_click4(Button* btn);

static unsigned int __stdcall slider_show(void* p) {
   HWND pw = (HWND)p;
   while (1) {
      if (!g_tb.slider_show) {
         g_tb.status = FIT;
         g_tb.img.Show(g_tb.status);
         break;
      }
      g_tb.img.Show(g_tb.status);
      btn_click4(&g_tb.btns[4]);
  }
   return 0;
}

int btn_click3(Button* btn) {
   HWND pw = ::GetParent(g_tb.tb);
   PicEx* pe = (PicEx*)::GetWindowLongPtr(pw, GWLP_USERDATA);
   if (!pe) {
      return -1;
   }
   ::GetWindowPlacement(pw, &g_tb.wp);
   int w = ::GetSystemMetrics(SM_CXSCREEN);
   int h = ::GetSystemMetrics(SM_CYSCREEN);
   RECT rc;
   rc.left = rc.top = 0;
   rc.right = w;
   rc.bottom = h;
   ::AdjustWindowRectEx(&rc, WS_CAPTION, FALSE, 0);
   ::MoveWindow(pw, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE);
   ::MoveWindow(g_tb.img.hw(), 0, 0, w, h, TRUE);

   g_tb.slider_show = true;

   unsigned int n;
   ::ShowWindow(g_tb.tb, SW_HIDE);
   ::_beginthreadex(0, 0, slider_show, pw, 0, &n);
   g_tb.status = SLIDER;
   return 0;
}

int btn_click4(Button* btn) {
   HWND pw = ::GetParent(g_tb.tb);
   PicEx* pe = (PicEx*)::GetWindowLongPtr(pw, GWLP_USERDATA);
   if (!pe) {
      return -1;
   }
   node* p = pe->n_->p;
   if (p->childs.size() == 1) {
      return 0;
   }
   node* n = 0;
   for (int i = pe->n_->i+1; i < (int)p->childs.size(); ++i) {
      if (p->childs[i]->type == 1) {
         n = p->childs[i];
         break;
      }
   }
   if (!n) {
      for (int i = 0; i < pe->n_->i; ++i) {
         if (p->childs[i]->type == 1) {
            n = p->childs[i];
            break;
         }
      }
   }
   if (!n) {
      return 0;
   }
   delete pe->img_.img;
   ::GlobalFree(pe->img_.hg);
   image img = unzip_img(n, pe->uf_);
   if (img.img) {
      pe->img_ = img;
      pe->n_ = n;
      g_tb.img.SetImg(img.img);
   }
   return 0;
}

int btn_click5(Button* btn) {
   g_tb.img.RotateFlip270();
   return 0;
}

int btn_click2(Button* btn) {
   HWND pw = ::GetParent(g_tb.tb);
   PicEx* pe = (PicEx*)::GetWindowLongPtr(pw, GWLP_USERDATA);
   if (!pe) {
      return -1;
   }
   node* p = pe->n_->p;
   if (p->childs.size() == 1) {
      return 0;
   }
   node* n = 0;
   for (int i = pe->n_->i-1; i >= 0; --i) {
      if (p->childs[i]->type == 1) {
         n = p->childs[i];
         break;
      }
   }
   if (!n) {
      for (int i = p->childs.size() - 1; i > pe->n_->i; --i) {
         if (p->childs[i]->type == 1) {
            n = p->childs[i];
            break;
         }
      }
   }
   if (!n) {
      return 0;
   }
   delete pe->img_.img;
   ::GlobalFree(pe->img_.hg);
   image img = unzip_img(n, pe->uf_);
   if (img.img) {
      pe->img_ = img;
      pe->n_ = n;
      g_tb.img.SetImg(img.img);
   }
   return 0;
}

int btn_click6(Button* btn) {
   g_tb.img.RotateFlip90();
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

static void do_stcdraw(HWND hWnd, HDC dc) {
   RECT rc;
   ::GetClientRect(hWnd, &rc);
   int w = rc.right - rc.left;
   int h = rc.bottom - rc.top;

   HBITMAP memmap = ::CreateCompatibleBitmap(dc, w, h);
   HDC mdc = ::CreateCompatibleDC(dc); 
   HBITMAP oldmap = (HBITMAP)::SelectObject(mdc, (HGDIOBJ)memmap);
   Gdiplus::Graphics* g = new Gdiplus::Graphics(mdc);
   Gdiplus::SolidBrush br(Gdiplus::Color::White);

   Gdiplus::Color bk_color(185, 209, 234);
   Gdiplus::Color bar_bk_color(193, 214, 235);
   Gdiplus::Color line_color(133, 144, 160);

   int bottom_h = 0;
   int bar_h = 32;
   int bar_w = 340;

   br.SetColor(bk_color);
   g->FillRectangle(&br, 0, 0, w, h);

   g->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
   Gdiplus::GraphicsPath path;
   path.AddArc((w-bar_w)/2-16, 15, 33, 32, -240, 120);
   path.AddArc((w+bar_w)/2-16, 15, 33, 32, -60, 120);
   path.CloseFigure();

   Gdiplus::Pen pen(line_color, 1);
   br.SetColor(bar_bk_color);
   g->DrawPath(&pen, &path);
   g->FillPath(&br, &path);

   BitBlt(dc, 
      0, 0, w, h, 
      mdc, 0, 0, SRCCOPY);

   delete g;
   ::SelectObject(mdc, oldmap);
   ::DeleteObject(memmap); 
   ::DeleteDC(mdc);
}

static LRESULT CALLBACK _zoom_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
   if (message == WM_MOUSEMOVE) {
      if (MK_LBUTTON == wParam) {
         int y = GET_Y_LPARAM(lParam);
         int pos = SendMessage(g_tb.zoom_bar0, TBM_GETPOS, 0, 0);
         int newpos = y / 16;
         ::SendMessage(hWnd, TBM_SETPOS, TRUE, newpos);
         if (pos != newpos) {
            g_tb.zoom = 11 - newpos;
            if (g_tb.zoom < 1 || g_tb.zoom > 11) {
               return 0;
            }
            g_tb.status = ZOOM;
            g_tb.img.Show(g_tb.status, g_tb.zoom);
            ::InvalidateRect(g_tb.zoom_bar0, 0, FALSE);
            return 0;
         }
      }
   }
   return ::CallWindowProc(g_tb.zoom_proc, hWnd, message, wParam, lParam);
}

static LRESULT CALLBACK _stc_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
   if (message == WM_PAINT) {
      PAINTSTRUCT ps;
      HDC dc = ::BeginPaint(hWnd, &ps);
      do_stcdraw(hWnd, dc);
      ::EndPaint(hWnd, &ps);
      return 0;
   }
   if (message == WM_ERASEBKGND) {
      return 0;
   }
   return ::CallWindowProc(g_tb.stc_proc, hWnd, message, wParam, lParam);
}

static void init(HWND pw, HINSTANCE hInst) {
   g_tb.img.Init(pw, hInst);
   HWND hw = ::CreateWindowEx(0, WC_STATIC, L"",
      WS_CHILD | WS_VISIBLE, 
      0, 0, 0, 0,
      pw, (HMENU)IDC_STATIC, hInst, NULL);
   if (!hw) {
      return;
   }

   g_tb.tb = hw;
   g_tb.stc_proc = (WNDPROC)::SetWindowLongPtr(hw, GWLP_WNDPROC, (LONG)_stc_proc);

   g_tb.btns[0].Init(0, hw, hInst, 0, btn_click0, 0);
   g_tb.btns[1].Init(1, hw, hInst, 0, 0, btn_click1);
   g_tb.btns[2].Init(2, hw, hInst, 0, 0, btn_click2);
   g_tb.btns[3].Init(3, hw, hInst, 0, 0, btn_click3);
   g_tb.btns[4].Init(4, hw, hInst, 0, 0, btn_click4);
   g_tb.btns[5].Init(5, hw, hInst, 0, 0, btn_click5);
   g_tb.btns[6].Init(6, hw, hInst, 0, 0, btn_click6);
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

   hw = ::CreateWindowEx(0, TRACKBAR_CLASS, L"", 
      WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_VERT | TBS_NOTIFYBEFOREMOVE,
      0, 0, 0, 0,
      pw, (HMENU)IDC_TRACEBAR, hInst, NULL); 
   if(!hw) {
      return;
   }
   g_tb.zoom_proc = (WNDPROC)::SetWindowLongPtr(hw, GWLP_WNDPROC, (LONG)_zoom_proc);

   ::SendMessage(hw, TBM_SETRANGE, FALSE, MAKELONG(1,10));
   ::ShowWindow(hw, SW_HIDE);
   g_tb.zoom_bar0 = hw;
}


static void do_size(HWND hw, int w, int h) {
   int th = h - 64;
   ::MoveWindow(g_tb.img.hw(), 0, 0, w, th, TRUE);
   g_tb.img.Show(g_tb.status, g_tb.zoom);
   ::MoveWindow(g_tb.tb, 0, th, w, 64, TRUE);
   ::InvalidateRect(g_tb.tb, NULL, FALSE);

   int btn_top = 16+3;
   int w0 = (w-340)/2;
   g_tb.btns[0].Show(w0, btn_top, 37, 26);
   g_tb.btns[1].Show(w0+57, btn_top, 27, 26);
   g_tb.btns[2].Show(w0+57+47, btn_top, 53, 26);
   g_tb.btns[3].Show(w0+57+47+53, btn_top-11, 42, 50);
   g_tb.btns[4].Show(w0+57+47+53+42, btn_top, 53, 26);
   g_tb.btns[5].Show(w0+57+47+53+42+72, btn_top, 25, 26);
   g_tb.btns[6].Show(w0+57+47+53+42+72+45, btn_top, 25, 26);

   ::MoveWindow(g_tb.zoom_bar0, w0, h-16-160, 30, 160, FALSE);
   ::MoveWindow(g_tb.zoom_bar, 0, 0, 30, 160, FALSE);
}

static LRESULT CALLBACK _WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
   switch(message) {
   case WM_PAINT:
      {
         PAINTSTRUCT ps;
         HDC dc = ::BeginPaint(hWnd, &ps);
         ::EndPaint(hWnd, &ps);
      }
      break;
   case WM_SIZE: 
      {
         do_size(hWnd, LOWORD(lParam), HIWORD(lParam));
      }
      break;
   case WM_DESTROY:
      {
         PicEx* pe = (PicEx*)::GetWindowLongPtr(hWnd, GWLP_USERDATA);
         if (pe) {
            if (g_tb.slider_show) {
               g_tb.slider_show = false;
               ::Sleep(2*1000);
            }
            delete pe->img_.img;
            ::GlobalFree(pe->img_.hg);
            pe->img_.img = 0;
            pe->img_.hg = 0;
            pe->opened_ = false;
         }
      }
      break;
   case WM_ERASEBKGND:
      break;
   case WM_KEYDOWN:
      {
         if (wParam == VK_ESCAPE) {
            g_tb.slider_show = false;
            ::ShowWindow(g_tb.tb, SW_SHOW);
            ::SetWindowPlacement(hWnd, &g_tb.wp);
            ::InvalidateRect(g_tb.tb, 0, FALSE);
         }
      }
      break;
   case WM_NOTIFY:
      {
         switch (((LPNMHDR)lParam)->code) {
         case NM_RELEASEDCAPTURE:
            {
               ::ShowWindow(g_tb.btns[0].hw(), SW_SHOW);
               ::ShowWindow(g_tb.zoom_bar0, SW_HIDE);
               ::EnableWindow(g_tb.img.hw(), TRUE);
               ::InvalidateRect(g_tb.tb, 0, FALSE);
               ::ReleaseCapture(); 
            }
            break;
         }
      }
      break;
   case WM_GETMINMAXINFO:
      {
         LPMINMAXINFO mm = (LPMINMAXINFO)lParam;
         mm->ptMinTrackSize.x = 500;
         mm->ptMinTrackSize.y = 500;
      }
      break;
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

int PicEx::Init(HWND pw, HINSTANCE hinst, unzFile uf) {
   if (this->opened_) {
      delete this->img_.img;
      this->img_.img = 0;
      g_tb.img.SetImg(0);
      ::GlobalFree(this->img_.hg);
      this->img_.hg = 0;
      return 0;
   }

   reg_class(hinst, 0);
   HWND hw = ::CreateWindowEx(0, pic_class_name, L"Í¼Æ¬ä¯ÀÀÆ÷", 
      WS_OVERLAPPEDWINDOW,
      0, 0, 0, 0, 0, 0, hinst, NULL);
   if (!hw) {
      int e = ::GetLastError();
      return -1;
   }
   
   this->hw_ = hw;
   this->uf_ = uf;

   init(hw, hinst);
    ::SetWindowLongPtr(this->hw_, GWLP_USERDATA, (LONG)this);
   this->opened_ = true;

   return 0;
}

void PicEx::show(int x, int y, int w, int h) {
   ::MoveWindow(this->hw_, x, y, w, h, TRUE);
   ::ShowWindow(this->hw_, SW_SHOW);
   ::UpdateWindow(this->hw_);
}

void PicEx::hide() {
   ::ShowWindow(this->hw_, SW_HIDE);
}

void PicEx::set_img(node* n, image img) {
   this->img_ = img;
   this->n_ = n;
   g_tb.img.SetImg(img.img);
   g_tb.status = FIT;
   RECT rc;
   ::GetClientRect(this->hw_, &rc);
   g_tb.img.Show(g_tb.status);
}


image unzip_img(node* n, unzFile uf) {
   image img;
   unzLocateFile(uf, utf8util::UTF8FromUTF16(n->path).c_str(), 2);
   unzOpenCurrentFile(uf);
   HGLOBAL mem = ::GlobalAlloc(GMEM_MOVEABLE, n->size);
   if (!mem) {
      return img;
   }
   Gdiplus::Image* m = NULL;
   LPVOID pmem = ::GlobalLock(mem);
   if (pmem) {
      __int64 rd = 0;
      const int BL = 8192;
      char buf[BL];
      int n = 0;
      do {
         n = unzReadCurrentFile(uf, buf, BL);
         char* p = (char*)pmem;
         ::memcpy(p+rd, buf, n);
         rd += n;
      } while (n);
      unzCloseCurrentFile(uf);
      LPSTREAM is;
      if (::CreateStreamOnHGlobal(pmem, FALSE, &is) == S_OK) {
         m = new Gdiplus::Image(is);
         is->Release();
      }
      ::GlobalUnlock(mem);
   }
   img.hg = mem;
   img.img = m;
   return img;
}