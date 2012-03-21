// Created on: 1995-07-17
// Created by: Modelistation
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
