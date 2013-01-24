/*
 * picex.cc
 */

#include "stdafx.h"
#include "picex.h"
#include "button.h"
#include "resource.h"
#include "utf8conv.h"
#include "ShowStyle.h"
#include <process.h>
#include <Windowsx.h>

PicEx::PicEx() : opened_(false) {
}

PicEx::~PicEx() {
}

static void do_size(HWND hw, int w, int h);

struct tool_bar {
   tool_bar() : 
      cw(false), ccw(false), fit(true), 
      ori(false), zoom(1), slider_show(false), 
      showing(false), bigger(false), dragging(false), 
      sizing(false), zooming(false), hbr(0) {
         new_pt.x = 0;
         new_pt.y = 0;
   }

   int w, h;
   Button btns[7];
   HWND zoom_bar;
   HWND tb;
   WNDPROC stc_proc;
   bool cw, ccw;
   bool fit, ori;
   bool slider_show, showing;
   bool bigger, dragging;
   bool sizing;
   int zoom;
   bool zooming;
   POINT last_pt, new_pt;
   HBRUSH hbr;
   WINDOWPLACEMENT wp;
   CRITICAL_SECTION cs;
   RECT rc;
} g_tb;

int btn_click0(Button* btn) {
   HWND pw = ::GetParent(g_tb.tb);
   PicEx* pe = (PicEx*)::GetWindowLongPtr(pw, GWLP_USERDATA);
   if (!pe) {
      return -1;
   }
   g_tb.fit = g_tb.ori = false;
   //Gdiplus::Rect rc = btn->rect();
   ::ShowWindow(g_tb.zoom_bar, SW_SHOW);
   ::SendMessage(g_tb.zoom_bar, TBM_SETPOS, TRUE, 11 - g_tb.zoom);
   ::SetFocus(g_tb.zoom_bar);
   ::SetActiveWindow(g_tb.zoom_bar);
   ::EnableWindow(btn->hw(), 0);

   return 0;
}

int btn_click1(Button* btn) {
   HWND pw = ::GetParent(g_tb.tb);
   PicEx* pe = (PicEx*)::GetWindowLongPtr(pw, GWLP_USERDATA);
   if (!pe) {
      return -1;
   }
   g_tb.fit = !g_tb.fit;
   g_tb.ori = !g_tb.fit;
   ::InvalidateRect(pw, 0, true);
   g_tb.last_pt.x = 0;
   g_tb.last_pt.y = 0;
   g_tb.new_pt.x = 0;
   g_tb.new_pt.y = 0;
   return 0;
}

int btn_click4(Button* btn);
static unsigned int __stdcall slider_show(void* p) {
   HWND pw = (HWND)p;
   while (1) {
      ::EnterCriticalSection(&g_tb.cs);
      if (!g_tb.showing) {
         btn_click4(&g_tb.btns[4]);
         g_tb.showing = true;
      }
      ::LeaveCriticalSection(&g_tb.cs);
      while (!g_tb.slider_show) {
         ::SetWindowPlacement(pw, &g_tb.wp);
         return 0;
      }
      ::Sleep(1000);
   }
   return 0;
}

int btn_click3(Button* btn) {
   HWND pw = ::GetParent(g_tb.tb);
   PicEx* pe = (PicEx*)::GetWindowLongPtr(pw, GWLP_USERDATA);
   if (!pe) {
      return -1;
   }
   g_tb.fit = g_tb.ori = false;
   ::GetWindowPlacement(pw, &g_tb.wp);
   int w = ::GetSystemMetrics(SM_CXSCREEN);
   int h = ::GetSystemMetrics(SM_CYSCREEN);
   RECT rc;
   rc.left = rc.top = 0;;
   rc.right = w;
   rc.bottom = h;
   ::AdjustWindowRectEx(&rc, WS_CAPTION, FALSE, 0);
   g_tb.slider_show = true;
   g_tb.rc = rc;
   ::MoveWindow(pw, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top+64, TRUE);
   g_tb.showing = true;
   unsigned int n;
   ::ShowWindow(g_tb.tb, SW_HIDE);
   ::_beginthreadex(0, 0, slider_show, pw, 0, &n);
   do_size(pw, w, h);
   g_tb.last_pt.x = 0;
   g_tb.last_pt.y = 0;
   g_tb.new_pt.x = 0;
   g_tb.new_pt.y = 0;
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
   }
   ::InvalidateRect(pw, 0, true);
   g_tb.last_pt.x = 0;
   g_tb.last_pt.y = 0;
   g_tb.new_pt.x = 0;
   g_tb.new_pt.y = 0;
   return 0;
}

