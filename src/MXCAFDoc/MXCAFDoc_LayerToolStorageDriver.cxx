// Created on: 2000-08-15
// Created by: data exchange team
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#include <MDF_SRelocationTable.hxx>
#include <MXCAFDoc_LayerToolStorageDriver.ixx>
#include <PXCAFDoc_LayerTool.hxx>
#include <XCAFDoc_LayerTool.hxx>

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

MXCAFDoc_LayerToolStorageDriver::MXCAFDoc_LayerToolStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver) : MDF_ASDriver (theMsgDriver)
{
}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

 Standard_Integer MXCAFDoc_LayerToolStorageDriver::VersionNumber() const
{  return 0; }

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

 Handle(Standard_Type) MXCAFDoc_LayerToolStorageDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(XCAFDoc_LayerTool);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

 Handle(PDF_Attribute) MXCAFDoc_LayerToolStorageDriver::NewEmpty() const
{
  return new PXCAFDoc_LayerTool();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MXCAFDoc_LayerToolStorageDriver::Paste (const Handle(TDF_Attribute)& /* Source */,
					     const Handle(PDF_Attribute)& /* Target */,
					     const Handle(MDF_SRelocationTable)& /*RelocTable*/) const
{
}

