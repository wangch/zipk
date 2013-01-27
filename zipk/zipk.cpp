// zipk.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "zipk.h"
#include "button.h"
#include "picex.h"
#include "image.h"
#include "utf8conv.h"
#include "resource.h"

#include <Commdlg.h>
#include <Shellapi.h>
#include <commoncontrols.h>
#include <process.h>

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: 在此放置代码。
	MSG msg;
	HACCEL hAccelTable;

   ULONG_PTR gdiplusToken = 0;
   Gdiplus::GdiplusStartupInput gdiplusStartupInput;
   Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_ZIPK, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ZIPK));

	// 主消息循环:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

   Gdiplus::GdiplusShutdown(gdiplusToken);

	return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ZIPK));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_ZIPK);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

static bool split_path(const std::wstring& path, std::vector<std::wstring>& paths) {
   int pos = 0;
   std::size_t found = path.find_first_of(L'/');
   while (found != std::wstring::npos) {
      std::wstring s = path.substr(pos, found-pos);
      paths.push_back(s);
      pos = found + 1;
      found = path.find_first_of(L'/', pos);
   }
   bool foder = path[path.length()-1] == L'/';
   if (!foder) {
      paths.push_back(path.substr(pos));
   }
   return foder;
}

struct LV {
   LV() : root(0), sort(false) {}
   HWND pw;
   HWND toolbar;
   HWND lv;
   Gdiplus::Font* f;
   Gdiplus::Font* f1;
   bool moving;
   Button btns[4];
   unzFile unz;
   PicEx pe;
   node* root;
   node* n;
   int il_index[3];
   HIMAGELIST il;
   bool sort;
} g_lv;

node::~node() {
   NIT it = this->childs.begin();
   for (; it != this->childs.end(); ++it) {
      delete *it;
   }
}

std::wstring to_lower(const std::wstring& str) {
   std::wstring s(str);
   int len = s.length();
   for (int i = 0; i < len; ++i) {
      s[i] = ::tolower(s[i]);
   }
   return s;
}


static int add_node(node* n, const std::wstring& path) {
   std::vector<std::wstring> vp;
   bool is_folder = split_path(path, vp);

   node* r = g_lv.root;
   std::vector<std::wstring>::iterator itt = vp.begin();
   for (; itt != vp.end(); ++itt) {
      std::vector<node*>::iterator it = r->childs.begin();
      bool found = false;
      for (; it != r->childs.end(); ++it) {
         node* c = *it;
         if (*itt == c->name) {
            c->p = r;
            r = c;
            found = true;
            break;
         }
      }
      if (!found) {
         std::vector<std::wstring>::iterator ittt = vp.end();
         --ittt;
         if (ittt != itt) { 
            node* c = new node;
            c->name = *itt;
            c->p = r;
            c->type = 0;
            c->time = n->time;
            r->childs.push_back(c);
            r = c;
         } else {
            n->name = *itt;
            if (is_folder) {
               n->type = 0;
               n->size = 0;
            } else {
               n->type = 2;
               wchar_t* suffix[] = { L"jpg", L"jpeg", L"png", L"bmp", L"gif", L"tif", L"tiff"};
               int pos = n->name.find(L'.');
               if (pos != std::wstring::npos) {
                  std::wstring sf = n->name.substr(pos+1);
                  sf = to_lower(sf);
                  for (int i  = 0; i < 7; ++i) {
                     if (sf == suffix[i]) {
                        n->type = 1;
                        if (i == 4) {
                           n->gif = true;
                        }
                        break;
                     }
                  }
               }
            }
            n->p = r;
            r->childs.push_back(n);
            break;
         }
      }
   }
   return 0;
}

static int add_items1();

static unsigned int __stdcall display_tm(void* p) {
   return add_items1();
}

static void add_subtxt(int i, node* c) {
   wchar_t* s = const_cast<wchar_t*>(c->name.c_str());
   ListView_SetItemText(g_lv.lv, i, 0, s);
   s = const_cast<wchar_t*>(c->time.c_str());
   ListView_SetItemText(g_lv.lv, i, 1, s);
   int t = c->type;
   s = L"文件";
   if (t == 0) {
      s = L"文件夹";
   } else if (t == 1) {
      s = L"图片文件";
   }
   ListView_SetItemText(g_lv.lv, i, 2, s);
   if (c->size > 0) {
      s = const_cast<wchar_t*>(c->sz.c_str());
      ListView_SetItemText(g_lv.lv, i, 3, s);
   } else {
       ListView_SetItemText(g_lv.lv, i, 3, L"");
   }
}

