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

#include <MDataXtd_PlacementStorageDriver.ixx>
#include <PDataXtd_Placement.hxx>
#include <TDataXtd_Placement.hxx>
#include <CDM_MessageDriver.hxx>


//=======================================================================
//function : MDataXtd_PlacementStorageDriver
//purpose  : 
//=======================================================================

MDataXtd_PlacementStorageDriver::MDataXtd_PlacementStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{
}


//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MDataXtd_PlacementStorageDriver::VersionNumber() const
{ return 0; }



//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MDataXtd_PlacementStorageDriver::SourceType() const
{ return STANDARD_TYPE(TDataXtd_Placement); }



//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(PDF_Attribute) MDataXtd_PlacementStorageDriver::NewEmpty() const
{ return new PDataXtd_Placement; }



//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MDataXtd_PlacementStorageDriver::Paste (
//  const Handle(TDF_Attribute)&        Source,
  const Handle(TDF_Attribute)&        ,
//  const Handle(PDF_Attribute)&        Target,
  const Handle(PDF_Attribute)&        ,
//  const Handle(MDF_SRelocationTable)& RelocTable) const
  const Handle(MDF_SRelocationTable)& ) const
{
}

