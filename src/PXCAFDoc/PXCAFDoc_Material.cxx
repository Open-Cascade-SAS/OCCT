// Created on: 2008-12-10
// Created by: Pavel TELKOV
// Copyright (c) 2008-2012 OPEN CASCADE SAS
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
