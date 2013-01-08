/*
 * button.cc
 */

#include "stdafx.h"
#include "button.h"
#include "resource.h"


/// Button
Button::Button() : statuse_(NORMAL) {
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

void Button::draw(HDC dc) {
   RECT rc;
   ::GetClientRect(this->hw_, &rc);
   int x = rc.left;
   int y = rc.top;
   int w = rc.right - rc.left - 1;
   int h = rc.bottom - rc.top - 1;

   HBITMAP memmap = ::CreateCompatibleBitmap(dc, w, h);
   HDC mdc = ::CreateCompatibleDC(dc); 
   HBITMAP oldmap = (HBITMAP)::SelectObject(mdc, (HGDIOBJ)memmap);
   Gdiplus::Graphics* g = new Gdiplus::Graphics(mdc);

   Gdiplus::SolidBrush br(this->bk_color_);
   g->FillRectangle(&br, x, y, w, h);

   if (!this->imgs_.empty()) {
      img_info img = this->imgs_[this->statuse_];
      Gdiplus::Rect rc(img.x, img.y, img.w, img.h);
      if (img.img == 0) {
         Gdiplus::Image m(img.fname.c_str());        
         g->DrawImage(&m, rc);
      } else {
         g->DrawImage(img.img, rc);
         //delete img.img;
      }
   }

   if (this->txt_.txt.length() > 0) {
      Gdiplus::PointF pt(this->txt_.x, this->txt_.y);
      Gdiplus::SolidBrush sbr(Gdiplus::Color::White);
      g->DrawString(this->txt_.txt.c_str(), this->txt_.txt.length(), this->txt_.font, pt, &sbr);
   }

   BitBlt(dc, 
      x, y, w, h, 
      mdc, x, y, SRCCOPY);

   delete g;
   ::SelectObject(mdc, oldmap);
   ::DeleteObject(memmap); 
   ::DeleteDC(mdc);
}

void Button::re_draw() {
   RECT rc;
   ::GetClientRect(this->hw_, &rc);
   ::InvalidateRect(this->hw_, &rc, TRUE);
}

LRESULT CALLBACK Button::WndProc(UINT message, WPARAM wParam, LPARAM lParam) {
   switch(message) {
   case WM_PAINT:
      {
         PAINTSTRUCT ps;
         HDC dc = ::BeginPaint(this->hw_, &ps);
         this->draw(dc);
         ::EndPaint(this->hw_, &ps);
      }
      break;
   case WM_SIZE: 
      {
         this->re_draw();
      }
      break;
   case WM_LBUTTONDOWN:
      /*
      this->btn_down_ = true;
      ::InvalidateRect(this->hw_, NULL, TRUE);
      ::SetCapture(this->hw_);
      */
      this->statuse_ = MLBTNDOWN;
      this->re_draw();
      break;
   case WM_LBUTTONUP:
      /*
      if (this->checkbox_) {
         this->checked_ = !this->checked_;
         this->btn_down_ = this->checked_;
      } else {
         this->btn_down_ = false;
      }
      ::InvalidateRect(this->hw_, NULL, TRUE);
      ::ReleaseCapture();
      //this->cw_->Notify(this->id_);
      */
      this->statuse_ = NORMAL;
      if (this->fn_) {
         fn_(this);
      }
      this->re_draw();
      break;
   case WM_MOUSEMOVE:
      {
        // this->statuse_ = MMOVING;
         ::SetCapture(this->hw_);
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
            }
         }
         this->re_draw();
      }
      break;
   case WM_MOUSELEAVE:
      //this->moving_ = false;
      this->statuse_ = NORMAL;
      break;
   case WM_ERASEBKGND: 
      {
         //HDC hdc = (HDC)wParam;
         //RECT rc;
         //::GetWindowRect(this->hw_, &rc);
         //int w = rc.right - rc.left;
         //int h = rc.bottom - rc.top;
         //HWND pw = ::GetParent(this->hw_);
         //POINT pt;
         //pt.x = rc.left;
         //pt.y = rc.top;
         //::ScreenToClient(pw, &pt);
         //rc.left = pt.x;
         //rc.top = pt.y;
         //rc.right = pt.x + w;
         //rc.bottom = pt.y = h;
         //::InvalidateRect(pw, &rc, 0);
         //HDC pdc = ::GetDC(pw);
         //::BitBlt(hdc, 0, 0, w, h, pdc, pt.x, pt.y, SRCCOPY);
         //::ReleaseDC(pw, pdc);
         //SetBkMode(hdc, TRANSPARENT);
         //this->draw(hdc);
      }
      break;
   default:
      return ::CallWindowProc(this->btn_proc_, this->hw_, message, wParam, lParam);
   }
   return 0;
}

int Button::Init(int id, HWND pw, HINSTANCE hinst, btn_click fn, int idc) {
   HWND hw = ::CreateWindowEx(0, WC_BUTTON, L"",
      WS_CHILD | WS_VISIBLE | BS_CENTER | BS_FLAT | BS_OWNERDRAW, 
      0, 0, 0, 0,
      pw, (HMENU)idc, hinst, NULL);
   if (!hw) {
      return -1;
   }

   this->hw_ = hw;
   this->fn_ = fn;
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
   ::UpdateWindow(this->hw_);
   ::ShowWindow(this->hw_, SW_SHOW);
   this->re_draw();
}

void Button::Show(int x, int y) {
   this->Show(x, y, 25, 25);
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


