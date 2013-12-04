// Created on: 2007-05-29
// Created by: Vlad Romashko
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#include <MDataStd_TickStorageDriver.ixx>
#include <PDataStd_Tick.hxx>
#include <TDataStd_Tick.hxx>
#include <CDM_MessageDriver.hxx>

//=======================================================================
//function : MDataStd_TickStorageDriver
//purpose  : 
//=======================================================================
MDataStd_TickStorageDriver::MDataStd_TickStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{
}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================
Standard_Integer MDataStd_TickStorageDriver::VersionNumber() const
{ 
  return 0; 
}

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================
Handle(Standard_Type) MDataStd_TickStorageDriver::SourceType() const
{ 
  return STANDARD_TYPE(TDataStd_Tick); 
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(PDF_Attribute) MDataStd_TickStorageDriver::NewEmpty() const
{ 
  return new PDataStd_Tick; 
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void MDataStd_TickStorageDriver::Paste (const Handle(TDF_Attribute)& ,
					const Handle(PDF_Attribute)& ,
					const Handle(MDF_SRelocationTable)& ) const
{
}
