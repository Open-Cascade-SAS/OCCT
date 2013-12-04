// Created on: 2008-12-10
// Created by: Pavel TELKOV
// Copyright (c) 2008-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <PXCAFDoc_Material.ixx>

//=======================================================================
//function : PXCAFDoc_Material
//purpose  : 
//=======================================================================

PXCAFDoc_Material::PXCAFDoc_Material () : myDensity(0.)
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
