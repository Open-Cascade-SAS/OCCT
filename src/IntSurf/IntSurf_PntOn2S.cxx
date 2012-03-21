// Copyright (c) 1995-1999 Matra Datavision
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



