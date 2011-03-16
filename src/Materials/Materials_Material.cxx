// File:	Materials_Material.cxx
// Created:	Thu Oct 22 12:39:47 1992
// Author:	Gilles DEBARBOUILLE
//		<gde@bravox>



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
