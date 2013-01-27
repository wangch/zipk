///////////////////////////////////////////////////////////////////
//Author: Jedyking
//http://www.appspeed.com
//
//This library is freeware, however, you may not sale or charge any money for it. 
//This code is supplied as-is. I make no guarantees about the suitability
//of this code - use at your own risk.

//Visit my web-page for other software!
//www.appspeed.com

#include "stdafx.h"
#include "ShowStyle.h"
#include <time.h>


#define		MAGIC_STYLE_NUM		19

bool MagicFun1( HDC hSrcDC , int x , int y , HDC hDestDC , int xd , int yd , int width , int height , int nDelay, int nStep);
bool MagicFun2( HDC hSrcDC , int x , int y , HDC hDestDC , int xd , int yd , int width , int height , int nDelay, int nStep);
bool MagicFun3( HDC hSrcDC , int x , int y , HDC hDestDC , int xd , int yd , int width , int height , int nDelay, int nStep);
bool MagicFun4( HDC hSrcDC , int x , int y , HDC hDestDC , int xd , int yd , int width , int height , int nDelay, int nStep);
bool MagicFun5( HDC hSrcDC , int x , int y , HDC hDestDC , int xd , int yd , int width , int height , int nDelay, int nStep);
bool MagicFun6( HDC hSrcDC , int x , int y , HDC hDestDC , int xd , int yd , int width , int height , int nDelay, int nStep);
bool MagicFun7( HDC hSrcDC , int x , int y , HDC hDestDC , int xd , int yd , int width , int height , int nDelay, int nStep);
bool MagicFun8( HDC hSrcDC , int x , int y , HDC hDestDC , int xd , int yd , int width , int height , int nDelay, int nStep);
bool MagicFun9( HDC hSrcDC , int x , int y , HDC hDestDC , int xd , int yd , int width , int height , int nDelay, int nStep);
bool MagicFun10( HDC hSrcDC , int x , int y , HDC hDestDC , int xd , int yd , int width , int height , int nDelay, int nStep);
bool MagicFun11( HDC hSrcDC , int x , int y , HDC hDestDC , int xd , int yd , int width , int height , int nDelay, int nStep);
bool MagicFun12( HDC hSrcDC , int x , int y , HDC hDestDC , int xd , int yd , int width , int height , int nDelay, int nStep);
bool MagicFun13( HDC hSrcDC , int x , int y , HDC hDestDC , int xd , int yd , int width , int height , int nDelay, int nStep);
bool MagicFun14( HDC hSrcDC , int x , int y , HDC hDestDC , int xd , int yd , int width , int height , int nDelay, int nStep);
bool MagicFun15( HDC hSrcDC , int x , int y , HDC hDestDC , int xd , int yd , int width , int height , int nDelay, int nStep);
bool MagicFun16( HDC hSrcDC , int x , int y , HDC hDestDC , int xd , int yd , int width , int height , int nDelay, int nStep);
bool MagicFun17( HDC hSrcDC , int x , int y , HDC hDestDC , int xd , int yd , int width , int height , int nDelay, int nStep);
bool MagicFun18( HDC hSrcDC , int x , int y , HDC hDestDC , int xd , int yd , int width , int height , int nDelay, int nStep);
bool MagicFun19( HDC hSrcDC , int x , int y , HDC hDestDC , int xd , int yd , int width , int height , int nDelay, int nStep);
bool MagicFun20( HDC hSrcDC , int x , int y , HDC hDestDC , int xd , int yd , int width , int height , int nDelay, int nStep);

typedef bool (*LPMAGICFUNCTION)( HDC hSrcDC , int x , int y , HDC hDestDC , int xd , int yd , int width , int height , int nDelay, int nStep);

LPMAGICFUNCTION MagicFuns[ MAGIC_STYLE_NUM ]=
{
	MagicFun1,
	MagicFun2,
	MagicFun3,
	MagicFun4,
	MagicFun5,
	MagicFun6,
	MagicFun7,
	MagicFun8,
	MagicFun9,
	//MagicFun10,
	MagicFun11,
	MagicFun12,
	MagicFun13,
	MagicFun14,
	MagicFun15,
	MagicFun16,
	MagicFun17,
	MagicFun18,
	MagicFun19,
	MagicFun20
};

