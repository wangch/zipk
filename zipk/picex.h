/*
 * button.h
 */


#ifndef PICEX_H_
#define PICEX_H_

#include "unzip.h"
#include <vector>
#include <string>

struct node {
   node() : gif(false) {}
   ~node();
   std::vector<node*>  childs;
   std::wstring path;
   std::wstring name;
   std::wstring time;
   std::wstring sz;
   int type;
   unsigned int size;
   tm_unz tm;
   node* p;
   void* lp;
   int i;
   bool gif;
};

typedef std::vector<node*>::iterator NIT;

struct image {
   image() : hg(0), img(0) {}
   HGLOBAL hg;
   Gdiplus::Image* img;
};

class PicEx {
public:
   PicEx();
   ~PicEx();
   int Init(HWND pw, HINSTANCE hinst, unzFile uf);
   void set_img(node* n, image img);
   void show(int x, int y, int w, int h);
   void hide();
   image img() { return img_; }
   bool is_open() { return opened_; }

   HWND hw_;
   image img_;
   int time_;
   bool opened_;
   Gdiplus::Rect rc_;
   unzFile uf_;
   node* n_;
};

image unzip_img(node* n, unzFile uf);


#endif