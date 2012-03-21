// Copyright (c) 1991-1999 Matra Datavision
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

/*
File InterfaceGraphic_tgl_all.h

16/06/2000 : ATS : G005 : Copied from OpenGl_tgl_all.h to support required
for InterfaceGraphic_Parray.hxx definitions
23-12-02 : SAV ; Added Tuint type
*/

#ifndef  INTERFACEGRAPHIC_TGL_ALL_H
#define  INTERFACEGRAPHIC_TGL_ALL_H

typedef  int          Tint;
typedef  float        Tfloat;
/* PCD 04/07/07  */
typedef  double       Tdouble;
/* Tchar is treated as a signed char in visualization code,
therefore it should be made signed explicitly, as on Linux 
-funsigned-char option is specified when building OCCT */
typedef  signed char  Tchar;
typedef  char         Tbool;
typedef  unsigned int Tuint;
/* szv: Techar is an Extended character */
typedef  unsigned short Techar;

#define  TGL_SP   1
#define  TGL_DP   0

#if TGL_DP
#define v3f v3d
#define c3f c3d
#define n3f n3d
#endif

typedef  enum  
{ 
  TFailure = -1, 
  TSuccess 
} TStatus;

#define  TOn   1
#define  TOff  0

#define  TDone     1
#define  TNotDone  0

typedef  Tfloat  Tmatrix3[4][4];

typedef  enum    
{ 
  TPreConcatenate, 
  TPostConcatenate, 
  TReplace 
} TComposeType;

#endif /* INTERFACEGRAPHIC_TGL_ALL_H */
