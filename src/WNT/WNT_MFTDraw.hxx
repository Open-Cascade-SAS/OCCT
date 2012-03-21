// Created by: CHABROVSKY Dmitry
// Copyright (c) 1998-1999 Matra Datavision
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
