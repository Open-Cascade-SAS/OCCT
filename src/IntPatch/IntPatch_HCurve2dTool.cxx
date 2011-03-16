// File      : IntPatch_HCurve2dTool.cxx
// Created   : Mon Jul 17 17:39:39 1995
// Author    : Modelistation
// Copyright : OPEN CASCADE 1995

#include <IntPatch_HCurve2dTool.ixx>

#include <GeomAbs_CurveType.hxx>
#include <GeomAbs_Shape.hxx>
#include <Handle_Geom2d_BezierCurve.hxx>
#include <Handle_Geom2d_BSplineCurve.hxx>

#include <TColStd_Array1OfReal.hxx>


//============================================================
Standard_Integer IntPatch_HCurve2dTool::NbSamples (const Handle(Adaptor2d_HCurve2d)& C,
                                                   const Standard_Real U0,
                                                   const Standard_Real U1)
{
  Standard_Real nbs;
  switch (C->GetType())
  {
    case GeomAbs_Line: return 2;
    case GeomAbs_BezierCurve:
    {
      nbs = (3 + C->NbPoles());
    }
	break;
    case GeomAbs_BSplineCurve:
    {
      nbs = C->NbKnots();
      nbs *= C->Degree();
      //szv:nbs *= C->LastParameter() - C->FirstParameter();
      //szv:nbs /= U1-U0;
      nbs *= U1-U0;
      nbs /= C->LastParameter() - C->FirstParameter();
      if (nbs < 2.0) nbs = 2.0;
    }
	break;
	default: return 25;
  }
  if (nbs>50.0)
    return 50;
  return((Standard_Integer)nbs);
}