static int add_items() {
   int style = ::SendMessage(g_lv.lv, LVM_GETVIEW, 0, 0);
   if (style == LV_VIEW_TILE) {
      unsigned int n;
      ::_beginthreadex(0, 0, display_tm, 0, 0, &n);
   }

   SHFILEINFO si;
   HIMAGELIST il = (HIMAGELIST)::SHGetFileInfo(L"", 0, &si, sizeof(si), SHGFI_SYSICONINDEX);
   ListView_SetImageList(g_lv.lv, il, LVSIL_NORMAL);

   ListView_DeleteAllItems(g_lv.lv);
   LVITEM lvi;
   lvi.pszText   = L"";//LPSTR_TEXTCALLBACK;
   lvi.mask      = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
   lvi.stateMask = 0;
   lvi.iSubItem  = 0;
   lvi.state     = 0;
   node* n = g_lv.n;
   NIT it = n->childs.begin();
   for (int i = 0; it != n->childs.end(); ++it, ++i) {
      node* c = *it;
      lvi.iItem = i;
      c->i = i;
      //SHFILEINFO si;
      if (c->type == 0) {
         SHGetFileInfo(L"folder", FILE_ATTRIBUTE_DIRECTORY, &si, sizeof(si),
            SHGFI_USEFILEATTRIBUTES|SHGFI_SMALLICON|SHGFI_LARGEICON|SHGFI_SYSICONINDEX);
      } else {
         int pos = c->name.find(L'.');
         if (pos != std::wstring::npos) {
            std::wstring sf = c->name.substr(pos);
            SHGetFileInfo(sf.c_str(), FILE_ATTRIBUTE_NORMAL, &si, sizeof(si),
               SHGFI_USEFILEATTRIBUTES|SHGFI_SMALLICON|SHGFI_LARGEICON|SHGFI_SYSICONINDEX);
         } else {
            SHGetFileInfo(L"file", FILE_ATTRIBUTE_NORMAL, &si, sizeof(si),
               SHGFI_USEFILEATTRIBUTES|SHGFI_SMALLICON|SHGFI_LARGEICON|SHGFI_SYSICONINDEX);
         }
      }
      lvi.iImage = si.iIcon;
      lvi.lParam = (LPARAM)c;
      ListView_InsertItem(g_lv.lv, &lvi);
      add_subtxt(i, c);
   }
   return 0;
}

