// Created on: 2008-12-10
// Created by: Pavel TELKOV
// Copyright (c) 2008-2014 OPEN CASCADE SAS
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

#include <MXCAFDoc_DatumStorageDriver.ixx>
#include <XCAFDoc_Datum.hxx>
#include <PXCAFDoc_Datum.hxx>

#include <TCollection_HAsciiString.hxx>
#include <PCollection_HAsciiString.hxx>

//=======================================================================
//function : MXCAFDoc_DatumStorageDriver
//purpose  : 
//=======================================================================

MXCAFDoc_DatumStorageDriver::MXCAFDoc_DatumStorageDriver
  (const Handle(CDM_MessageDriver)& theMsgDriver) : MDF_ASDriver (theMsgDriver)
{}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MXCAFDoc_DatumStorageDriver::VersionNumber() const
{ return 0; }

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MXCAFDoc_DatumStorageDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(XCAFDoc_Datum);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(PDF_Attribute) MXCAFDoc_DatumStorageDriver::NewEmpty() const
{
  return new PXCAFDoc_Datum();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MXCAFDoc_DatumStorageDriver::Paste(const Handle(TDF_Attribute)& Source,
                                        const Handle(PDF_Attribute)& Target,
                                        const Handle(MDF_SRelocationTable)& /*RelocTable*/) const
{
  Handle(XCAFDoc_Datum) S = Handle(XCAFDoc_Datum)::DownCast (Source);
  Handle(PXCAFDoc_Datum) T = Handle(PXCAFDoc_Datum)::DownCast (Target);
  Handle(TCollection_HAsciiString) aNameStr = S->GetName();
  Handle(TCollection_HAsciiString) aDescrStr = S->GetDescription();
  Handle(TCollection_HAsciiString) anIdStr = S->GetIdentification();
  
  Handle(PCollection_HAsciiString) aName, aDescr, anId;
  if ( !aNameStr.IsNull() )
    aName = new PCollection_HAsciiString (aNameStr->String());
  if ( !aDescrStr.IsNull() )
    aDescr = new PCollection_HAsciiString (aDescrStr->String());
  if ( !anIdStr.IsNull() )
    anId = new PCollection_HAsciiString (anIdStr->String());

  T->Set(aName, aDescr, anId);
}