bool MagicShow( HDC hSrcDC , int x , int y , HDC hDestDC , int xd , int yd , int width , int height , int nDelay, int nStep, int nStyle  )
{
	if( nStyle < 0 || nStyle >= MAGIC_STYLE_NUM ) nStyle = 0;
	return (*MagicFuns[nStyle])( hSrcDC ,  x ,  y ,  hDestDC , xd , yd , width , height  ,nDelay , nStep );
}

bool SleepEx( DWORD dwMilliseconds )
{
	if( dwMilliseconds == 0 ) return TRUE;
   ::Sleep(dwMilliseconds);

	//DWORD t1 = GetTickCount();
	//while(1)
	//{
	//	DWORD t2 = GetTickCount();
	//	if( t2 < t1 || (t2 - t1) > dwMilliseconds )break;
	//	
	//	MSG msg;
	//	if( PeekMessage(&msg, NULL,  0, 0, PM_NOREMOVE) ) 
	//	{
	//		GetMessage( &msg, NULL,  0, 0 );
	//		TranslateMessage(&msg); 
	//		DispatchMessage(&msg); 
	//	}
	//	else
	//	{
	//		Sleep(1);
	//	}
	//}
	return TRUE;
}

bool MagicFun1( HDC hSrcDC , int x , int y , HDC hDestDC , int xd , int yd , int width , int height , int nDelay, int nStep)
{
	if( nStep < 2 ) nStep = 2;

	int num = width / nStep;
	for(int i=0;i<nStep;i++)
	{
		for(int j=0;j<num;j++)
		{
			BitBlt( hDestDC , xd + j*nStep+i , yd, 1 , height , hSrcDC , x+j*nStep+i,y,SRCCOPY);  
		}
		SleepEx( nDelay );
	}
	BitBlt( hDestDC , xd , yd , width , height ,hSrcDC,x,y,SRCCOPY);
	return TRUE;
}

bool MagicFun2( HDC hSrcDC , int x , int y , HDC hDestDC , int xd , int yd , int width , int height , int nDelay, int nStep)
{
	if( nStep < 2 ) nStep = 2;
	int num=height/nStep;
	for(int i=0;i<nStep;i++)
	{
		for(int j=0;j<num;j++)
		{
			BitBlt( hDestDC,xd,yd+j*nStep+i,width,1, hSrcDC,x, y+j*nStep+i,SRCCOPY);
		}
		SleepEx( nDelay);
	}
	BitBlt( hDestDC , xd , yd , width , height ,hSrcDC,x,y,SRCCOPY);
	return TRUE;
}

bool MagicFun3( HDC hSrcDC , int x , int y , HDC hDestDC , int xd , int yd , int width , int height , int nDelay, int nStep)
{
	if( nStep < 1 )nStep = 1;

	for(int i=0;i<width; i+=nStep )
	{
		BitBlt(hDestDC, xd+i,yd,nStep,height,hSrcDC,x+i,y,SRCCOPY);
		SleepEx(nDelay);
	}
	BitBlt( hDestDC , xd , yd , width , height ,hSrcDC,x,y,SRCCOPY);
	return TRUE;
}

bool MagicFun4( HDC hSrcDC , int x , int y , HDC hDestDC , int xd , int yd , int width , int height , int nDelay, int nStep )
{
	if( nStep < 1 )nStep = 1;
	for(int i=width;i>=0; i-=nStep )
	{
		BitBlt(hDestDC, xd+i -nStep ,yd,nStep,height,hSrcDC,x+i-nStep,y,SRCCOPY);
		SleepEx(nDelay);
	}
	BitBlt( hDestDC , xd , yd , width , height ,hSrcDC,x,y,SRCCOPY);
	return TRUE;
}

