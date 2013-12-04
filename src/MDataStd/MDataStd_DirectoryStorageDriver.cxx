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

#include <MDataStd_DirectoryStorageDriver.ixx>
#include <PDataStd_Directory.hxx>
#include <TDataStd_Directory.hxx>
#include <CDM_MessageDriver.hxx>

MDataStd_DirectoryStorageDriver::MDataStd_DirectoryStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{
}

Standard_Integer MDataStd_DirectoryStorageDriver::VersionNumber() const
{ return 0; }

Handle(Standard_Type) MDataStd_DirectoryStorageDriver::SourceType() const
{ return STANDARD_TYPE(TDataStd_Directory); }

Handle(PDF_Attribute) MDataStd_DirectoryStorageDriver::NewEmpty() const
{ return new PDataStd_Directory; }

//void MDataStd_DirectoryStorageDriver::Paste(const Handle(TDF_Attribute)& Source,const Handle(PDF_Attribute)& Target,const Handle(MDF_SRelocationTable)& RelocTable) const
void MDataStd_DirectoryStorageDriver::Paste(const Handle(TDF_Attribute)& ,const Handle(PDF_Attribute)& ,const Handle(MDF_SRelocationTable)& ) const
{
}

