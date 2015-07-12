// Created on: 1995-07-17
// Created by: Modelistation
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


#include <Adaptor2d_HCurve2d.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomAbs_Shape.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <IntPatch_HCurve2dTool.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_OutOfRange.hxx>
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