int btn_click5(Button* btn) {
   HWND pw = ::GetParent(g_tb.tb);
   PicEx* pe = (PicEx*)::GetWindowLongPtr(pw, GWLP_USERDATA);
   if (!pe) {
      return -1;
   }
   g_tb.ccw = true;
   ::InvalidateRect(pw, 0, true);
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
   }
   ::InvalidateRect(pw, 0, true);
   g_tb.last_pt.x = 0;
   g_tb.last_pt.y = 0;
   g_tb.new_pt.x = 0;
   g_tb.new_pt.y = 0;
   return 0;
}

int btn_click6(Button* btn) {
   HWND pw = ::GetParent(g_tb.tb);
   PicEx* pe = (PicEx*)::GetWindowLongPtr(pw, GWLP_USERDATA);
   if (!pe) {
      return -1;
   }
   g_tb.cw = true;
   ::InvalidateRect(pw, 0, true);
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
   int x = rc.left;
   int y = rc.top;
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

   int bottom_h = y;
   int bar_h = 32;
   int bar_w = 340;

   br.SetColor(bk_color);
   g->FillRectangle(&br, 0, 0, w, h);

   g->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
   Gdiplus::GraphicsPath path;
   path.AddArc((w-bar_w)/2, 15, 33, 32, -240, 120);
   path.AddArc((w+bar_w)/2, 15, 33, 32, -60, 120);
   path.CloseFigure();

   Gdiplus::Pen pen(line_color, 1);
   br.SetColor(bar_bk_color);
   g->DrawPath(&pen, &path);
   g->FillPath(&br, &path);

   BitBlt(dc, 
      x, y, w, h, 
      mdc, x, y, SRCCOPY);

   delete g;
   //::SelectObject(mdc, oldmap);
   ::DeleteObject(memmap); 
   ::DeleteDC(mdc);
   //::InvalidateRect(g_tb.zoom_bar, NULL, TRUE);
}

static LRESULT CALLBACK _stc_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
   if (message == WM_PAINT) {
      PAINTSTRUCT ps;
      HDC dc = ::BeginPaint(hWnd, &ps);
      do_stcdraw(hWnd, dc);
      ::EndPaint(hWnd, &ps);
      return 0;
   }
   return ::CallWindowProc(g_tb.stc_proc, hWnd, message, wParam, lParam);
}

static void init(HWND pw, HINSTANCE hInst) {
   ::InitializeCriticalSection(&g_tb.cs);
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

   hw = ::CreateWindowEx(WS_EX_TOPMOST,                               // no extended styles 
      TRACKBAR_CLASS,                  // class name 
      L"Trackbar Control",              // title (caption) 
      WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_VERT,              // style 
      0, 0, 0, 0,                  // size 
      pw, (HMENU)IDC_TRACEBAR,                     // control identifier 
      hInst,                         // instance 
      NULL                             // no WM_CREATE parameter 
      ); 
   if(!hw) {
      return;
   }

   ::SendMessage(hw, TBM_SETRANGE, FALSE, MAKELONG(1,10));
   ::ShowWindow(hw, SW_HIDE);
   g_tb.zoom_bar = hw;
}

