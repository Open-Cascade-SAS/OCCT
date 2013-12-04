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

#include <MDataXtd_PointStorageDriver.ixx>
#include <PDataXtd_Point.hxx>
#include <TDataXtd_Point.hxx>
#include <CDM_MessageDriver.hxx>

MDataXtd_PointStorageDriver::MDataXtd_PointStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver) {}

Standard_Integer MDataXtd_PointStorageDriver::VersionNumber() const

{ return 0; }

Handle(Standard_Type) MDataXtd_PointStorageDriver::SourceType() const
{ return STANDARD_TYPE(TDataXtd_Point);}

Handle(PDF_Attribute) MDataXtd_PointStorageDriver::NewEmpty() const
{ return new PDataXtd_Point (); }

//void MDataXtd_PointStorageDriver::Paste(const Handle(TDF_Attribute)& Source,const Handle(PDF_Attribute)& Target,const Handle(MDF_SRelocationTable)& RelocTable) const
void MDataXtd_PointStorageDriver::Paste(const Handle(TDF_Attribute)& ,const Handle(PDF_Attribute)& ,const Handle(MDF_SRelocationTable)& ) const
{
}

