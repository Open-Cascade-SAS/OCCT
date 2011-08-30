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
