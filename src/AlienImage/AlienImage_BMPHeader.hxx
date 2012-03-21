// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

//============================================================================
//==== Titre: AlienImage_BMPHeader.hxx
//============================================================================

#ifndef _AlienImage_BMPHeader_HeaderFile
#define _AlienImage_BMPHeader_HeaderFile

//==== Definition de Type ====================================================
#include <Standard_Type.hxx>

#ifdef WNT
# include <windows.h>
#else
typedef unsigned int   DWORD; // 32-bit signed
typedef unsigned int*  PDWORD;
typedef int            LONG;  // 32-bit unsigned
typedef unsigned short WORD;  // 16-bit unsigned
typedef unsigned char  BYTE;  //  8-bit unsigned
#endif // WNT

typedef struct {
//  WORD    bfType;
  DWORD   bfSize;
  DWORD   bfReserved;
  DWORD   bfOffBits;
} AlienImage_BMPHeader;

const Handle(Standard_Type)& STANDARD_TYPE(AlienImage_BMPHeader);

//============================================================================
#ifndef WNT

typedef struct tagBITMAPINFOHEADER {
   DWORD  biSize; 
   LONG   biWidth; 
   LONG   biHeight; 
   WORD   biPlanes; 
   WORD   biBitCount; 
   DWORD  biCompression; 
   DWORD  biSizeImage; 
   LONG   biXPelsPerMeter; 
   LONG   biYPelsPerMeter; 
   DWORD  biClrUsed; 
   DWORD  biClrImportant; 
} BITMAPINFOHEADER; 

typedef struct tagBITMAPCOREHEADER {
   DWORD   bcSize;
   WORD    bcWidth;
   WORD    bcHeight;
   WORD    bcPlanes;   // 1
   WORD    bcBitCount; // 1,4,8 or 24
} BITMAPCOREHEADER;

// constants for the biCompression field
#define BI_RGB        0
#define BI_RLE8       1
#define BI_RLE4       2
#define BI_BITFIELDS  3

typedef struct tagRGBQUAD {
    BYTE    rgbBlue;
    BYTE    rgbGreen;
    BYTE    rgbRed;
    BYTE    rgbReserved;
} RGBQUAD;

#endif // WNT

#define LOW_VAL_AT_LOW_ADDR (*(char*)&__swaptest)

#endif // _AlienImage_BMPHeader_HeaderFile
 
