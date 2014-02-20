// Created on: 2007-08-23
// Created by: Sergey ZARITCHNY
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#include <MDataStd_AsciiStringRetrievalDriver.ixx>
#include <PDataStd_AsciiString.hxx>
#include <TDataStd_AsciiString.hxx>
#include <TCollection_AsciiString.hxx>
#include <PCollection_HAsciiString.hxx>
#include <CDM_MessageDriver.hxx>

//=======================================================================
//function : MDataStd_AsciiStringRetrievalDriver
//purpose  : Constructor
//=======================================================================
MDataStd_AsciiStringRetrievalDriver::MDataStd_AsciiStringRetrievalDriver
  (const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{
}
//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================
Standard_Integer MDataStd_AsciiStringRetrievalDriver::VersionNumber() const
{ return 0; }

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================
Handle(Standard_Type) MDataStd_AsciiStringRetrievalDriver::SourceType() const
{ return STANDARD_TYPE(PDataStd_AsciiString); }

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) MDataStd_AsciiStringRetrievalDriver::NewEmpty() const
{ return new TDataStd_AsciiString (); }

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void MDataStd_AsciiStringRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,
const Handle(TDF_Attribute)& Target,const Handle(MDF_RRelocationTable)& ) const
{
  Handle(PDataStd_AsciiString) S = Handle(PDataStd_AsciiString)::DownCast (Source);
  Handle(TDataStd_AsciiString) T = Handle(TDataStd_AsciiString)::DownCast (Target);
  if(!S.IsNull() && !T.IsNull()) {
    if(!S->Get().IsNull()) {
      TCollection_AsciiString aString((S->Get())->Convert ());
      T->Set (aString);
    }
  }
}

