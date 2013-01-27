/*
 * image.h
 */

#ifndef IMAGE_H_
#define IMAGE_H_

enum {
   FIT,
   ORI,
   ZOOM,
   SLIDER
};

class ImageCtrl {
public:
   ImageCtrl();
   ~ImageCtrl();

   void Init(HWND pw, HINSTANCE hinst);
   void SetImg(Gdiplus::Image* img);
   void Show(int s, int zoom = 0);
   LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
   void RotateFlip90();
   void RotateFlip270();
   Gdiplus::Rect rc() { return rc_; }
   Gdiplus::Image* img() { return img_; }
   HWND hw() { return hw_; }
   void set_cs(HANDLE m) { m_ = m; }
private:
   void draw(HDC dc);
   Gdiplus::Rect get_rc(int w, int h);
private:
   HWND hw_;
   Gdiplus::Image* img_;
   bool slider_show_;
   WNDPROC def_proc_;
   Gdiplus::Rect rc_;
   int status_;
   int zoom_;
   bool bigger_;
   int dx_, dy_;
   int px_, py_;
   HANDLE m_;
};

Gdiplus::Rect fit_rc(int w, int h, Gdiplus::Image* img);

#endif // IMAGE_H_