static int add_items1() {
   ListView_DeleteAllItems(g_lv.lv);
   LVITEM lvi;
   lvi.pszText   = L"";
   lvi.mask      = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
   lvi.stateMask = 0;
   lvi.iSubItem  = 0;
   lvi.state     = 0;
   node* n = g_lv.n;
   ::ImageList_Destroy(g_lv.il);
   OSVERSIONINFO os;
   os.dwOSVersionInfoSize=sizeof(os);
   ::GetVersionEx(&os);

   if (os.dwMajorVersion > 5) { 
      g_lv.il = ::ImageList_Create(92, 70, ILC_COLOR32|ILC_MIRROR|ILC_ORIGINALSIZE|ILC_HIGHQUALITYSCALE, 1, 1);
   } else {
      g_lv.il = ::ImageList_Create(92, 70, ILC_COLOR32|ILC_MIRROR, 1, 1);
   }
   HIMAGELIST il = g_lv.il;
   ListView_SetImageList(g_lv.lv, il, LVSIL_NORMAL);

   std::vector<node*>::iterator it = n->childs.begin();
   for (int i = 0; it != n->childs.end(); ++it, ++i) {
      node* c = *it;
      lvi.iItem = i;
      if (c->type == 1) {
         image img = unzip_img(c, g_lv.unz);
         if (img.img) {
            Gdiplus::Image* pm;
            if (os.dwMajorVersion > 5) {
               Gdiplus::Rect rc = fit_rc(92, 70, img.img);
               pm = img.img->GetThumbnailImage(rc.Width, rc.Height);
            } else { 
               pm = img.img->GetThumbnailImage(92, 70);
            }
            Gdiplus::Bitmap* bmp = static_cast<Gdiplus::Bitmap*>(pm);
            if (bmp) {
               HBITMAP hbmp;
               bmp->GetHBITMAP(Gdiplus::Color(0,0,0), &hbmp);
               ::ImageList_Add(il, hbmp, NULL);
               ::DeleteObject((HGDIOBJ)hbmp);
               delete pm;
            }
            delete img.img;
            ::GlobalFree(img.hg);
         }
      } else {        
         SHFILEINFO si;
         if (c->type == 0) {
            SHGetFileInfo(L"folder", FILE_ATTRIBUTE_DIRECTORY, &si, sizeof(si),
               SHGFI_USEFILEATTRIBUTES|SHGFI_SMALLICON|SHGFI_LARGEICON|SHGFI_SYSICONINDEX);
         } else {
            int pos = c->name.find(L'.');
            if (pos != std::wstring::npos) {
               std::wstring sf = c->name.substr(pos);
               SHGetFileInfo(sf.c_str(), FILE_ATTRIBUTE_NORMAL, &si, sizeof(si),
                  SHGFI_USEFILEATTRIBUTES|SHGFI_SMALLICON|SHGFI_LARGEICON|SHGFI_SYSICONINDEX);
            } else {
               SHGetFileInfo(L"file", FILE_ATTRIBUTE_NORMAL, &si, sizeof(si),
                  SHGFI_USEFILEATTRIBUTES|SHGFI_SMALLICON|SHGFI_LARGEICON|SHGFI_SYSICONINDEX);
            }
         }
         HIMAGELIST* himglst;
         HRESULT r = ::SHGetImageList(SHIL_EXTRALARGE, IID_IImageList, (void**)&himglst);
         if (r == S_OK) {
            HICON hIcon;
            r = ((IImageList*)himglst)->GetIcon(si.iIcon, ILD_TRANSPARENT, &hIcon);
            if (r == S_OK) {    
               ImageList_AddIcon(il, hIcon);
            }
         }
      }
      lvi.iImage = i;
      lvi.lParam = (LPARAM)c;
      ListView_InsertItem(g_lv.lv, &lvi);
      add_subtxt(i, c);
   }
   return 0;
}

static wchar_t* format_thousands_separator(long  val) {
     static wchar_t buf[16][16];
     static int  c = 0;
 
     long m, n = 0;
     wchar_t* p = &buf[c++ % 16][15];
     *p = L'\0';
 
    do {
        m = val % 10;
        val = val / 10;
        *--p = L'0' + (wchar_t)(m < 0 ? -m : m);

        if (!val && m < 0) 
            *--p = L'-';

        if (val && !(++n % 3))
            *--p = L',';
    
    } while(val);

    return p;
}
static int read_file_info(unzFile uf) {
   unz_file_info fi;
   memset( &fi, 0, sizeof(fi));
   g_lv.unz = uf;
   const unsigned int MAX_COMMENT = 256;
   char fname[MAX_PATH + 1];
   char comment[MAX_COMMENT + 1];   
   unzGoToFirstFile(uf);

   do {
      if (unzGetCurrentFileInfo(uf, &fi, fname, MAX_PATH, NULL, 0, comment, MAX_COMMENT) != UNZ_OK) {
         return -1;
      }
      node* n = new node;
      std::wstring path = utf8util::UTF16FromUTF8(fname);
      wchar_t time[32];
      ::wsprintf(time, L"%d/%d/%d %d:%d", 
         fi.tmu_date.tm_year, fi.tmu_date.tm_mon, fi.tmu_date.tm_mday,
         fi.tmu_date.tm_hour, fi.tmu_date.tm_min);
      n->size = fi.uncompressed_size;
      n->tm = fi.tmu_date;
      n->sz = format_thousands_separator(n->size/1024+1);
      n->sz += L" KB";
      n->time = time;
      n->path = path;
      add_node(n, path);
   } while (unzGoToNextFile(uf) == UNZ_OK);
   add_items();

   return 0;
}


