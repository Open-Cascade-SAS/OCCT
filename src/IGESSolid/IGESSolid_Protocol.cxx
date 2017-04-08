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
#include <IGESSolid_Face.hxx>
#include <IGESSolid_Loop.hxx>
#include <IGESSolid_ManifoldSolid.hxx>
#include <IGESSolid_PlaneSurface.hxx>
#include <IGESSolid_Protocol.hxx>
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

IMPLEMENT_STANDARD_RTTIEXT(IGESSolid_Protocol,IGESData_Protocol)

static int THE_IGESSolid_Protocol_deja = 0;
static Handle(Standard_Type) atype01,atype02,atype03,atype04,atype05,atype06,
  atype07,atype08,atype09,atype10,atype11,atype12,atype13,atype14,atype15,
  atype16,atype17,atype18,atype19,atype20,atype21,atype22,atype23,atype24;

IGESSolid_Protocol::IGESSolid_Protocol()
{
  if (THE_IGESSolid_Protocol_deja)
  {
    return;
  }
  THE_IGESSolid_Protocol_deja = 1;
  atype01 = STANDARD_TYPE(IGESSolid_Block);
  atype02 = STANDARD_TYPE(IGESSolid_BooleanTree);
  atype03 = STANDARD_TYPE(IGESSolid_ConeFrustum);
  atype04 = STANDARD_TYPE(IGESSolid_ConicalSurface);
  atype05 = STANDARD_TYPE(IGESSolid_Cylinder);
  atype06 = STANDARD_TYPE(IGESSolid_CylindricalSurface);
  atype07 = STANDARD_TYPE(IGESSolid_EdgeList);
  atype08 = STANDARD_TYPE(IGESSolid_Ellipsoid);
  atype09 = STANDARD_TYPE(IGESSolid_Face);
  atype10 = STANDARD_TYPE(IGESSolid_Loop);
  atype11 = STANDARD_TYPE(IGESSolid_ManifoldSolid);
  atype12 = STANDARD_TYPE(IGESSolid_PlaneSurface);
  atype13 = STANDARD_TYPE(IGESSolid_RightAngularWedge);
  atype14 = STANDARD_TYPE(IGESSolid_SelectedComponent);
  atype15 = STANDARD_TYPE(IGESSolid_Shell);
  atype16 = STANDARD_TYPE(IGESSolid_SolidAssembly);
  atype17 = STANDARD_TYPE(IGESSolid_SolidInstance);
  atype18 = STANDARD_TYPE(IGESSolid_SolidOfLinearExtrusion);
  atype19 = STANDARD_TYPE(IGESSolid_SolidOfRevolution);
  atype20 = STANDARD_TYPE(IGESSolid_Sphere);
  atype21 = STANDARD_TYPE(IGESSolid_SphericalSurface);
  atype22 = STANDARD_TYPE(IGESSolid_ToroidalSurface);
  atype23 = STANDARD_TYPE(IGESSolid_Torus);
  atype24 = STANDARD_TYPE(IGESSolid_VertexList);
}

    Standard_Integer IGESSolid_Protocol::NbResources () const
      {  return 1;  }

    Handle(Interface_Protocol) IGESSolid_Protocol::Resource
  (const Standard_Integer /*num*/) const
{
  Handle(Interface_Protocol) res = IGESGeom::Protocol();;
  return res;
}

    Standard_Integer IGESSolid_Protocol::TypeNumber
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
  else if (atype == atype18) return 18;
  else if (atype == atype19) return 19;
  else if (atype == atype20) return 20;
  else if (atype == atype21) return 21;
  else if (atype == atype22) return 22;
  else if (atype == atype23) return 23;
  else if (atype == atype24) return 24;
  return 0;
}