bool MagicFun5( HDC hSrcDC , int x , int y , HDC hDestDC , int xd , int yd , int width , int height , int nDelay, int nStep )
{
	if( nStep < 1 ) nStep = 1;
	int nNum = width / nStep + 1;
	int* pPos = (int*) malloc( nNum * sizeof(int) );
	memset( pPos , 0 , sizeof(int)*nNum );

	srand( (unsigned)time( NULL ) );
	bool bStoped = FALSE;
	while( !bStoped )
	{
		bStoped = TRUE;
		for( int i = 0 ; i< nNum; i++ )
		{
			if( pPos[i] < height )
			{
				int nSize = rand() % 10;
				BitBlt( hDestDC , xd+i*nStep , yd+pPos[i] , nStep , nSize , hSrcDC , x+i*nStep , y+pPos[i] , SRCCOPY );
				pPos[i] += nSize;
				bStoped = FALSE;
			}
		}
		SleepEx( nDelay );
	}	
//	BitBlt( hDestDC , xd , yd , width , height ,hSrcDC,x,y,SRCCOPY);
	free(pPos);
	return TRUE;
}

bool MagicFun6( HDC hSrcDC , int x , int y , HDC hDestDC , int xd , int yd , int width , int height , int nDelay, int nStep )
{
	if( nStep < 1 ) nStep = 1;
	int nNum = width / nStep + 1;
	int* pPos = (int*) malloc( nNum * sizeof(int) );
	memset( pPos , 0 , sizeof(int)*nNum );

	srand( (unsigned)time( NULL ) );
	bool bStoped = FALSE;
	while( !bStoped )
	{
		bStoped = TRUE;
		for( int i = 0 ; i< nNum; i++ )
		{
			if( pPos[i] < height )
			{
				int nSize = rand() % 10;
				BitBlt( hDestDC , xd+i*nStep , yd+height-pPos[i]-nSize , nStep , nSize , hSrcDC , x+i*nStep , y+height-pPos[i]-nSize , SRCCOPY );
				pPos[i] += nSize;
				bStoped = FALSE;
			}
		}
		SleepEx( nDelay );
	}	
//	BitBlt( hDestDC , xd , yd , width , height ,hSrcDC,x,y,SRCCOPY);
	free(pPos);
	return TRUE;
}

bool MagicFun7( HDC hSrcDC , int x , int y , HDC hDestDC , int xd , int yd , int width , int height , int nDelay, int nStep )
{
	if( nStep < 1 ) nStep = 1;
	int nNum = height / nStep + 1;
	int* pPos = (int*) malloc( nNum * sizeof(int) );
	memset( pPos , 0 , sizeof(int)*nNum );

	srand( (unsigned)time( NULL ) );
	bool bStoped = FALSE;
	while( !bStoped )
	{
		bStoped = TRUE;
		for( int i = 0 ; i< nNum; i++ )
		{
			if( pPos[i] < width )
			{
				int nSize = rand() % 10;
				BitBlt( hDestDC , xd+pPos[i] , yd+i*nStep , nSize, nStep , hSrcDC , x+pPos[i] , y+i*nStep , SRCCOPY );
				pPos[i] += nSize;
				bStoped = FALSE;
			}
		}
		SleepEx( nDelay );
	}	
	free(pPos);
//	BitBlt( hDestDC , xd+width , yd , width , height ,hSrcDC,x,y,SRCCOPY);
	return TRUE;
}


bool MagicFun8( HDC hSrcDC , int x , int y , HDC hDestDC , int xd , int yd , int width , int height , int nDelay, int nStep )
{
	if( nStep < 1 ) nStep = 1;
	int nNum = height / nStep + 1;
	int* pPos = (int*) malloc( nNum * sizeof(int) );
	memset( pPos , 0 , sizeof(int)*nNum );

	srand( (unsigned)time( NULL ) );
	bool bStoped = FALSE;
	while( !bStoped )
	{
		bStoped = TRUE;
		for( int i = 0 ; i< nNum; i++ )
		{
			if( pPos[i] < width )
			{
				int nSize = rand() % 10;
				BitBlt( hDestDC , xd+width-pPos[i]-nSize , yd+i*nStep , nSize, nStep , hSrcDC , x+width-pPos[i]-nSize , y+i*nStep , SRCCOPY );
				pPos[i] += nSize;
				bStoped = FALSE;
			}
		}
		SleepEx( nDelay );
	}	
	free(pPos);

	return TRUE;
}


