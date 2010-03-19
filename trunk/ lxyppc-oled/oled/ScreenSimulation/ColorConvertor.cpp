#include "StdAfx.h"
#include "ColorConvertor.h"

/**  Private functions  */
BOOL ConvertImageRGB888(
    void* pDes,
    const void* pSrc,
    LONG width,
    LONG height,
    LONG scale,
    ColorFormat srcColorFormat);

void ConvertImageGray12RGB888(
    DWORD* pDes,
    const BYTE* pSrc,
    LONG width,
    LONG height,
    LONG scale);

void ConvertImageGray82RGB888(
    DWORD* pDes,
    const BYTE* pSrc,
    LONG width,
    LONG height,
    LONG scale);

// Return value is the converted image buffer
BOOL ConvertImage(
    void* pDes,
    const void* pSrc,
    LONG desSize,
    LONG width,
    LONG height,
    LONG scale,
    ColorFormat desColotFormat,
    ColorFormat srcColorFormat)
{
    if(pDes && pSrc && scale>0 && scale<16 && width>0 && height>0){
        switch(desColotFormat){
            case CF_RGB888:
                if(scale*width*height*scale*4 > desSize)return FALSE;
                return ConvertImageRGB888(pDes,pSrc,width,height,scale,srcColorFormat);
            case CF_GRAY8:
                //return ConvertImageGray8(pDes,pSrc,width,height,scale,srcColorFormat);
            default:
                return FALSE;
        }
    }
    return FALSE;
}


BOOL ConvertImageRGB888(
    void* pDes,
    const void* pSrc,
    LONG width,
    LONG height,
    LONG scale,
    ColorFormat srcColorFormat)
{
    memset(pDes,0,width*height*scale*scale);
    switch(srcColorFormat){
        case CF_GRAY1:
            ConvertImageGray12RGB888((DWORD*)pDes,(const BYTE*)pSrc,width,height,scale);
            break;
        case CF_GRAY8:
            ConvertImageGray82RGB888((DWORD*)pDes,(const BYTE*)pSrc,width,height,scale);
            break;
    }
    return TRUE;
}

void ConvertImageGray12RGB888(
    DWORD* pDes,
    const BYTE* pSrc,
    LONG width,
    LONG height,
    LONG scale)
{
    _ASSERT(pDes && pSrc);
    _ASSERT( (width&0x7) == 0);
    LONG cx = width*scale;
    LONG cy = height*scale;
    DWORD factor = 0x00FFFFFF;
    for(LONG row=0; row<cy;row++){
        for(LONG col=0;col<cx;col++){
            LONG x = col/scale;
            LONG y = row/scale;
            pDes[col+row*cx] = factor*  ((pSrc[x/8 + y* (width/8)] & (1<<(x%8))) ? 1 : 0);
        }
    }
}


void ConvertImageGray82RGB888(
    DWORD* pDes,
    const BYTE* pSrc,
    LONG width,
    LONG height,
    LONG scale)
{
    LONG cx = width*scale;
    LONG cy = height*scale;
    DWORD factor = 0x00010101;
    for(LONG row=0; row<cy;row++){
        for(LONG col=0;col<cx;col++){
            LONG x = col/scale;
            LONG y = row/scale;
            pDes[col+row*cx] = factor*pSrc[x + y* width];
        }
    }
}