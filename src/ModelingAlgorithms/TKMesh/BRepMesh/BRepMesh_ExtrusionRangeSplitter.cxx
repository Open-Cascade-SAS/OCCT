// Created on: 2022-09-07
// Copyright (c) 2022 OPEN CASCADE SAS
// Created by: Oleg AGASHIN
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

#include <BRepMesh_ExtrusionRangeSplitter.hxx>

//=================================================================================================

Standard_Integer BRepMesh_ExtrusionRangeSplitter::getUndefinedIntervalNb(
  const Handle(Adaptor3d_Surface)& theSurface,
  const Standard_Boolean /*isU*/,
  const GeomAbs_Shape theContinuity) const
{
  // Here we need just a regular grid along dimension with no
  // geometrical data regarding intervals like extrusion surface.
  const Handle(Adaptor3d_Curve) aCurve       = theSurface->BasisCurve();
  Standard_Integer              aIntervalsNb = aCurve->NbIntervals(theContinuity);
  if (aIntervalsNb == 1)
  {
    const GeomAbs_CurveType aCurveType = aCurve->GetType();
    const Standard_Boolean  isBSplineCurve =
      aCurveType == GeomAbs_BezierCurve || aCurveType == GeomAbs_BSplineCurve;

    if (isBSplineCurve)
    {
      aIntervalsNb = aCurve->NbPoles() - 1;
    }
  }

  return aIntervalsNb;
}