//===================================================================================
//random show
//===================================================================================
bool MagicFun9( HDC hSrcDC , int x , int y , HDC hDestDC , int xd , int yd, int width , int height , int nDelay, int nStep )
{
    if( nStep < 4 ) nStep = 4;
    
    int nXNum = width / nStep + 1  , nYNum = height / nStep + 1;
	
	//
	srand( (unsigned)time( NULL ) );
	int nCount = nXNum * nYNum;

	while( nCount > 0 )
	{
		int nX = rand()%nXNum;
		int nY = rand()%nYNum;

		BitBlt( hDestDC , xd+nX*nStep , yd+nY*nStep , nStep , nStep , hSrcDC , x+nX*nStep , y+nY*nStep , SRCCOPY );

		if( nCount% 10 == 0 )
		{
			SleepEx( nDelay );
		}
		nCount--;
	}
	BitBlt( hDestDC , xd , yd , width , height , hSrcDC , x , y , SRCCOPY );
	return TRUE;
}

bool MagicFun10( HDC hSrcDC , int x , int y , HDC hDestDC , int xd , int yd , int width , int height , int nDelay, int nStep )
{
    if( nStep < 4 ) nStep = 4;
    
    int nXNum = width / nStep + 1  , nYNum = height / nStep + 1;
	
	int nLoop = 0;
	while( nLoop < nYNum/2 )
	{
		for( int i = 0 ; i< nXNum-nLoop; i++ )
		{
			BitBlt( hDestDC , xd+i*nStep , yd+nLoop*nStep , nStep , nStep , hSrcDC , x+i*nStep , y+nLoop*nStep , SRCCOPY );
			SleepEx(nDelay);
		}

		for(int  i = 0 ; i< nYNum-nLoop; i++ )
		{
			BitBlt( hDestDC , xd+width-(nLoop+1)*nStep , yd+i*nStep , nStep , nStep , hSrcDC , x+width-(nLoop+1)*nStep , y+i*nStep , SRCCOPY );
			SleepEx(nDelay);
		}

		for( int i = nXNum-nLoop ; i >=0 ; i-- )
		{
			BitBlt( hDestDC , xd+(i-1)*nStep , yd+height-(nLoop+1)*nStep , nStep , nStep , hSrcDC , x+(i-1)*nStep ,y+height-(nLoop+1)*nStep , SRCCOPY );
			SleepEx(nDelay);
		}

		for( int i = nYNum-nLoop ; i>=0 ; i-- )
		{
			BitBlt( hDestDC , xd+nLoop*nStep , yd+(i-1)*nStep , nStep , nStep , hSrcDC , x+nLoop*nStep , y+(i-1)*nStep , SRCCOPY );
			SleepEx(nDelay);
		}

		nLoop++;
	}
	BitBlt( hDestDC , xd , yd , width , height , hSrcDC , x , y , SRCCOPY );
	return TRUE;
}

//---------------------------------------------------------------------
//left to right stretch show
//---------------------------------------------------------------------
bool MagicFun11( HDC hSrcDC , int x , int y , HDC hDestDC , int xd , int yd , int width , int height , int nDelay, int nStep )
{
	HDC hBuffDC = CreateCompatibleDC(hDestDC);
	HBITMAP hBitmap = CreateCompatibleBitmap( hDestDC, width , height );
	SelectObject( hBuffDC , hBitmap );
	
	BitBlt( hBuffDC , 0 , 0 , width , height , hDestDC , xd , yd , SRCCOPY );

	for( int i=0; i< width/nStep+1; i++ )
	{
		
		StretchBlt( hBuffDC , 0 , 0 , (i+1)*nStep , height , hSrcDC , x , y , width , height , SRCCOPY );
		BitBlt( hDestDC , xd , yd , (i+1)*nStep , height , hBuffDC , 0 , 0 , SRCCOPY );
		SleepEx( nDelay );
	}
	BitBlt( hDestDC , xd , yd , width , height , hSrcDC , x , y , SRCCOPY );
	DeleteObject( hBitmap );
	DeleteDC( hBuffDC );
	return TRUE;
}

