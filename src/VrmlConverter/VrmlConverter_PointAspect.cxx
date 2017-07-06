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
#include <VrmlConverter_PointAspect.hxx>

IMPLEMENT_STANDARD_RTTIEXT(VrmlConverter_PointAspect,Standard_Transient)

VrmlConverter_PointAspect::VrmlConverter_PointAspect()
{
 myHasMaterial = Standard_False;
}

VrmlConverter_PointAspect::VrmlConverter_PointAspect (const Handle(Vrml_Material)& aMaterial,
                                                      const Standard_Boolean OnOff)
{
 myMaterial = aMaterial;
 myHasMaterial = OnOff;
}


void VrmlConverter_PointAspect::SetMaterial(const Handle(Vrml_Material)& aMaterial)
{
 myMaterial = aMaterial;
}

Handle(Vrml_Material) VrmlConverter_PointAspect::Material() const 
{
 return myMaterial;
}
void VrmlConverter_PointAspect::SetHasMaterial(const Standard_Boolean OnOff)
{
 myHasMaterial = OnOff;
}

Standard_Boolean VrmlConverter_PointAspect::HasMaterial() const 
{
 return myHasMaterial;
}
