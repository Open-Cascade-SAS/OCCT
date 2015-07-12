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


#include <BRepAdaptor_Curve.hxx>
#include <BRepGProp_EdgeTool.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Standard_OutOfRange.hxx>

Standard_Real  BRepGProp_EdgeTool::FirstParameter(const BRepAdaptor_Curve& C)
{
  return C.FirstParameter();
}

Standard_Real  BRepGProp_EdgeTool::LastParameter(const BRepAdaptor_Curve& C)
{
  return C.LastParameter();
}

Standard_Integer  BRepGProp_EdgeTool::IntegrationOrder(const BRepAdaptor_Curve& BAC)
{
  switch (BAC.GetType()) {

  case GeomAbs_Line :
    return 2;

  case GeomAbs_Parabola :
    return 5;

  case GeomAbs_BezierCurve :
    {
    const GeomAdaptor_Curve& GAC = BAC.Curve();
    const Handle(Geom_Curve)& GC = GAC.Curve();
    Handle(Geom_BezierCurve) GBZC (Handle(Geom_BezierCurve)::DownCast (GC));
    Standard_Integer n = 2*(GBZC->NbPoles()) - 1;
    return n; 
    }
    break;
  case GeomAbs_BSplineCurve :
    {
    const GeomAdaptor_Curve& GAC = BAC.Curve();
    const Handle(Geom_Curve)& GC = GAC.Curve();
    Handle(Geom_BSplineCurve) GBSC (Handle(Geom_BSplineCurve)::DownCast (GC));
    Standard_Integer n = 2*(GBSC->NbPoles()) - 1;
    return n; 
    }
    break;
    
    default :
      return 10;
  }
}

gp_Pnt  BRepGProp_EdgeTool::Value(const BRepAdaptor_Curve& C, const Standard_Real U)
{
  return C.Value(U);
}

void  BRepGProp_EdgeTool::D1(const BRepAdaptor_Curve& C, 
	 const Standard_Real U, gp_Pnt& P, gp_Vec& V1)
{
  C.D1(U,P,V1);
}

// modified by NIZHNY-MKK  Thu Jun  9 12:15:15 2005.BEGIN
Standard_Integer BRepGProp_EdgeTool::NbIntervals(const BRepAdaptor_Curve& C,const GeomAbs_Shape S) 
{
  BRepAdaptor_Curve* pC = (BRepAdaptor_Curve*) &C; // at the moment actually NbIntervals() does not modify the 
                                                   // object "C". So it is safe to do such a cast.
  return pC->NbIntervals(S);
}

void BRepGProp_EdgeTool::Intervals(const BRepAdaptor_Curve& C,TColStd_Array1OfReal& T,const GeomAbs_Shape S) 
{
  BRepAdaptor_Curve* pC = (BRepAdaptor_Curve*) &C; // at the moment actually Intervals() does not modify the
                                                   // object "C". So it is safe to do such a cast.
  pC->Intervals(T, S);
}
// modified by NIZHNY-MKK  Thu Jun  9 12:15:18 2005.END
