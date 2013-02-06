/*
 * button.cc
 */

#include "stdafx.h"
#include "button.h"
#include "resource.h"


/// Button
Button::Button() : statuse_(NORMAL), hand_(false) {
}

Button::~Button() {
}

static LRESULT CALLBACK _btnProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
   Button* btn = (Button*)::GetWindowLongPtr(hWnd, GWLP_USERDATA);
   if (btn == NULL) {
      return ::DefWindowProc(hWnd, message, wParam, lParam);
   }

   return btn->WndProc(message, wParam, lParam);
}

void Button::draw(int w, int h) {
   if (w == 0) {
      RECT rc; 
      ::GetClientRect(this->hw_, &rc);
      int x = rc.left;
      int y = rc.top;
      w = rc.right - x;
      h = rc.bottom - y;
   }
   HDC dc = ::GetDC(this->hw_);
   HBITMAP memmap = ::CreateCompatibleBitmap(dc, w, h);
   HDC mdc = ::CreateCompatibleDC(dc); 
   HBITMAP oldmap = (HBITMAP)::SelectObject(mdc, (HGDIOBJ)memmap);
   Gdiplus::Graphics* g = new Gdiplus::Graphics(mdc);

   Gdiplus::SolidBrush br(this->bk_color_);
   g->FillRectangle(&br, 0, 0, w, h);

   if (!this->imgs_.empty()) {
      img_info img = this->imgs_[this->statuse_];
      Gdiplus::Rect rc(img.x, img.y, img.w, img.h);
      if (img.img == 0) {
         wchar_t filep[MAX_PATH];
         ::GetModuleFileName(NULL, filep, _MAX_PATH);
         std::wstring p(filep);
         int pos = p.find_last_of(L'\\');
         if (pos != std::wstring::npos) {
            p = p.substr(0, pos+1);
         }
         Gdiplus::Image m((p+img.fname).c_str());        
         g->DrawImage(&m, rc);
      } else {
         g->DrawImage(img.img, rc);
         //delete img.img;
      }
   }

   if (this->txt_.txt.length() > 0) {
      Gdiplus::PointF pt((Gdiplus::REAL)this->txt_.x, (Gdiplus::REAL)this->txt_.y);
      Gdiplus::SolidBrush sbr(Gdiplus::Color::White);
      g->DrawString(this->txt_.txt.c_str(), this->txt_.txt.length(), this->txt_.font, pt, &sbr);
   }


   delete g;
   ::DeleteObject(memmap);
   ::DeleteDC(this->mdc_);
   this->mdc_ = mdc;
   ::InvalidateRect(this->hw_, NULL, TRUE);
}

void Button::re_draw(RECT rc, HDC dc) {
   int x = rc.left;
   int y = rc.top;
   int w = rc.right - x;
   int h = rc.bottom - y;
   BitBlt(dc, 
      x, y, w, h, 
      this->mdc_, x, y, SRCCOPY);
}

LRESULT CALLBACK Button::WndProc(UINT message, WPARAM wParam, LPARAM lParam) {
   switch(message) {
   case WM_PAINT:
      {
         PAINTSTRUCT ps;
         HDC dc = ::BeginPaint(this->hw_, &ps);
         this->re_draw(ps.rcPaint, dc);
         ::EndPaint(this->hw_, &ps);
      }
      break;
   case WM_SIZE: 
      {
         this->draw(LOWORD(lParam), HIWORD(lParam));
      }
      break;
   case WM_LBUTTONDOWN:
      this->statuse_ = MLBTNDOWN;
      this->draw(0, 0);
      if (this->fn1_) {
         fn1_(this);
      }
      break;
   case WM_LBUTTONUP:
    this->statuse_ = NORMAL;
      this->draw(0, 0);
      if (this->fn2_) {
         fn2_(this);
      }
      break;
   case WM_MOUSEMOVE:
      {
         ::SetCapture(this->hw_);
         if (this->hand_) {
            ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND))); 
         }
         POINT pt;
         pt.x = LOWORD(lParam);
         pt.y = HIWORD(lParam);
         RECT rc;
         ::GetClientRect(this->hw_, &rc);
         if (::PtInRect(&rc, pt)) {
            this->statuse_ = MMOVING;
         } else {
            this->statuse_ = NORMAL;
            if ((wParam & MK_LBUTTON) == 0) {
               ::ReleaseCapture();
               ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW))); 
            }
         }
         this->draw(0, 0);
      }
      break;
   case WM_MOUSELEAVE:
      this->statuse_ = NORMAL;
      break;
   case WM_ERASEBKGND: 
      break;
   default:
      return ::CallWindowProc(this->btn_proc_, this->hw_, message, wParam, lParam);
   }
   return 0;
}

