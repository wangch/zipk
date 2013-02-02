/*
 * image.cc
 */

#include "stdafx.h"
#include "resource.h"
#include "ShowStyle.h"
#include "image.h"
#include <Windowsx.h>


ImageCtrl ::ImageCtrl() : img_(0), status_(false), bigger_(false), dx_(0), dy_(0), draging_(false) {
}

ImageCtrl ::~ImageCtrl() {
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

static Gdiplus::Rect zoom_rc(int w, int h, Gdiplus::Image* img, int zoom) {
   Gdiplus::Rect rc = fit_rc(w, h, img);
   int w0 = zoom * rc.Width;
   int h0 = zoom * rc.Height;
   int x0 = rc.X + (rc.Width - w0) / 2;
   int y0 = rc.Y + (rc.Height - h0) / 2;
   rc.X = x0;
   rc.Y = y0;
   rc.Width = w0;
   rc.Height = h0;
   return rc;
}

static void drag_rc(Gdiplus::Rect& rc, int& dx, int& dy, int w, int h) {
   int x = rc.X;
   int y = rc.Y;
   if (rc.Width > w) {
      rc.X += dx;
      if (rc.X > 0) {
         dx = -x;
         rc.X = 0;
      }
      if (rc.X + rc.Width < w) {
         dx = w - rc.Width - x;
         rc.X = w - rc.Width;
      }
   }
   if (rc.Height > h) {
      rc.Y += dy;
      if (rc.Y> 0) {
         dy = -y;
         rc.Y = 0;
      }
      if (rc.Y + rc.Height < h) {
         dy = h - rc.Height - y;
         rc.Y = h - rc.Height;
      }
   }
}

Gdiplus::Rect ImageCtrl::get_rc(int w, int h) {
   int s = this->status_;
   Gdiplus::Rect rc;
   if (s == FIT) {
      this->dx_ = 0;
      this->dy_ = 0;
      rc = fit_rc(w, h, this->img_);
   } else if (s == ORI) {
      rc = ori_rc(w, h, this->img_);
   } else if (s == ZOOM) {
      rc = zoom_rc(w, h, this->img_, this->zoom_);
   } else {
      this->dx_ = 0;
      this->dy_ = 0;
      rc = Gdiplus::Rect(0, 0, w, h);
   }
   this->bigger_ = rc.Width > w || rc.Height > h;
   drag_rc(rc, this->dx_, this->dy_, w, h);
   return rc;
}


void ImageCtrl::Show(int s, int zoom) {
   if (!this->img_) {
      return;
   }

   this->status_ = s;
   this->zoom_ = zoom;
   do_draw(0, 0);
}

void ImageCtrl::do_size(int w, int h) {
   HDC dc = ::GetDC(this->hw_);
   HDC mdc = ::CreateCompatibleDC(dc); 
   ::DeleteDC(this->memdc_);
   this->memdc_ = mdc;
   this->do_draw(w, h);
}

void ImageCtrl::do_draw(int w, int h) {
   if (w == 0) {
      RECT rc;
      ::GetClientRect(this->hw_, &rc);
      w = rc.right - rc.left;
      h = rc.bottom - rc.top;
   }
   HDC dc = ::GetDC(this->hw_);
   HBITMAP memmap = ::CreateCompatibleBitmap(dc, w, h);
   HDC mdc = this->memdc_; 
   HBITMAP oldmap = (HBITMAP)::SelectObject(mdc, (HGDIOBJ)memmap);

   Gdiplus::Graphics g(mdc);
   Gdiplus::SolidBrush br(Gdiplus::Color::White);
   g.FillRectangle(&br, 0, 0, w, h);

   if (this->img_) {
      g.DrawImage(this->img_, get_rc(w, h));
      if (this->status_ == SLIDER) {
         MagicShow(mdc, 0, 0, dc, 0, 0, w, h, 10, 19, ::rand()%19);
      }
   }
   
   ::SelectObject(mdc, oldmap);
   ::DeleteObject(memmap); 
   if (!this->draging_) {
      ::InvalidateRect(this->hw_, 0, FALSE);
   }
}

void ImageCtrl::re_draw(PAINTSTRUCT* ps) {
   RECT rc;
   ::GetClientRect(this->hw_, &rc);
   int w = rc.right - rc.left;
   int h = rc.bottom - rc.top;

   if (this->draging_) {
      this->do_draw(0, 0);
      this->draging_ = false;
   }
   HDC dc = ps->hdc;
   rc = ps->rcPaint;
   int x = rc.left;
   int y = rc.top;
   BitBlt(dc, 
      x, y, rc.right-x, rc.bottom-y, 
      this->memdc_, x, y, SRCCOPY);
}

static LRESULT CALLBACK _WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
   ImageCtrl* imgctrl = (ImageCtrl*)::GetWindowLongPtr(hWnd, GWLP_USERDATA);
   if (!imgctrl) {
		return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return imgctrl->WndProc(hWnd, message, wParam, lParam);
}

LRESULT CALLBACK ImageCtrl::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
   switch(message) {
   case WM_PAINT:
      {
         PAINTSTRUCT ps;
         HDC dc = ::BeginPaint(hWnd, &ps);
         this->re_draw(&ps);
         ::EndPaint(hWnd, &ps);
      }
      break;
   case WM_SIZE:
      {
         this->do_size(LOWORD(lParam), HIWORD(lParam));
      }
      break;
   case WM_LBUTTONDOWN:
      {
         if (this->bigger_) {
            this->px_ = GET_X_LPARAM(lParam);
            this->py_ = GET_Y_LPARAM(lParam);
         }
      }
      break;
   case WM_MOUSEMOVE:
      {
         if (this->bigger_) {
            if (MK_LBUTTON == wParam) {
               int x = GET_X_LPARAM(lParam);
               int y = GET_Y_LPARAM(lParam);
               this->dx_ += x - this->px_; 
               this->dy_ += y - this->py_; 
               this->px_ = x;
               this->py_ = y;
               this->draging_ = true;
               ::InvalidateRect(this->hw_, 0, FALSE);
            }
         } else {
            ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
         }
      }
      break;
   case WM_ERASEBKGND:
      break;
	default:
      return ::CallWindowProc(this->def_proc_, hWnd, message, wParam, lParam);
	}
	return 0;
}

void ImageCtrl::Init(HWND pw, HINSTANCE hinst) {
   HWND hw = ::CreateWindowEx(0, WC_BUTTON, L"",
       WS_CHILD | WS_VISIBLE | BS_CENTER | BS_FLAT | BS_OWNERDRAW, 
      0, 0, 0, 0,
      pw, (HMENU)IDC_STATIC, hinst, NULL);
   if (!hw) {
      return;
   }
   this->hw_ = hw;
   ::SetWindowLongPtr(hw, GWLP_USERDATA, (LONG)this);
   this->def_proc_ = (WNDPROC)::SetWindowLongPtr(hw, GWLP_WNDPROC, (LONG)_WndProc);
   ::ShowWindow(hw, SW_SHOW);
}

void ImageCtrl::SetImg(Gdiplus::Image* img) {
   this->img_ = img;
   this->do_draw(0, 0);
}

void ImageCtrl::RotateFlip90() {
   if (!this->img_) {
      return;
   }
   this->img_->RotateFlip(Gdiplus::Rotate90FlipNone);
   this->do_draw(0, 0);
}

void ImageCtrl::RotateFlip270() {
   if (!this->img_) {
      return;
   }
   this->img_->RotateFlip(Gdiplus::Rotate270FlipNone);
   this->do_draw(0, 0);
}

