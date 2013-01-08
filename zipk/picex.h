/*
 * button.h
 */

#ifndef PICEX_H_
#define PICEX_H_

class PicEx {
public:
   PicEx();
   ~PicEx();
   int Init(HWND pw, HINSTANCE hinst, int idc = 0);

private:
   HWND hw_;
};

#endif