int Button::Init(int id, HWND pw, HINSTANCE hinst, int idc, btn_click fn1, btn_click fn2) {
   HWND hw = ::CreateWindowEx(0, WC_BUTTON, L"",
      WS_CHILD | WS_VISIBLE | BS_CENTER | BS_FLAT/* | BS_OWNERDRAW*/, 
      0, 0, 0, 0,
      pw, (HMENU)idc, hinst, NULL);
   if (!hw) {
      return -1;
   }

   this->hw_ = hw;
   this->fn1_ = fn1;
   this->fn2_ = fn2;
   this->id_ = id;

   ::SetWindowLongPtr(this->hw_, GWLP_USERDATA, (LONG)this);
   this->btn_proc_ = (WNDPROC)::SetWindowLongPtr(this->hw_, GWLP_WNDPROC, (LONG)_btnProc);

   return 0;
}

void Button::SetImg(const std::wstring& fname, int status, int x, int y, int w, int h) {
   img_info img;
   img.fname = fname;
   img.x = x;
   img.y = y;
   img.w = w;
   img.h = h;

   this->imgs_[status] = img;
}

void Button::SetTxtColor(Gdiplus::Color color) {
   this->txt_.color = color;
}

void Button::SetTxtFont(Gdiplus::Font* f) {
   this->txt_.font = f;
}

void Button::SetText(const std::wstring& txt, int x, int y) {
   this->txt_.txt = txt;
   this->txt_.x = x;
   this->txt_.y = y;
}

void Button::SetBkColor(Gdiplus::Color color) {
   this->bk_color_ = color;
}

void Button::Show(int x, int y, int w, int h) {
   ::MoveWindow(this->hw_, x, y, w, h, TRUE);
   this->rc_.X = x;
   this->rc_.Y = y;
   this->rc_.Width = w;
   this->rc_.Height = h;
   this->draw(w, h);
   ::ShowWindow(this->hw_, SW_SHOW);
}

void Button::Show(int x, int y) {
   this->Show(x, y, 25, 25);
}

void Button::hide() {
   ::ShowWindow(this->hw_, SW_HIDE);
}

void Button::SetRgn(HRGN rgn) {
   ::SetWindowRgn(this->hw_, rgn, TRUE);
}

void Button::SetImg(HINSTANCE hinst, int rcid, int status, int x, int y, int w, int h) {
   HRSRC hrsc = ::FindResource(hinst, MAKEINTRESOURCE(rcid), TEXT("PNG"));
   DWORD size = ::SizeofResource(hinst, hrsc);
   LPVOID hg = ::LoadResource(hinst, hrsc);
   HGLOBAL mem = ::GlobalAlloc(GMEM_MOVEABLE, size);
   if (mem) {
      LPVOID pmem = ::GlobalLock(mem);
      if (pmem) {
         ::CopyMemory(pmem, hg, size);
         LPSTREAM is;
         if (::CreateStreamOnHGlobal(pmem, FALSE, &is) == S_OK) {
            Gdiplus::Image* m = new Gdiplus::Image(is);
            is->Release();
            if (m->GetLastStatus() != Gdiplus::Ok) {
               delete m;
               return;
            }
            img_info img;
            img.x = x;
            img.y = y;
            img.w = w;
            img.h = h;
            img.img = m;
            this->imgs_[status] = img;
         }
         ::GlobalUnlock(mem);
      }
      ::GlobalFree(mem);
   }
 }


