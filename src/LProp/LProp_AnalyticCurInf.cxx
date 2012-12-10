// Created on: 1994-09-05
// Created by: Yves FRICAUD
// Copyright (c) 1994-1999 Matra Datavision
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



#include <LProp_AnalyticCurInf.ixx>
#include <ElCLib.hxx>

//=======================================================================
//function : LProp_AnalyticCurInf
//purpose  : 
//=======================================================================

LProp_AnalyticCurInf::LProp_AnalyticCurInf()
{
}


//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void LProp_AnalyticCurInf::Perform (const GeomAbs_CurveType  CType,
				    const Standard_Real      UFirst,
				    const Standard_Real      ULast,
				    LProp_CurAndInf&         Result)
{
  Standard_Boolean IsMin = Standard_True;
  Standard_Boolean IsMax = Standard_False;

  switch (CType) {
  
  case GeomAbs_Ellipse: 
    {
      Standard_Real U1,U2,U3,U4;
      Standard_Real UFPlus2PI = UFirst + 2*M_PI;
      
      U1 = ElCLib::InPeriod(0.0     ,UFirst,UFPlus2PI);
      U2 = ElCLib::InPeriod(M_PI/2.   ,UFirst,UFPlus2PI);
      U3 = ElCLib::InPeriod(M_PI      ,UFirst,UFPlus2PI);
      U4 = ElCLib::InPeriod(3.*M_PI/2.,UFirst,UFPlus2PI);
      
      if (UFirst <= U1 && U1 <= ULast) {Result.AddExtCur(U1, IsMin);}
      if (UFirst <= U2 && U2 <= ULast) {Result.AddExtCur(U2, IsMax);}
      if (UFirst <= U3 && U3 <= ULast) {Result.AddExtCur(U3, IsMin);}
      if (UFirst <= U4 && U4 <= ULast) {Result.AddExtCur(U4, IsMax);}
    }
    break;
    
  case GeomAbs_Hyperbola:
    if (UFirst <= 0.0 && ULast >= 0.0) {
      Result.AddExtCur(0.0   , Standard_True);
    }
    break;
  
  case GeomAbs_Parabola:     
    if (UFirst <= 0.0 && ULast >= 0.0) {
      Result.AddExtCur(0.0   , Standard_True);
    }
    break; 
  default:
    break; 
  }
}


