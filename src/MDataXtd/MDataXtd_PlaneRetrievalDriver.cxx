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

#include <MDataXtd_PlaneRetrievalDriver.ixx>
#include <PDataXtd_Plane.hxx>
#include <TDataXtd_Plane.hxx>
#include <CDM_MessageDriver.hxx>

MDataXtd_PlaneRetrievalDriver::MDataXtd_PlaneRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{
}

Standard_Integer MDataXtd_PlaneRetrievalDriver::VersionNumber() const
{ return 0; }

Handle(Standard_Type) MDataXtd_PlaneRetrievalDriver::SourceType() const
{ return STANDARD_TYPE (PDataXtd_Plane); }

Handle(TDF_Attribute) MDataXtd_PlaneRetrievalDriver::NewEmpty() const
{ return new TDataXtd_Plane; }

//void MDataXtd_PlaneRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,const Handle(TDF_Attribute)& Target,const Handle(MDF_RRelocationTable)& RelocTable) const
void MDataXtd_PlaneRetrievalDriver::Paste(const Handle(PDF_Attribute)& ,const Handle(TDF_Attribute)& ,const Handle(MDF_RRelocationTable)& ) const
{
}