//---------------------------------------------------------------------
//right to left stretch show
//---------------------------------------------------------------------
bool MagicFun12( HDC hSrcDC , int x , int y , HDC hDestDC , int xd , int yd , int width , int height , int nDelay, int nStep )
{
	HDC hBuffDC = CreateCompatibleDC(hDestDC);
	HBITMAP hBitmap = CreateCompatibleBitmap( hDestDC, width , height );
	SelectObject( hBuffDC , hBitmap );
	
	BitBlt( hBuffDC , 0 , 0 , width , height , hDestDC , xd , yd , SRCCOPY );

	for( int i=0; i< width/nStep+1; i++ )
	{
		
		StretchBlt( hBuffDC , width - (i+1)*nStep , 0 , (i+1)*nStep , height , hSrcDC , x , y , width , height , SRCCOPY );
		BitBlt( hDestDC , xd+width- (i+1)*nStep , yd , (i+1)*nStep , height , hBuffDC , width - (i+1)*nStep , 0 , SRCCOPY );
		SleepEx( nDelay );
	}
	BitBlt( hDestDC , xd , yd , width , height , hSrcDC , x , y , SRCCOPY );
	DeleteObject( hBitmap );
	DeleteDC( hBuffDC );
	return TRUE;
}

//---------------------------------------------------------------------
//top to bottom stretch show
//---------------------------------------------------------------------
bool MagicFun13( HDC hSrcDC , int x , int y , HDC hDestDC , int xd , int yd , int width , int height , int nDelay, int nStep )
{
	HDC hBuffDC = CreateCompatibleDC(hDestDC);
	HBITMAP hBitmap = CreateCompatibleBitmap( hDestDC, width , height );
	SelectObject( hBuffDC , hBitmap );
	
	BitBlt( hBuffDC , 0 , 0 , width , height , hDestDC , xd , yd , SRCCOPY );

	for( int i=0; i< height/nStep+1; i++ )
	{
		
		StretchBlt( hBuffDC , 0 , 0 , width , (i+1)*nStep , hSrcDC , x , y , width , height , SRCCOPY );
		
		BitBlt( hDestDC , xd , yd , width , (i+1)*nStep , hBuffDC , 0 ,0 , SRCCOPY );
		SleepEx( nDelay );
	}

	BitBlt( hDestDC , xd , yd , width , height , hSrcDC , x , y , SRCCOPY );
	DeleteObject( hBitmap );
	DeleteDC( hBuffDC );

	return TRUE;
}


//---------------------------------------------------------------------
//from bottom to top stretch show
//---------------------------------------------------------------------
bool MagicFun14( HDC hSrcDC , int x , int y , HDC hDestDC , int xd , int yd , int width , int height , int nDelay, int nStep )
{
	HDC hBuffDC = CreateCompatibleDC(hDestDC);
	HBITMAP hBitmap = CreateCompatibleBitmap( hDestDC, width , height );
	SelectObject( hBuffDC , hBitmap );
	
	BitBlt( hBuffDC , 0 , 0 , width , height , hDestDC , xd , yd , SRCCOPY );

	for( int i=0; i< height/nStep+1; i++ )
	{
		
		StretchBlt( hBuffDC , 0 , height-(i+1)*nStep , width , (i+1)*nStep , hSrcDC , x , y , width , height , SRCCOPY );
		
		BitBlt( hDestDC , xd , yd+height-(i+1)*nStep , width , (i+1)*nStep , hBuffDC , 0 ,height-(i+1)*nStep , SRCCOPY );
		SleepEx( nDelay );
	}

	BitBlt( hDestDC , xd , yd , width , height , hSrcDC , x , y , SRCCOPY );
	DeleteObject( hBitmap );
	DeleteDC( hBuffDC );
	return TRUE;
}


