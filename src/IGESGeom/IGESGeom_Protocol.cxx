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

#include <IGESGeom_Protocol.hxx>

#include <IGESBasic.hxx>
#include <IGESBasic_Protocol.hxx>
#include <IGESGeom_BSplineCurve.hxx>
#include <IGESGeom_BSplineSurface.hxx>
#include <IGESGeom_BoundedSurface.hxx>
#include <IGESGeom_CircularArc.hxx>
#include <IGESGeom_CompositeCurve.hxx>
#include <IGESGeom_ConicArc.hxx>
#include <IGESGeom_CopiousData.hxx>
#include <IGESGeom_Direction.hxx>
#include <IGESGeom_Flash.hxx>
#include <IGESGeom_Line.hxx>
#include <IGESGeom_OffsetCurve.hxx>
#include <IGESGeom_OffsetSurface.hxx>
#include <IGESGeom_Plane.hxx>
#include <IGESGeom_Point.hxx>
#include <IGESGeom_RuledSurface.hxx>
#include <IGESGeom_SplineCurve.hxx>
#include <IGESGeom_SplineSurface.hxx>
#include <IGESGeom_SurfaceOfRevolution.hxx>
#include <IGESGeom_TabulatedCylinder.hxx>
#include <IGESGeom_TransformationMatrix.hxx>
#include <IGESGeom_TrimmedSurface.hxx>
#include <Interface_Protocol.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESGeom_Protocol, IGESData_Protocol)

IGESGeom_Protocol::IGESGeom_Protocol() {}

Standard_Integer IGESGeom_Protocol::NbResources() const
{
  return 1;
}

Handle(Interface_Protocol) IGESGeom_Protocol::Resource(const Standard_Integer /*num*/) const
{
  Handle(Interface_Protocol) res = IGESBasic::Protocol();
  return res;
}

Standard_Integer IGESGeom_Protocol::TypeNumber(const Handle(Standard_Type)& atype) const
{
  if (atype == STANDARD_TYPE(IGESGeom_BSplineCurve))
    return 1;
  else if (atype == STANDARD_TYPE(IGESGeom_BSplineSurface))
    return 2;
  else if (atype == STANDARD_TYPE(IGESGeom_Boundary))
    return 3;
  else if (atype == STANDARD_TYPE(IGESGeom_BoundedSurface))
    return 4;
  else if (atype == STANDARD_TYPE(IGESGeom_CircularArc))
    return 5;
  else if (atype == STANDARD_TYPE(IGESGeom_CompositeCurve))
    return 6;
  else if (atype == STANDARD_TYPE(IGESGeom_ConicArc))
    return 7;
  else if (atype == STANDARD_TYPE(IGESGeom_CopiousData))
    return 8;
  else if (atype == STANDARD_TYPE(IGESGeom_CurveOnSurface))
    return 9;
  else if (atype == STANDARD_TYPE(IGESGeom_Direction))
    return 10;
  else if (atype == STANDARD_TYPE(IGESGeom_Flash))
    return 11;
  else if (atype == STANDARD_TYPE(IGESGeom_Line))
    return 12;
  else if (atype == STANDARD_TYPE(IGESGeom_OffsetCurve))
    return 13;
  else if (atype == STANDARD_TYPE(IGESGeom_OffsetSurface))
    return 14;
  else if (atype == STANDARD_TYPE(IGESGeom_Plane))
    return 15;
  else if (atype == STANDARD_TYPE(IGESGeom_Point))
    return 16;
  else if (atype == STANDARD_TYPE(IGESGeom_RuledSurface))
    return 17;
  else if (atype == STANDARD_TYPE(IGESGeom_SplineCurve))
    return 18;
  else if (atype == STANDARD_TYPE(IGESGeom_SplineSurface))
    return 19;
  else if (atype == STANDARD_TYPE(IGESGeom_SurfaceOfRevolution))
    return 20;
  else if (atype == STANDARD_TYPE(IGESGeom_TabulatedCylinder))
    return 21;
  else if (atype == STANDARD_TYPE(IGESGeom_TransformationMatrix))
    return 22;
  else if (atype == STANDARD_TYPE(IGESGeom_TrimmedSurface))
    return 23;
  return 0;
}
