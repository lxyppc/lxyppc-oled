#pragma once


typedef enum
{
    CF_RGB888,
    CF_RGB565,
    CF_GRAY1,
    CF_GRAY8,
    CF_YCrCb,
}ColorFormat;

// Return value is the converted image buffer
BOOL ConvertImage(
    void* pDes,
    const void* pSrc,
    LONG desSize,
    LONG width,
    LONG height,
    LONG scale,
    ColorFormat desColotFormat,
    ColorFormat srcColorFormat);
