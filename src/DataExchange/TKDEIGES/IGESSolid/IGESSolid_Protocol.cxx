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

#include <IGESSolid_Protocol.hxx>

#include <IGESGeom.hxx>
#include <IGESGeom_Protocol.hxx>
#include <IGESSolid_Block.hxx>
#include <IGESSolid_BooleanTree.hxx>
#include <IGESSolid_ConeFrustum.hxx>
#include <IGESSolid_ConicalSurface.hxx>
#include <IGESSolid_Cylinder.hxx>
#include <IGESSolid_CylindricalSurface.hxx>
#include <IGESSolid_EdgeList.hxx>
#include <IGESSolid_Ellipsoid.hxx>
#include <IGESSolid_Loop.hxx>
#include <IGESSolid_ManifoldSolid.hxx>
#include <IGESSolid_PlaneSurface.hxx>
#include <IGESSolid_RightAngularWedge.hxx>
#include <IGESSolid_SelectedComponent.hxx>
#include <IGESSolid_Shell.hxx>
#include <IGESSolid_SolidAssembly.hxx>
#include <IGESSolid_SolidInstance.hxx>
#include <IGESSolid_SolidOfLinearExtrusion.hxx>
#include <IGESSolid_SolidOfRevolution.hxx>
#include <IGESSolid_Sphere.hxx>
#include <IGESSolid_SphericalSurface.hxx>
#include <IGESSolid_ToroidalSurface.hxx>
#include <IGESSolid_Torus.hxx>
#include <IGESSolid_VertexList.hxx>
#include <Interface_Protocol.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESSolid_Protocol, IGESData_Protocol)

IGESSolid_Protocol::IGESSolid_Protocol() {}

Standard_Integer IGESSolid_Protocol::NbResources() const
{
  return 1;
}

Handle(Interface_Protocol) IGESSolid_Protocol::Resource(const Standard_Integer /*num*/) const
{
  Handle(Interface_Protocol) res = IGESGeom::Protocol();
  return res;
}

Standard_Integer IGESSolid_Protocol::TypeNumber(const Handle(Standard_Type)& atype) const
{
  if (atype == STANDARD_TYPE(IGESSolid_Block))
    return 1;
  else if (atype == STANDARD_TYPE(IGESSolid_BooleanTree))
    return 2;
  else if (atype == STANDARD_TYPE(IGESSolid_ConeFrustum))
    return 3;
  else if (atype == STANDARD_TYPE(IGESSolid_ConicalSurface))
    return 4;
  else if (atype == STANDARD_TYPE(IGESSolid_Cylinder))
    return 5;
  else if (atype == STANDARD_TYPE(IGESSolid_CylindricalSurface))
    return 6;
  else if (atype == STANDARD_TYPE(IGESSolid_EdgeList))
    return 7;
  else if (atype == STANDARD_TYPE(IGESSolid_Ellipsoid))
    return 8;
  else if (atype == STANDARD_TYPE(IGESSolid_Face))
    return 9;
  else if (atype == STANDARD_TYPE(IGESSolid_Loop))
    return 10;
  else if (atype == STANDARD_TYPE(IGESSolid_ManifoldSolid))
    return 11;
  else if (atype == STANDARD_TYPE(IGESSolid_PlaneSurface))
    return 12;
  else if (atype == STANDARD_TYPE(IGESSolid_RightAngularWedge))
    return 13;
  else if (atype == STANDARD_TYPE(IGESSolid_SelectedComponent))
    return 14;
  else if (atype == STANDARD_TYPE(IGESSolid_Shell))
    return 15;
  else if (atype == STANDARD_TYPE(IGESSolid_SolidAssembly))
    return 16;
  else if (atype == STANDARD_TYPE(IGESSolid_SolidInstance))
    return 17;
  else if (atype == STANDARD_TYPE(IGESSolid_SolidOfLinearExtrusion))
    return 18;
  else if (atype == STANDARD_TYPE(IGESSolid_SolidOfRevolution))
    return 19;
  else if (atype == STANDARD_TYPE(IGESSolid_Sphere))
    return 20;
  else if (atype == STANDARD_TYPE(IGESSolid_SphericalSurface))
    return 21;
  else if (atype == STANDARD_TYPE(IGESSolid_ToroidalSurface))
    return 22;
  else if (atype == STANDARD_TYPE(IGESSolid_Torus))
    return 23;
  else if (atype == STANDARD_TYPE(IGESSolid_VertexList))
    return 24;
  return 0;
}
