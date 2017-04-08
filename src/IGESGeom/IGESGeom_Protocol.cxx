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


#include <IGESBasic.hxx>
#include <IGESBasic_Protocol.hxx>
#include <IGESGeom_Boundary.hxx>
#include <IGESGeom_BoundedSurface.hxx>
#include <IGESGeom_BSplineCurve.hxx>
#include <IGESGeom_BSplineSurface.hxx>
#include <IGESGeom_CircularArc.hxx>
#include <IGESGeom_CompositeCurve.hxx>
#include <IGESGeom_ConicArc.hxx>
#include <IGESGeom_CopiousData.hxx>
#include <IGESGeom_CurveOnSurface.hxx>
#include <IGESGeom_Direction.hxx>
#include <IGESGeom_Flash.hxx>
#include <IGESGeom_Line.hxx>
#include <IGESGeom_OffsetCurve.hxx>
#include <IGESGeom_OffsetSurface.hxx>
#include <IGESGeom_Plane.hxx>
#include <IGESGeom_Point.hxx>
#include <IGESGeom_Protocol.hxx>
#include <IGESGeom_RuledSurface.hxx>
#include <IGESGeom_SplineCurve.hxx>
#include <IGESGeom_SplineSurface.hxx>
#include <IGESGeom_SurfaceOfRevolution.hxx>
#include <IGESGeom_TabulatedCylinder.hxx>
#include <IGESGeom_TransformationMatrix.hxx>
#include <IGESGeom_TrimmedSurface.hxx>
#include <Interface_Protocol.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESGeom_Protocol,IGESData_Protocol)

static int THE_IGESGeom_Protocol_deja = 0;
static Handle(Standard_Type) atype01,atype02,atype03,atype04,atype05,atype06,
  atype07,atype08,atype09,atype10,atype11,atype12,atype13,atype14,atype15,
  atype16,atype17,atype18,atype19,atype20,atype21,atype22,atype23;

IGESGeom_Protocol::IGESGeom_Protocol()
{
  if (THE_IGESGeom_Protocol_deja)
  {
    return;
  }
  THE_IGESGeom_Protocol_deja = 1;
  atype01 = STANDARD_TYPE(IGESGeom_BSplineCurve);
  atype02 = STANDARD_TYPE(IGESGeom_BSplineSurface);
  atype03 = STANDARD_TYPE(IGESGeom_Boundary);
  atype04 = STANDARD_TYPE(IGESGeom_BoundedSurface);
  atype05 = STANDARD_TYPE(IGESGeom_CircularArc);
  atype06 = STANDARD_TYPE(IGESGeom_CompositeCurve);
  atype07 = STANDARD_TYPE(IGESGeom_ConicArc);
  atype08 = STANDARD_TYPE(IGESGeom_CopiousData);
  atype09 = STANDARD_TYPE(IGESGeom_CurveOnSurface);
  atype10 = STANDARD_TYPE(IGESGeom_Direction);
  atype11 = STANDARD_TYPE(IGESGeom_Flash);
  atype12 = STANDARD_TYPE(IGESGeom_Line);
  atype13 = STANDARD_TYPE(IGESGeom_OffsetCurve);
  atype14 = STANDARD_TYPE(IGESGeom_OffsetSurface);
  atype15 = STANDARD_TYPE(IGESGeom_Plane);
  atype16 = STANDARD_TYPE(IGESGeom_Point);
  atype17 = STANDARD_TYPE(IGESGeom_RuledSurface);
  atype18 = STANDARD_TYPE(IGESGeom_SplineCurve);
  atype19 = STANDARD_TYPE(IGESGeom_SplineSurface);
  atype20 = STANDARD_TYPE(IGESGeom_SurfaceOfRevolution);
  atype21 = STANDARD_TYPE(IGESGeom_TabulatedCylinder);
  atype22 = STANDARD_TYPE(IGESGeom_TransformationMatrix);
  atype23 = STANDARD_TYPE(IGESGeom_TrimmedSurface);
}

    Standard_Integer IGESGeom_Protocol::NbResources () const
      {  return 1;  }

    Handle(Interface_Protocol) IGESGeom_Protocol::Resource
  (const Standard_Integer /*num*/) const
{
  Handle(Interface_Protocol) res = IGESBasic::Protocol();;
  return res;
}

    Standard_Integer IGESGeom_Protocol::TypeNumber
  (const Handle(Standard_Type)& atype) const
{
  if      (atype == atype01) return  1;
  else if (atype == atype02) return  2;
  else if (atype == atype03) return  3;
  else if (atype == atype04) return  4;
  else if (atype == atype05) return  5;
  else if (atype == atype06) return  6;
  else if (atype == atype07) return  7;
  else if (atype == atype08) return  8;
  else if (atype == atype09) return  9;
  else if (atype == atype10) return 10;
  else if (atype == atype11) return 11;
  else if (atype == atype12) return 12;
  else if (atype == atype13) return 13;
  else if (atype == atype14) return 14;
  else if (atype == atype15) return 15;
  else if (atype == atype16) return 16;
  else if (atype == atype17) return 17;
  else if (atype == atype18 ) return 18;
  else if (atype == atype19) return 19;
  else if (atype == atype20) return 20;
  else if (atype == atype21) return 21;
  else if (atype == atype22) return 22;
  else if (atype == atype23) return 23;
  return 0;
}
