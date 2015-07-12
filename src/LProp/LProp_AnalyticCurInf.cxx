// Created on: 1994-09-05
// Created by: Yves FRICAUD
// Copyright (c) 1994-1999 Matra Datavision
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


#include <ElCLib.hxx>
#include <LProp_AnalyticCurInf.hxx>
#include <LProp_CurAndInf.hxx>

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


