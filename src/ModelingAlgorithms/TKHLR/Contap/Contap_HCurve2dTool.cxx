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

#include <Contap_HCurve2dTool.hxx>
#include <Geom2d_BezierCurve.hxx>

//============================================================
Standard_Integer Contap_HCurve2dTool::NbSamples(const Handle(Adaptor2d_Curve2d)& C,
                                                const Standard_Real              U0,
                                                const Standard_Real              U1)
{
  Standard_Real nbs = 10.0;
  switch (C->GetType())
  {
    case GeomAbs_Line:
      nbs = 2.;
      break;
    case GeomAbs_BezierCurve:
      nbs = 3. + C->NbPoles();
      break;
    case GeomAbs_BSplineCurve:
      nbs = C->NbKnots();
      nbs *= C->Degree();
      nbs *= C->LastParameter() - C->FirstParameter();
      nbs /= U1 - U0;
      if (nbs < 2.0)
        nbs = 2.;
      break;
    default:
      break;
  }
  if (nbs > 50.)
    nbs = 50.;
  return ((Standard_Integer)nbs);
}
