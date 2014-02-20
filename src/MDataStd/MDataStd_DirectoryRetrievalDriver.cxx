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

#include <MDataStd_DirectoryRetrievalDriver.ixx>
#include <PDataStd_Directory.hxx>
#include <TDataStd_Directory.hxx>
#include <CDM_MessageDriver.hxx>

MDataStd_DirectoryRetrievalDriver::MDataStd_DirectoryRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{}

Standard_Integer MDataStd_DirectoryRetrievalDriver::VersionNumber() const
{ return 0; }

Handle(Standard_Type) MDataStd_DirectoryRetrievalDriver::SourceType() const
{ return STANDARD_TYPE(PDataStd_Directory); }

Handle(TDF_Attribute) MDataStd_DirectoryRetrievalDriver::NewEmpty() const
{ return new TDataStd_Directory; }

void MDataStd_DirectoryRetrievalDriver::Paste(const Handle(PDF_Attribute)&,
                                              const Handle(TDF_Attribute)& ,
                                              const Handle(MDF_RRelocationTable)&) const
{
}