static int btn_click0(Button* btn) {
	OPENFILENAME ofn;       // common dialog box structure
	wchar_t szFile[260];       // buffer for file name

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
   ofn.hwndOwner = g_lv.pw;
	ofn.lpstrFile = szFile;
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	ofn.lpstrFile[0] = L'\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"zip file\0*.zip\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Display the Open dialog box. 
	if (::GetOpenFileName(&ofn) == TRUE) {
      std::string path = utf8util::UTF8FromUTF16(ofn.lpstrFile);
      unzClose(g_lv.unz);
      delete g_lv.root;
      g_lv.root = new node;
      g_lv.n = g_lv.root;
      unzFile uf = unzOpen(path.c_str());
      if (!uf) {
         ::MessageBox(0, L"zip 文件错误", L"错误", 0);
         return -1;
      }
      read_file_info(uf);
   }
   ::SendMessage(g_lv.lv, LVM_SETVIEW, LV_VIEW_ICON, 0);
   return 0;
}

static int btn_click1(Button* btn) {
   if(g_lv.n != g_lv.root) {
      g_lv.n = g_lv.n->p;
      add_items();
   } else { // 按钮变灰
   }
   return 0;
}

static int btn_click2(Button* btn) {
   return 0;
}

static int btn_click3(Button* btn) {
   int style = ::SendMessage(g_lv.lv, LVM_GETVIEW, 0, 0);
   if (style == LV_VIEW_DETAILS) {
      ::SendMessage(g_lv.lv, LVM_SETVIEW, LV_VIEW_TILE, 0);
   } else if (style == LV_VIEW_TILE) {
      ::SendMessage(g_lv.lv, LVM_SETVIEW, LV_VIEW_ICON, 0);
   } else if (style == LV_VIEW_ICON) {
      ::SendMessage(g_lv.lv, LVM_SETVIEW, LV_VIEW_LIST, 0);
   } else if (style == LV_VIEW_LIST) {
      ::SendMessage(g_lv.lv, LVM_SETVIEW, LV_VIEW_DETAILS, 0);
   }
   add_items();
   return 0;
}

