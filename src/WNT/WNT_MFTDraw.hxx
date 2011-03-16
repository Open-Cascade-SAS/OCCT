// File:        WNT_MFTDraw.hxx
// Created:     March 1998
// Author:      CHABROVSKY Dmitry
// Copyright:   Matra Datavision 1998

#ifdef  MFT

#ifndef _WNT_MFT_DRAW_HEADER_
#define _WNT_MFT_DRAW_HEADER_

#define   MAXCHARS    2048
#include "W32_Allocator.hxx"

typedef struct _MFT_DRAWTEXT {
  W32_FCALLPARAM theParam;
  double         theP2U;
  float          theXPos;
  float          theYPos;
  float          theAngle;
  double         theMargin;
  int            theTextType;
  BOOL           isTextPoly;
  BOOL           isTextWide;
  int            theTextLength;
  short          theText[MAXCHARS];
} MFT_DRAWTEXT, *PMFT_DRAWTEXT;

typedef struct _MFT_TEXTMAN_DATA {
  HDC           theHDC;
  BOOL          theWin95;
  int           theDevWidth;
  int           theDevHeight;
  int           theUWidth;
  BOOL          theMonoBuffer;
} MFT_TEXTMAN_DATA, *PMFT_TEXTMAN_DATA;

extern void __cdecl _Do_MFTDrawText_ (PW32_FCALLPARAM lpParam);

#endif // _WNT_MFT_DRAW_HEADER_

#endif // MFT
