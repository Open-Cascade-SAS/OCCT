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

#include <VrmlConverter_PointAspect.ixx>

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
