// Created on: 2008-12-10
// Created by: Pavel TELKOV
// Copyright (c) 2008-2014 OPEN CASCADE SAS
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

#include <MDF_RRelocationTable.hxx>
#include <MXCAFDoc_MaterialToolRetrievalDriver.ixx>
#include <PXCAFDoc_MaterialTool.hxx>
#include <XCAFDoc_MaterialTool.hxx>


//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

MXCAFDoc_MaterialToolRetrievalDriver::MXCAFDoc_MaterialToolRetrievalDriver
  (const Handle(CDM_MessageDriver)& theMsgDriver) : MDF_ARDriver (theMsgDriver)
{
}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MXCAFDoc_MaterialToolRetrievalDriver::VersionNumber() const
{
  return 0;
}

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MXCAFDoc_MaterialToolRetrievalDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(PXCAFDoc_MaterialTool);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) MXCAFDoc_MaterialToolRetrievalDriver::NewEmpty() const
{
  return new XCAFDoc_MaterialTool();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MXCAFDoc_MaterialToolRetrievalDriver::Paste 
  (const Handle(PDF_Attribute)& /* Source */,
   const Handle(TDF_Attribute)& /* Target */,
   const Handle(MDF_RRelocationTable)& /*RelocTable*/) const
{
}
