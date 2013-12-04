// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <MDataStd_NoteBookRetrievalDriver.ixx>
#include <PDataStd_NoteBook.hxx>
#include <TDataStd_NoteBook.hxx>
#include <CDM_MessageDriver.hxx>

MDataStd_NoteBookRetrievalDriver::MDataStd_NoteBookRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{
}

Standard_Integer MDataStd_NoteBookRetrievalDriver::VersionNumber() const
{ return 0; }

Handle(Standard_Type) MDataStd_NoteBookRetrievalDriver::SourceType() const
{ return STANDARD_TYPE(PDataStd_NoteBook); }

Handle(TDF_Attribute) MDataStd_NoteBookRetrievalDriver::NewEmpty() const
{ return new TDataStd_NoteBook; }

//void MDataStd_NoteBookRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,const Handle(TDF_Attribute)& Target,const Handle(MDF_RRelocationTable)& RelocTable) const
void MDataStd_NoteBookRetrievalDriver::Paste(const Handle(PDF_Attribute)& ,const Handle(TDF_Attribute)& ,const Handle(MDF_RRelocationTable)& ) const
{
}

