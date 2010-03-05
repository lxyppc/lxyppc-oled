#ifndef   FONT_H
#define   FONT_H

typedef struct _FontData
{
    unsigned char width;
    unsigned char height;
    unsigned char data[32];
}FontData;

const FontData* GetFont_fixedsys(unsigned char ch);

extern const FontData fontBuffer_fixedsys[128];

#endif
