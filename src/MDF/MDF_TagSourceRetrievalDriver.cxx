// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <MDF_TagSourceRetrievalDriver.ixx>
#include <PDF_TagSource.hxx>
#include <TDF_TagSource.hxx>
#include <CDM_MessageDriver.hxx>

MDF_TagSourceRetrievalDriver::MDF_TagSourceRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{
}

Standard_Integer MDF_TagSourceRetrievalDriver::VersionNumber() const
{ return 0; }

Handle(Standard_Type) MDF_TagSourceRetrievalDriver::SourceType() const
{ return STANDARD_TYPE(PDF_TagSource); }

Handle(TDF_Attribute) MDF_TagSourceRetrievalDriver::NewEmpty() const
{ return new TDF_TagSource; }

//void MDF_TagSourceRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,const Handle(TDF_Attribute)& Target,const Handle(MDF_RRelocationTable)& RelocTable) const
void MDF_TagSourceRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,const Handle(TDF_Attribute)& Target,const Handle(MDF_RRelocationTable)& ) const
{
  Handle(PDF_TagSource) S = Handle(PDF_TagSource)::DownCast (Source);
  Handle(TDF_TagSource) T = Handle(TDF_TagSource)::DownCast (Target);
  T->Set (S->Get ());
}

