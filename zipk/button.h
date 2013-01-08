/*
 * button.h
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#include <string>
#include <map>

enum {
   NORMAL,
   MMOVING,
   MLBTNDOWN
};

struct img_info {
   img_info() : img(0) {}
   Gdiplus::Image* img;
   std::wstring fname;
   int x, y, w, h;
};

struct txt_info {
   std::wstring txt;
   int x, y;
   Gdiplus::Color color;
   Gdiplus::Font* font;
};

class Button;
typedef int (*btn_click) (Button*);

class Button {
public:
   Button();
   ~Button();
   int Init(int id, HWND pw, HINSTANCE hinst, btn_click fn, int idc = 0);
   void SetText(const std::wstring& txt, int x, int y);
   void SetTxtColor(Gdiplus::Color color);
   void SetTxtFont(Gdiplus::Font* f);
   void SetBkColor(Gdiplus::Color color);
   void SetImg(const std::wstring& fname, int status, int x, int y, int w, int h);
   void SetImg(HINSTANCE hinst, int rcid, int status, int x, int y, int w, int h);
   void Show(int x, int y, int w, int h);
   void Show(int x, int y);
   void SetRgn(HRGN rgn);
   HWND hw() { return hw_; }

   LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
   void draw(HDC dc);
   void re_draw();
private:
   HWND hw_;
   int id_;

   btn_click fn_;

   txt_info txt_;
   std::map<int, img_info> imgs_;
   
   Gdiplus::Color bk_color_;
   WNDPROC btn_proc_;
   int statuse_;
};

#endif