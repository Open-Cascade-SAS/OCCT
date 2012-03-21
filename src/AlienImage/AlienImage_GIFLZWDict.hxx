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
//==== Titre: AlienImage_GIFLZWDict.hxx
//============================================================================

#ifndef _AlienImage_GIFLZWDict_HeaderFile
#define _AlienImage_GIFLZWDict_HeaderFile

//==== Definition de Type ====================================================
#include <Standard_Type.hxx>
const Handle(Standard_Type)& STANDARD_TYPE(AlienImage_GIFLZWDict);

//============================================================================
#include <OSD_File.hxx>

#ifndef WNT
typedef unsigned int   DWORD; // 32-bit signed
typedef int            LONG;  // 32-bit unsigned
typedef unsigned int   ULONG, UINT; // 32-bit signed
typedef unsigned short WORD;  // 16-bit unsigned
typedef unsigned char  BYTE;  // 8-bit unsigned
typedef unsigned int   BOOL;
typedef int*           PINT;
typedef unsigned char* PBYTE;
typedef void*          LPVOID;
#else
#ifdef NOGDI
#undef NOGDI /* we need GDI definitions here... */
#endif
# include <windows.h>
#endif

#if defined(__alpha)  || defined(DECOSF1) || defined(WNT)
#  define SWAP_WORD(__w)  (__w)
#  define SWAP_DWORD(__w)  (__w)
#else
#  define SWAP_WORD(__w) ((((__w)&0xFF) << 8) | (((__w)&0xFF00) >> 8))
#  define SWAP_DWORD(__w) ((((__w)&0x000000FF) << 24) | (((__w)&0x0000FF00) << 8) \
         |  (((__w)&0xFF0000) >> 8)  |  (((__w)&0xFF000000) >> 24)  )
#endif

#define NBITS     12
#define TBL_SIZE  5021
#define MAX_CODE  (  ( 1 << NBITS ) - 1  )
#define BUFF_SIZE 255
#define UNUSED -1
#define TRUE 1
#define FALSE 0

//=============================================================================
// Functions and structs to write GIF image to file.
//=============================================================================
extern int _lzw_encode (OSD_File&, const BYTE*, int, int, int);

typedef struct {
  char gifID[ 6 ];
  WORD scrnWidth;
  WORD scrnHeight;
  BYTE scrnFlag;
} SCREEN_DESCR;

typedef struct {
  WORD imgX;
  WORD imgY;
  WORD imgWidth;
  WORD imgHeight;
  BYTE imgFlag;
} IMAGE_DESCR;

typedef struct {
  int code;
  int prnt;
  BYTE byte;
} AlienImage_GIFLZWDict;

//=============================================================================
// Functions to convert from 24 to 8 color image
//=============================================================================
#define PAD(a)     (   ( a ) % sizeof ( LONG ) ?                         \
                    sizeof ( LONG ) - (  ( a ) % sizeof ( LONG )  ) : 0  )
#define MALLOC(s) calloc (  ( s ), 1  )
#define FREE(p)   free   (  ( p )     )

#ifndef WNT
typedef struct {
  BYTE rgbBlue;
  BYTE rgbGreen;
  BYTE rgbRed;
  BYTE rgbReserved;
} RGBQUAD, *LPRGBQUAD;

#define __fastcall
#endif // WNT

BOOL __fastcall _convert24to8 (LPRGBQUAD, PBYTE, PBYTE, int, int);

#endif // _AlienImage_GIFLZWDict_HeaderFile
 