//---------------------------------------------------------------------
//from left to right
//---------------------------------------------------------------------
bool MagicFun15( HDC hSrcDC , int x , int y , HDC hDestDC , int xd , int yd , int width , int height , int nDelay, int nStep )
{
	for( int i=0; i< width/nStep; i++ )
	{
		BitBlt( hDestDC , xd , yd , (i+1)*nStep , height , hSrcDC , x+width-(i+1)*nStep , y , SRCCOPY );
		SleepEx( nDelay );
	}
	BitBlt( hDestDC , xd , yd , width , height , hSrcDC , x , y , SRCCOPY );
	return TRUE;
}

//---------------------------------------------------------------------
//from right to left
//---------------------------------------------------------------------
bool MagicFun16( HDC hSrcDC , int x , int y , HDC hDestDC , int xd , int yd , int width , int height , int nDelay, int nStep )
{
	for( int i=0; i< width/nStep; i++ )
	{
		BitBlt( hDestDC , xd+width-(i+1)*nStep , yd , (i+1)*nStep , height , hSrcDC , x , y , SRCCOPY );
		SleepEx( nDelay );
	}
	BitBlt( hDestDC , xd , yd , width , height , hSrcDC , x, y , SRCCOPY );
	return TRUE;
}

//---------------------------------------------------------------------
//from top to bottom
//---------------------------------------------------------------------
bool MagicFun17( HDC hSrcDC , int x , int y , HDC hDestDC , int xd , int yd , int width , int height , int nDelay, int nStep )
{
	for( int i=0; i< height/nStep; i++ )
	{
		BitBlt( hDestDC , xd, yd , width, (i+1)*nStep , hSrcDC , x , y+height-(i+1)*nStep , SRCCOPY );
		SleepEx( nDelay );
	}
	BitBlt( hDestDC , xd , yd , width , height , hSrcDC , x, y , SRCCOPY );
	return TRUE;
}

//---------------------------------------------------------------------
//from bottom to top
//---------------------------------------------------------------------
bool MagicFun18( HDC hSrcDC , int x , int y , HDC hDestDC , int xd , int yd , int width , int height , int nDelay, int nStep )
{
	for( int i=0; i< height/nStep; i++ )
	{
		BitBlt( hDestDC , xd, yd+height-(i+1)*nStep , width, (i+1)*nStep , hSrcDC , x , y , SRCCOPY );
		SleepEx( nDelay );
	}
	BitBlt( hDestDC , xd , yd , width , height , hSrcDC , x, y , SRCCOPY );
	return TRUE;
}

//----------------------------------------------------------------------
//from lefttop to rightbottom
//----------------------------------------------------------------------
bool MagicFun19( HDC hSrcDC , int x , int y , HDC hDestDC , int xd , int yd , int width , int height , int nDelay, int nStep)
{
	int nSize = nStep;
	int Max = width>height?width:height;

	while( nSize < Max )
	{
		int nX = x+width-nSize;
		int nY = y+height-nSize;
		if( nX < 0 ) nX = 0;
		if( nY < 0 ) nY = 0;

		BitBlt( hDestDC , xd , yd , nSize ,nSize ,hSrcDC,nX,nY,SRCCOPY);
		SleepEx(nDelay);
		nSize += nStep;
	}
	BitBlt( hDestDC , xd , yd , width , height ,hSrcDC,x,y,SRCCOPY);
	return TRUE;
}

//---------------------------------------------------
//from leftbottom to righttop
//-------------------------------------------------
bool MagicFun20( HDC hSrcDC , int x , int y , HDC hDestDC , int xd , int yd , int width , int height , int nDelay, int nStep)
{
	int nSize = nStep;
	int Max = width>height?width:height;

	while( nSize < Max )
	{
		int nX = x+width-nSize;
		int nY = nSize;
		if( nX < 0 ) nX = 0;
		if( nY > height ) nY = height;

		BitBlt( hDestDC , xd  , yd+height-nY , nSize ,nSize ,hSrcDC,nX,0,SRCCOPY);
		SleepEx(nDelay);
		nSize += nStep;
	}
	BitBlt( hDestDC , xd , yd , width , height ,hSrcDC,x,y,SRCCOPY);
	return TRUE;
}

