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

#include <VrmlConverter_ShadingAspect.ixx>
#include <Vrml_ShapeHints.hxx>
#include <Vrml_Material.hxx>

VrmlConverter_ShadingAspect::VrmlConverter_ShadingAspect()
{
 Handle(Vrml_Material) m = new Vrml_Material;
 Vrml_ShapeHints sh;
 myFrontMaterial = m;
 myShapeHints = sh;
 myHasNormals = Standard_False;
 myHasMaterial = Standard_False;
}
void VrmlConverter_ShadingAspect::SetFrontMaterial(const Handle(Vrml_Material)& aMaterial)
{
 myFrontMaterial = aMaterial;
}

Handle(Vrml_Material) VrmlConverter_ShadingAspect::FrontMaterial() const 
{
 return myFrontMaterial;
}

void VrmlConverter_ShadingAspect::SetShapeHints(const Vrml_ShapeHints& aShapeHints)
{
 myShapeHints = aShapeHints;
}

Vrml_ShapeHints VrmlConverter_ShadingAspect::ShapeHints() const 
{
 return  myShapeHints;
}

void VrmlConverter_ShadingAspect::SetHasNormals(const Standard_Boolean OnOff)
{
 myHasNormals = OnOff; 
}

Standard_Boolean VrmlConverter_ShadingAspect::HasNormals() const 
{
 return myHasNormals;
}

void VrmlConverter_ShadingAspect::SetHasMaterial(const Standard_Boolean OnOff)
{
 myHasMaterial = OnOff;

}

Standard_Boolean VrmlConverter_ShadingAspect::HasMaterial() const 
{
 return myHasMaterial;
}
