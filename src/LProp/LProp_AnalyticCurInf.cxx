// File:	LProp_AnalyticCurInf.cxx
// Created:	Mon Sep  5 10:31:35 1994
// Author:	Yves FRICAUD
//		<yfr@ecolox>


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
      Standard_Real UFPlus2PI = UFirst + 2*PI;
      
      U1 = ElCLib::InPeriod(0.0     ,UFirst,UFPlus2PI);
      U2 = ElCLib::InPeriod(PI/2.   ,UFirst,UFPlus2PI);
      U3 = ElCLib::InPeriod(PI      ,UFirst,UFPlus2PI);
      U4 = ElCLib::InPeriod(3.*PI/2.,UFirst,UFPlus2PI);
      
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
#ifndef DEB
  default:
    break; 
#endif
  }
}


