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

#include <IntPatch_Line.ixx>


IntPatch_Line::IntPatch_Line (const Standard_Boolean Tang,
			      const IntSurf_TypeTrans Trans1,
			      const IntSurf_TypeTrans Trans2):
       tg(Tang),
       tS1(Trans1),tS2(Trans2),
       sit1(IntSurf_Unknown),sit2(IntSurf_Unknown),
       uS1(Standard_False),vS1(Standard_False),
       uS2(Standard_False),vS2(Standard_False)
{}

IntPatch_Line::IntPatch_Line (const Standard_Boolean Tang,
			      const IntSurf_Situation Situ1,
                              const IntSurf_Situation Situ2):
       tg(Tang),
       tS1(IntSurf_Touch),tS2(IntSurf_Touch),
       sit1(Situ1),sit2(Situ2),
       uS1(Standard_False),vS1(Standard_False),
       uS2(Standard_False),vS2(Standard_False)
{}

IntPatch_Line::IntPatch_Line (const Standard_Boolean Tang):
       tg(Tang),
       tS1(IntSurf_Undecided),tS2(IntSurf_Undecided),
       sit1(IntSurf_Unknown),sit2(IntSurf_Unknown),
       uS1(Standard_False),vS1(Standard_False),
       uS2(Standard_False),vS2(Standard_False)
{}