static int init(HWND pw) {
   InitCommonControls();

   Gdiplus::FontFamily ff(L"宋体");
   Gdiplus::Font* f = new Gdiplus::Font(&ff, 9);
   Gdiplus::Font* f1 = new Gdiplus::Font(&ff, 12, 4);

   g_lv.btns[0].Init(0, pw, hInst, 0, btn_click0);
   g_lv.btns[1].Init(1, pw, hInst, 0, btn_click1);
   g_lv.btns[2].Init(2, pw, hInst, 0, btn_click2);
   g_lv.btns[3].Init(3, pw, hInst, 0, btn_click3);
           
   g_lv.btns[0].SetTxtFont(f);
   g_lv.btns[1].SetTxtFont(f);
   g_lv.btns[2].SetTxtFont(f);
   g_lv.btns[3].SetTxtFont(f);

   g_lv.btns[0].SetBkColor(Gdiplus::Color((69+96)/2, (110+141)/2, (149+198)/2));
   g_lv.btns[1].SetBkColor(Gdiplus::Color((69+96)/2, (110+141)/2, (149+198)/2));
   g_lv.btns[2].SetBkColor(Gdiplus::Color((69+96)/2, (110+141)/2, (149+198)/2));
   g_lv.btns[3].SetBkColor(Gdiplus::Color((69+96)/2, (110+141)/2, (149+198)/2));
           
   g_lv.btns[0].SetImg(L"res\\folder.png", 0, 0, 0, 24, 24);
   g_lv.btns[1].SetImg(L"res\\up.png", 0, 0, 0, 24, 24);
   g_lv.btns[2].SetImg(L"res\\home.png", 0, 0, 0, 24, 24);
   g_lv.btns[3].SetImg(L"res\\group.png", 0, 0, 0, 24, 24);

   g_lv.btns[0].SetImg(L"res\\folder.png", 1, 0, 0, 24, 24);
   g_lv.btns[1].SetImg(L"res\\up.png", 1, 0, 0, 24, 24);
   g_lv.btns[2].SetImg(L"res\\home.png", 1, 0, 0, 24, 24);
   g_lv.btns[3].SetImg(L"res\\group.png", 1, 0, 0, 24, 24);

   //g_lv.btns[0].SetImg(L"res\\folder.png", 2, 0, 0, 24, 24);
   //g_lv.btns[1].SetImg(L"res\\up.png", 2, 0, 0, 24, 24);
   //g_lv.btns[2].SetImg(L"res\\home.png", 2, 0, 0, 24, 24);
   //g_lv.btns[3].SetImg(L"res\\group.png", 2, 0, 0, 24, 24);

   g_lv.btns[0].SetText(L"打開", 30, 6);
   g_lv.btns[1].SetText(L"向上", 30, 6);
   g_lv.btns[2].SetText(L"主页", 30, 6);
   g_lv.btns[3].SetText(L"视图", 30, 6);

   g_lv.pw = pw;
   g_lv.f = f;
   g_lv.f1 = f1;
   g_lv.moving = false;

   HWND hw = ::CreateWindowEx(LVS_EX_DOUBLEBUFFER, WC_LISTVIEW, TEXT(""),
      WS_CHILD | WS_VISIBLE | LVS_ALIGNTOP, 
      18, 84, 400, 225, pw, (HMENU)IDC_TV, hInst, 0);
   if (!hw) {
      return -1;
   }
   g_lv.lv = hw;

   SHFILEINFO si;
   HIMAGELIST il = (HIMAGELIST)::SHGetFileInfo(L"", 0, &si, sizeof(si), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
   ListView_SetImageList(g_lv.lv, il, LVSIL_SMALL);
   il = (HIMAGELIST)::SHGetFileInfo(L"", 0, &si, sizeof(si), SHGFI_SYSICONINDEX | SHGFI_LARGEICON);
   ListView_SetImageList(g_lv.lv, il, LVSIL_NORMAL);
   g_lv.il = ::ImageList_Create(92, 70, ILC_COLOR32, 1, 1);

   wchar_t* arr[4] = { L"名称", L"修改日期", L"类型", L"大小" };
   LVCOLUMN lvc;
   lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;// | LVCF_ORDER;
   lvc.fmt = LVCFMT_LEFT;
   for (int i = 0; i < 4; ++i) {
      lvc.iSubItem = i;
      lvc.pszText = arr[i];
      lvc.cx = 160;     
      if (i > 1) {
         lvc.fmt = LVCFMT_RIGHT;
      } else {
         lvc.fmt = LVCFMT_LEFT;
      }
      ListView_InsertColumn(g_lv.lv, i, &lvc);
   }
   return 0;
}


static void open_exp(node* n, int x, int y, int w, int h) {
   g_lv.pe.Init(NULL, hInst, g_lv.unz);
   g_lv.pe.show(x, y, w, h);
   image img = unzip_img(n, g_lv.unz);
   if (img.img) {
      g_lv.pe.set_img(n, img);
   } else {
      ::MessageBox(0, L"错误", L"错误", 0);
   }
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
   UINT  num = 0;          // number of image encoders
   UINT  size = 0;         // size of the image encoder array in bytes

   Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

   Gdiplus::GetImageEncodersSize(&num, &size);
   if(size == 0)
      return -1;  // Failure

   pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
   if(pImageCodecInfo == NULL)
      return -1;  // Failure

   Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

   for(UINT j = 0; j < num; ++j)
   {
      if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
      {
         *pClsid = pImageCodecInfo[j].Clsid;
         free(pImageCodecInfo);
         return j;  // Success
      }    
   }

   free(pImageCodecInfo);
   return -1;  // Failure
}

static void do_draw(HWND hWnd, HDC dc) {
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

   Gdiplus::Rect grc(x, y, w, h);
   Gdiplus::LinearGradientBrush lbr(grc, 
      Gdiplus::Color(69, 110, 149), Gdiplus::Color(96, 141, 198), 0);
   g->FillRectangle(&lbr, x, y, w, h);

   int top_h = 64;
   int bar_h = 32;
   int bar_w = 375;

   g->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
   Gdiplus::GraphicsPath path;
   path.AddArc(60, (top_h-bar_h)/2, 33, 32, -240, 120);
   path.AddArc((60+bar_w), (top_h-bar_h)/2, 33, 32, -60, 120);
   path.CloseFigure();

   Gdiplus::Color line_color(133, 144, 160);
   Gdiplus::Pen pen(line_color, 1);
   Gdiplus::SolidBrush sbr(Gdiplus::Color((69+96)/2, (110+141)/2, (149+198)/2));
   g->DrawPath(&pen, &path);
   g->FillPath(&sbr, &path);

   BitBlt(dc, 
      x, y, w, h, 
      mdc, x, y, SRCCOPY);
   delete g;
   ::SelectObject(mdc, oldmap);
   ::DeleteObject(memmap); 
   ::DeleteDC(mdc);
}

static void do_size(int w, int h) {
   ::MoveWindow(g_lv.lv, 18, 84, w - 18, h-102, TRUE);

   int btn_top = 16+3;
   int w0 = 100;

   g_lv.btns[0].Show(w0, btn_top, 60, 26);
   g_lv.btns[1].Show(w0+80, btn_top, 60, 26);
   g_lv.btns[2].Show(w0+80+80, btn_top, 60, 26);
   g_lv.btns[3].Show(w0+80+80+80, btn_top, 60, 26);
};

static void do_mm(int x, int y) {
   if ((x > 18 && x < 80) && (y > 18 && y < 48)) {
      g_lv.moving = true;
   } else {
      g_lv.moving = false;
   }
   //::UpdateWindow(g_lv.pw);
   RECT rc;
   ::GetClientRect(g_lv.pw, &rc);
   rc.bottom = 48;
   ::InvalidateRect(g_lv.pw, &rc, TRUE);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 将实例句柄存储在全局变量中

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }
   ::SetMenu(hWnd, 0);
   init(hWnd);
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

int cmpfn_name(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
   node* n1 = (node*)lParam1;
   node* n2 = (node*)lParam2;
   if (g_lv.sort) {
      return ::lstrcmp(n1->name.c_str(), n2->name.c_str());
   }
   return -::lstrcmp(n1->name.c_str(), n2->name.c_str());
}

int cmpfn_time(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
   node* n1 = (node*)lParam1;
   node* n2 = (node*)lParam2;
   int t1 = n1->tm.tm_year - n2->tm.tm_year;
   if (t1 == 0) {
      t1 = n1->tm.tm_mon - n2->tm.tm_mon;
   }
   if (t1 == 0) {
      t1 = n1->tm.tm_mday - n2->tm.tm_mday;
   }
   if (t1 == 0) {
      t1 = n1->tm.tm_hour - n2->tm.tm_hour;
   }
   if (t1 == 0) {
      t1 = n1->tm.tm_min - n2->tm.tm_min;
   }
   if (t1 == 0) {
      t1 = n1->tm.tm_sec - n2->tm.tm_sec;
   }
   if (g_lv.sort) {
      return t1;
   }
   return -t1;;
}

int cmpfn_type(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
   node* n1 = (node*)lParam1;
   node* n2 = (node*)lParam2;
   if (g_lv.sort) {
      return n1->type - n2->type;
   }
   return n2->type - n1->type;
}

int cmpfn_size(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
   node* n1 = (node*)lParam1;
   node* n2 = (node*)lParam2;
   if (g_lv.sort) {
      return n1->size - n2->size;
   }
   return n2->size - n1->size;
}

int CALLBACK cmp_fn(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
   int i = (int)lParamSort;
   int r = 0;
   if (i == 0) {
      r = cmpfn_name(lParam1, lParam2, lParamSort);
   } else if (i == 1) {
      r = cmpfn_time(lParam1, lParam2, lParamSort);
   } else if (i == 2) {
      r = cmpfn_type(lParam1, lParam2, lParamSort);
   } else if (i == 3) {
      r = cmpfn_size(lParam1, lParam2, lParamSort);
   }
   return r;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: 处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此添加任意绘图代码...
      do_draw(hWnd, hdc);
		EndPaint(hWnd, &ps);
		break;
   case WM_LBUTTONDBLCLK:
      {
      }
      break;
   case WM_NOTIFY:
      {
         switch (((LPNMHDR)lParam)->code) {
         case NM_DBLCLK:
            {
               LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE)lParam;
               int i = lpnmitem->iItem;
               if (i < 0) {
                  break;
               }
               node* n = g_lv.n->childs[i];
               if (n->type == 0) {
                  g_lv.n = n;
                  add_items();
               } 
               if (n->type == 1) {
                  open_exp(n, 300, 200, 600, 500);
               }
               ::InvalidateRect(hWnd, NULL, TRUE);
            }
            break;
         case LVN_COLUMNCLICK:
            {
               LPNMLISTVIEW pnm = (LPNMLISTVIEW)lParam;
               ListView_SortItems(g_lv.lv, cmp_fn,(LPARAM)pnm->iSubItem);
               g_lv.sort = !g_lv.sort;

            }
            break;
        } // switch (((LPNMHDR)lParam)->code)
      }
      break;
   case WM_SIZE:
      do_size(LOWORD(lParam), HIWORD(lParam));
      ::InvalidateRect(hWnd, NULL, 1);
      break;
   case WM_ERASEBKGND:
      break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
