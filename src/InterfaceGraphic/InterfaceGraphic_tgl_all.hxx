// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef  INTERFACEGRAPHIC_TGL_ALL_H
#define  INTERFACEGRAPHIC_TGL_ALL_H

typedef  int          Tint;
typedef  float        Tfloat;
typedef  double       Tdouble;
/* Tchar is treated as a signed char in visualization code,
therefore it should be made signed explicitly, as on Linux 
-funsigned-char option is specified when building OCCT */
typedef  signed char  Tchar;
typedef  char         Tbool;
typedef  unsigned int Tuint;
/* szv: Techar is an Extended character */
typedef  unsigned short Techar;

#define  TOn   1
#define  TOff  0

typedef  Tfloat  Tmatrix3[4][4];

#endif /* INTERFACEGRAPHIC_TGL_ALL_H */
