// Created on: 1992-10-22
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1992-1999 Matra Datavision
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




#include <Materials_Material.ixx>
#include <TCollection_HAsciiString.hxx>
#include <TCollection_AsciiString.hxx>

//=======================================================================
//function : Materials_Material
//purpose  : 
//=======================================================================

Materials_Material::Materials_Material(const Standard_CString amaterial)
     : Materials_FuzzyInstance("material definition")
{
  thematerial = new TCollection_HAsciiString(amaterial);
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

TCollection_AsciiString Materials_Material::Name() const
{
  return thematerial->String();
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void Materials_Material::Dump(Standard_OStream& astream) const
{
  TCollection_AsciiString string = thematerial->String();
  astream<<"\nMaterial : "<<string.ToCString()<<"\n";
  Materials_FuzzyInstance::Dump(astream);
}
