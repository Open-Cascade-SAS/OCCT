// Copyright (c) 1995-1999 Matra Datavision
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

#include <IntSurf_PntOn2S.ixx>


IntSurf_PntOn2S::IntSurf_PntOn2S () : pt(0,0,0),u1(0),v1(0),u2(0),v2(0) {};

void IntSurf_PntOn2S::SetValue (const gp_Pnt& Pt,
				const Standard_Boolean OnFirst,
				const Standard_Real U,
				const Standard_Real V) {

  pt = Pt;
  if (OnFirst) {
    u1 = U;
    v1 = V;
  }
  else {
    u2 = U;
    v2 = V;
  }
}


void IntSurf_PntOn2S::SetValue (const Standard_Boolean OnFirst,
				const Standard_Real U,
				const Standard_Real V) {

  if (OnFirst) {
    u1 = U;
    v1 = V;
  }
  else {
    u2 = U;
    v2 = V;
  }
}



