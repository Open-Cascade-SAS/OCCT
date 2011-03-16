// File:	HLRBRep_CurveTool.cxx
// Created:	Mon Jul 17 17:39:39 1995
// Author:	Modelistation
//		<model@mastox>

#include <HLRBRep_CurveTool.ixx>
#include <HLRBRep_Curve.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomAbs_Shape.hxx>
#include <Handle_Geom2d_BezierCurve.hxx>
#include <Handle_Geom2d_BSplineCurve.hxx>
#include <TColStd_Array1OfReal.hxx>

//=======================================================================
//function : NbSamples
//purpose  : 
//=======================================================================

Standard_Integer
HLRBRep_CurveTool::NbSamples (const Standard_Address C)
{ 
  GeomAbs_CurveType typC = ((HLRBRep_Curve *)C)->GetType();
  static Standard_Real nbsOther = 10.0;
  Standard_Real nbs = nbsOther;
  
  if(typC == GeomAbs_Line) 
    nbs = 2;
  else if(typC == GeomAbs_BezierCurve) 
    nbs = 3 + ((HLRBRep_Curve *)C)->NbPoles();
  else if(typC == GeomAbs_BSplineCurve) { 
    nbs = ((HLRBRep_Curve *)C)->NbKnots();
    nbs*= ((HLRBRep_Curve *)C)->Degree();
    if(nbs < 2.0) nbs=2;
  }
  if(nbs>50)
    nbs = 50;
  return((Standard_Integer)nbs);
}

//=======================================================================
//function : NbSamples
//purpose  : 
//=======================================================================

Standard_Integer
HLRBRep_CurveTool::NbSamples (const Standard_Address C,
			      const Standard_Real u1,
			      const Standard_Real u2) 
{ 
  GeomAbs_CurveType typC = ((HLRBRep_Curve *)C)->GetType();
  static Standard_Real nbsOther = 10.0;
  Standard_Real nbs = nbsOther;
  
  if(typC == GeomAbs_Line) 
    nbs = 2;
  else if(typC == GeomAbs_BezierCurve) 
    nbs = 3 + ((HLRBRep_Curve *)C)->NbPoles();
  else if(typC == GeomAbs_BSplineCurve) { 
    nbs = ((HLRBRep_Curve *)C)->NbKnots();
    nbs*= ((HLRBRep_Curve *)C)->Degree();
    if(nbs < 2.0) nbs=2;
  }
  if(nbs>50)
    nbs = 50;
  return((Standard_Integer)nbs);
}

