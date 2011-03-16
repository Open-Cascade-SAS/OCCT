// File:      PXCAFDoc_Material.cxx
// Created:   10.12.08 11:46:02
// Author:    Pavel TELKOV
// Copyright: Open CASCADE 2008

#include <PXCAFDoc_Material.ixx>

//=======================================================================
//function : PXCAFDoc_Material
//purpose  : 
//=======================================================================

PXCAFDoc_Material::PXCAFDoc_Material ()
{}

//=======================================================================
//function : PXCAFDoc_Material
//purpose  : 
//=======================================================================

PXCAFDoc_Material::PXCAFDoc_Material 
  (const Handle(PCollection_HAsciiString)& theName,
   const Handle(PCollection_HAsciiString)& theDescr,
   const Standard_Real theDensity,
   const Handle(PCollection_HAsciiString)& theDensName,
   const Handle(PCollection_HAsciiString)& theDensValType)
: myName(theName),
  myDescr(theDescr),
  myDensity(theDensity),
  myDensName(theDensName),
  myDensValType(theDensValType)
{}
    
//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void PXCAFDoc_Material::Set 
  (const Handle(PCollection_HAsciiString)& theName,
   const Handle(PCollection_HAsciiString)& theDescr,
   const Standard_Real theDensity,
   const Handle(PCollection_HAsciiString)& theDensName,
   const Handle(PCollection_HAsciiString)& theDensValType)
{
  myName = theName;
  myDescr = theDescr;
  myDensity = theDensity;
  myDensName = theDensName;
  myDensValType = theDensValType;
}

//=======================================================================
//function : GetDensity
//purpose  : 
//=======================================================================

Standard_Real PXCAFDoc_Material::GetDensity () const
{
  return myDensity;
}

//=======================================================================
//function : GetName
//purpose  : 
//=======================================================================

Handle(PCollection_HAsciiString) PXCAFDoc_Material::GetName () const
{
  return myName;
}

//=======================================================================
//function : GetDescription
//purpose  : 
//=======================================================================

Handle(PCollection_HAsciiString) PXCAFDoc_Material::GetDescription () const
{
  return myDescr;
}

//=======================================================================
//function : GetDensName
//purpose  : 
//=======================================================================

Handle(PCollection_HAsciiString) PXCAFDoc_Material::GetDensName () const
{
  return myDensName;
}

//=======================================================================
//function : GetDensValType
//purpose  : 
//=======================================================================

Handle(PCollection_HAsciiString) PXCAFDoc_Material::GetDensValType () const
{
  return myDensValType;
}
