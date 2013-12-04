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

#include <MXCAFDoc_MaterialStorageDriver.ixx>
#include <XCAFDoc_Material.hxx>
#include <PXCAFDoc_Material.hxx>

#include <TCollection_HAsciiString.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <PCollection_HAsciiString.hxx>
#include <PColStd_HArray1OfReal.hxx>

//=======================================================================
//function : MXCAFDoc_MaterialStorageDriver
//purpose  : 
//=======================================================================

MXCAFDoc_MaterialStorageDriver::MXCAFDoc_MaterialStorageDriver
  (const Handle(CDM_MessageDriver)& theMsgDriver) : MDF_ASDriver (theMsgDriver)
{}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MXCAFDoc_MaterialStorageDriver::VersionNumber() const
{ return 0; }

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MXCAFDoc_MaterialStorageDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(XCAFDoc_Material);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(PDF_Attribute) MXCAFDoc_MaterialStorageDriver::NewEmpty() const
{
  return new PXCAFDoc_Material();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MXCAFDoc_MaterialStorageDriver::Paste(const Handle(TDF_Attribute)& Source,
                                         const Handle(PDF_Attribute)& Target,
                                         const Handle(MDF_SRelocationTable)& /*RelocTable*/) const
{
  Handle(XCAFDoc_Material) S = Handle(XCAFDoc_Material)::DownCast (Source);
  Handle(PXCAFDoc_Material) T = Handle(PXCAFDoc_Material)::DownCast (Target);
  Handle(TCollection_HAsciiString) aNameStr = S->GetName();
  Handle(TCollection_HAsciiString) aDescrStr = S->GetDescription();
  Handle(TCollection_HAsciiString) aDensNameStr = S->GetDensName();
  Handle(TCollection_HAsciiString) aDensValTypeStr = S->GetDensValType();
  
  Handle(PCollection_HAsciiString) aName, aDescr, aDensName, aDensValType;
  if ( !aNameStr.IsNull() )
    aName = new PCollection_HAsciiString (aNameStr->String());
  if ( !aDescrStr.IsNull() )
    aDescr = new PCollection_HAsciiString (aDescrStr->String());
  if ( !aDensNameStr.IsNull() )
    aDensName = new PCollection_HAsciiString (aDensNameStr->String());
  if ( !aDensValTypeStr.IsNull() )
    aDensValType = new PCollection_HAsciiString (aDensValTypeStr->String());

  T->Set(aName, aDescr, S->GetDensity(), aDensName, aDensValType);
}