static Gdiplus::Rect ori_rc(int w, int h, Gdiplus::Image* img) {
   int w0 = img->GetWidth();
   int h0 = img->GetHeight();

   int x = (w - w0) / 2;
   int y = (h - h0) / 2;

   Gdiplus::Rect rc;
   rc.X = x;
   rc.Y = y;
   rc.Width = w0;
   rc.Height = h0;

   g_tb.bigger = ((w0 > w) || (h0 > h));
   if (1) {
      rc.X += g_tb.new_pt.x;
      rc.Y += g_tb.new_pt.y;
      if (x < 0) {
         if ((rc.X + w0) < w) {
            rc.X = w - w0;
            g_tb.new_pt.x = rc.X - x;
         }
         if (rc.X > 0) {
            rc.X = 0;
            g_tb.new_pt.x = 0 - x;
         }
      }
      if (y < 0) {
         if((rc.Y + h0) < h) {
            rc.Y = h - h0;
            g_tb.new_pt.y = rc.Y - y;
         }
         if (rc.Y > 0) {
            rc.Y = 0;
            g_tb.new_pt.y = rc.Y - y;
         }
      }
   }
      
   return rc;
}

Gdiplus::Rect fit_rc(int w, int h, Gdiplus::Image* img) {
   int w0 = img->GetWidth();
   int h0 = img->GetHeight();

   bool b = (double)w/h > (double)w0/h0;
   int w1 = w, h1 = h;
   if (b) {
      h1 = h0 < h ? h0 : h;
      w1 = w0 * h1 / h0;
   } else {
      w1 = w0 < w ? w0 : w;
      h1 = h0 * w1 / w0;
   }

   int x = (w - w1) / 2;
   int y = (h - h1) / 2;
   if (x < 0) x = 0;
   if (y < 0) y = 0;

   Gdiplus::Rect rc;
   rc.X = x;
   rc.Y = y;
   rc.Width = w1;
   rc.Height = h1;

   return rc;
}

static void re_draw(HWND hWnd) {
   RECT rc;
   ::GetClientRect(hWnd, &rc);
   rc.bottom -= 64;
   ::InvalidateRect(hWnd, &rc, TRUE);
}

static void do_draw(HWND hWnd, HDC dc) {
   RECT rc;
   ::GetClientRect(hWnd, &rc);
   int x = rc.left;
   int y = rc.top;
   int w = rc.right - rc.left;
   int h = rc.bottom - rc.top;
   if (!g_tb.slider_show) {
      h -= 64;
   }

   HBITMAP memmap = ::CreateCompatibleBitmap(dc, w, h);
   HDC mdc = ::CreateCompatibleDC(dc); 
   HBITMAP oldmap = (HBITMAP)::SelectObject(mdc, (HGDIOBJ)memmap);
   Gdiplus::Graphics* g = new Gdiplus::Graphics(mdc);

   Gdiplus::SolidBrush br(Gdiplus::Color::White);
   g->FillRectangle(&br, 0, 0, w, h);

   ::EnterCriticalSection(&g_tb.cs);
   PicEx* pe = (PicEx*)::GetWindowLongPtr(hWnd, GWLP_USERDATA);
   image img = pe->img_;
   if (pe && img.img) {
      if (g_tb.ccw) {
         g_tb.ccw = false;
         img.img->RotateFlip(Gdiplus::Rotate270FlipNone);
         g->DrawImage(pe->img_.img, fit_rc(w, h, img.img));
      } else if (g_tb.cw) {
         g_tb.cw = false;
         img.img->RotateFlip(Gdiplus::Rotate90FlipNone);
         g->DrawImage(pe->img_.img, fit_rc(w, h, img.img));
      } else if (g_tb.fit) {
         g->DrawImage(pe->img_.img, fit_rc(w, h, img.img));
      } else if (g_tb.ori) {
         g->DrawImage(pe->img_.img, ori_rc(w, h, img.img));
      } else if (g_tb.slider_show) {
         if (w > 0 && h-64 > 0) {
            Gdiplus::Rect rc;// = fit_rc(w, h, pe);
            rc.X = g_tb.rc.left;
            rc.Y = g_tb.rc.top;
            rc.Width = g_tb.rc.right - rc.X;
            rc.Height = g_tb.rc.bottom - rc.Y;
            g->DrawImage(pe->img_.img, rc);
            if (g_tb.showing) {
               MagicShow(mdc, rc.X, rc.Y, dc, rc.X, rc.Y, rc.Width, rc.Height, 1, 19, ::rand()%19);
               g_tb.showing = false;
            }
         }
      } else {
         Gdiplus::Rect rc = fit_rc(w, h, img.img);
         int w0 = g_tb.zoom * rc.Width;
         int h0 = g_tb.zoom * rc.Height;
         int x0 = rc.X + (rc.Width - w0) / 2;
         int x1 = x0;
         int y0 = rc.Y + (rc.Height - h0) / 2;
         int y1 = y0;
         if (1) {
            x0 += g_tb.new_pt.x;
            y0 += g_tb.new_pt.y;
            if (x1 < 0) {
               if ((x0 + w0) < w) {
                  x0 = w - w0;
                  g_tb.new_pt.x = x0 - x1;
               }
               if (x0 > 0) {
                  x0 = 0;
                  g_tb.new_pt.x = 0 - x1;
               }
            }
            if (y1 < 0) {
               if((y0 + h0) < h) {
                  y0 = h - h0;
                  g_tb.new_pt.y = y0 - y1;
               }
               if (y0 > 0) {
                  y0 = 0;
                  g_tb.new_pt.y = y0 - y1;
               }
            }
         }
         g->DrawImage(pe->img_.img, x0, y0, w0, h0);
         g_tb.bigger = ((w0 > w) || (h0 > h-64));
      }
   }
   ::LeaveCriticalSection(&g_tb.cs);

   BitBlt(dc, 
      x, y, w, h, 
      mdc, x, y, SRCCOPY);

   delete g;
   //::SelectObject(mdc, oldmap);
   ::DeleteObject(memmap); 
   ::DeleteDC(mdc);
   ::InvalidateRect(g_tb.zoom_bar, NULL, TRUE);
}

