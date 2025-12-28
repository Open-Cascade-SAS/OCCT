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

#include <Standard_Type.hxx>
#include <Vrml_Material.hxx>
#include <VrmlConverter_ShadingAspect.hxx>

IMPLEMENT_STANDARD_RTTIEXT(VrmlConverter_ShadingAspect, Standard_Transient)

VrmlConverter_ShadingAspect::VrmlConverter_ShadingAspect()
{
  occ::handle<Vrml_Material> m = new Vrml_Material;
  Vrml_ShapeHints            sh;
  myFrontMaterial = m;
  myShapeHints    = sh;
  myHasNormals    = false;
  myHasMaterial   = false;
}

void VrmlConverter_ShadingAspect::SetFrontMaterial(const occ::handle<Vrml_Material>& aMaterial)
{
  myFrontMaterial = aMaterial;
}

occ::handle<Vrml_Material> VrmlConverter_ShadingAspect::FrontMaterial() const
{
  return myFrontMaterial;
}

void VrmlConverter_ShadingAspect::SetShapeHints(const Vrml_ShapeHints& aShapeHints)
{
  myShapeHints = aShapeHints;
}

Vrml_ShapeHints VrmlConverter_ShadingAspect::ShapeHints() const
{
  return myShapeHints;
}

void VrmlConverter_ShadingAspect::SetHasNormals(const bool OnOff)
{
  myHasNormals = OnOff;
}

bool VrmlConverter_ShadingAspect::HasNormals() const
{
  return myHasNormals;
}

void VrmlConverter_ShadingAspect::SetHasMaterial(const bool OnOff)
{
  myHasMaterial = OnOff;
}

bool VrmlConverter_ShadingAspect::HasMaterial() const
{
  return myHasMaterial;
}