static void do_size(HWND hw, int w, int h) {
   int th = h - 64;
   ::MoveWindow(g_tb.tb, 0, th, w, 64, TRUE);

   int btn_top = 16+3;
   int w0 = (w-340)/2+20;
   g_tb.btns[0].Show(w0, btn_top, 37, 26);
   g_tb.btns[1].Show(w0+57, btn_top, 27, 26);
   g_tb.btns[2].Show(w0+57+47, btn_top, 53, 26);
   g_tb.btns[3].Show(w0+57+47+53, btn_top-11, 42, 50);
   g_tb.btns[4].Show(w0+57+47+53+42, btn_top, 53, 26);
   g_tb.btns[5].Show(w0+57+47+53+42+72, btn_top, 25, 26);
   g_tb.btns[6].Show(w0+57+47+53+42+72+45, btn_top, 25, 26);

   ::MoveWindow(g_tb.zoom_bar, w0, h-16-200, 30, 200, FALSE);
   //::UpdateWindow(g_tb.tb);
   //::InvalidateRect(g_tb.tb, NULL, TRUE);
}

static void do_size1(HWND hw, int w, int h) {
   if (!g_tb.slider_show) {
      int btn_top = h-64+16+3;
      int w0 = (w-340)/2+20;
      g_tb.btns[0].Show(w0, btn_top, 37, 26);
      g_tb.btns[1].Show(w0+57, btn_top, 27, 26);
      g_tb.btns[2].Show(w0+57+47, btn_top, 53, 26);
      g_tb.btns[3].Show(w0+57+47+53, btn_top-11, 42, 50);
      g_tb.btns[4].Show(w0+57+47+53+42, btn_top, 53, 26);
      g_tb.btns[5].Show(w0+57+47+53+42+72, btn_top, 25, 26);
      g_tb.btns[6].Show(w0+57+47+53+42+72+45, btn_top, 25, 26);
      //g_tb.btns[7].Show(w0+57+47+52+42+72+45+45, btn_top, 25, 26);

   } else {
      g_tb.btns[0].hide();
      g_tb.btns[1].hide();
      g_tb.btns[2].hide();
      g_tb.btns[3].hide();
      g_tb.btns[4].hide();
      g_tb.btns[5].hide();
      g_tb.btns[6].hide();
   }

   //PicEx* pe = (PicEx*)::GetWindowLongPtr(hw, GWLP_USERDATA);
   //if (pe) {
   //}
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
         g_tb.sizing = true;
         do_size(hWnd, LOWORD(lParam), HIWORD(lParam));
         //::InvalidateRect(hWnd, NULL, TRUE);
      }
      break;
   case WM_DESTROY:
      {
         PicEx* pe = (PicEx*)::GetWindowLongPtr(hWnd, GWLP_USERDATA);
         if (pe) {
            delete pe->img_.img;
            ::GlobalFree(pe->img_.hg);
            pe->img_.img = 0;
            pe->img_.hg = 0;
            pe->opened_ = false;
         }
      }
      break;
   case WM_LBUTTONDOWN:
      {
         ::ShowWindow(g_tb.zoom_bar, SW_HIDE);
         ::EnableWindow(g_tb.btns[0].hw(), 1);
         if (g_tb.bigger) {
            //::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_PNG_DRAGHAND)));
            g_tb.last_pt.x = GET_X_LPARAM(lParam);
            g_tb.last_pt.y = GET_Y_LPARAM(lParam);
         } else {
            ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
         }
      }
      break;
   case WM_MOUSEMOVE:
      {
         if (g_tb.bigger) {
            //::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_PNG_HAND)));
            if (MK_LBUTTON == wParam) {
               g_tb.dragging = true;
               int x = GET_X_LPARAM(lParam);
               int y = GET_Y_LPARAM(lParam);
               g_tb.new_pt.x += x - g_tb.last_pt.x; 
               g_tb.new_pt.y += y - g_tb.last_pt.y;
               g_tb.last_pt.x = x;
               g_tb.last_pt.y = y;
               re_draw(hWnd);
            }
         } else {
            ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
         }
      }
      break;
   case WM_LBUTTONUP:
      {
         g_tb.dragging = false;
      }
      break;
   case WM_ERASEBKGND:
      break;
   case WM_KEYDOWN:
      {
         if (wParam == VK_ESCAPE) {
            g_tb.slider_show = false;
            g_tb.fit = true;
            g_tb.ori = false;
            ::ShowWindow(g_tb.tb, SW_SHOW);
            ::InvalidateRect(g_tb.tb, 0, TRUE);
         }
      }
      break;
   case WM_VSCROLL:
      {
         if ((HWND)lParam != g_tb.zoom_bar) {
            break;
         }
         int newPos = SendMessage(g_tb.zoom_bar, TBM_GETPOS, 0, 0);
         g_tb.zoom = 11 - newPos;
         ::InvalidateRect(hWnd, 0, true);
         if (LOWORD(wParam) > 4) {
            break;
         }
         ::ShowWindow(g_tb.zoom_bar, SW_HIDE);
         ::EnableWindow(g_tb.btns[0].hw(), 1);
         g_tb.zooming = true;
      }
      break;
   case WM_GETMINMAXINFO:
      {
         LPMINMAXINFO mm = (LPMINMAXINFO)lParam;
         mm->ptMinTrackSize.x = 500;
         mm->ptMinTrackSize.y = 500;
      }
      break;
   //case WM_CTLCOLORSTATIC:
   //   {
   //      if (g_tb.tb == (HWND)lParam) {
   //         HDC dc = (HDC)wParam;
   //         ::SetBkColor(dc, RGB(192, 192, 192));
   //         if (g_tb.hbr == 0) {
   //            g_tb.hbr = ::CreateSolidBrush(RGB(192, 192, 192));
   //         }
   //         return (INT_PTR)g_tb.hbr;
   //      }
   //   }
   //   break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

const LPTSTR pic_class_name = TEXT("picex");

ATOM reg_class(HINSTANCE hInstance, int idc) {
   WNDCLASSEX wcex;

   wcex.cbSize = sizeof(WNDCLASSEX);

   wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
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
      ::GlobalFree(this->img_.hg);
      this->img_.hg = 0;
      return 0;
   }

   reg_class(hinst, 0);
   HWND hw = ::CreateWindowEx(0, pic_class_name, L"Í¼Æ¬ä¯ÀÀÆ÷", 
      WS_OVERLAPPEDWINDOW | WS_VISIBLE,
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
   ::InvalidateRect(this->hw_, NULL, TRUE);
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