// Created on: 2000-09-08
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

#include <MXCAFDoc_AreaRetrievalDriver.ixx>
#include <PXCAFDoc_Area.hxx>
#include <XCAFDoc_Area.hxx>

//=======================================================================
//function : MCAFDoc_AreaRetrievalDriver
//purpose  : 
//=======================================================================

MXCAFDoc_AreaRetrievalDriver::MXCAFDoc_AreaRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver) : MDF_ARDriver (theMsgDriver)
{}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MXCAFDoc_AreaRetrievalDriver::VersionNumber() const
{ return 0; }

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

 Handle(Standard_Type) MXCAFDoc_AreaRetrievalDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(PXCAFDoc_Area);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

 Handle(TDF_Attribute) MXCAFDoc_AreaRetrievalDriver::NewEmpty() const
{
  return new XCAFDoc_Area();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

 void MXCAFDoc_AreaRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,
						    const Handle(TDF_Attribute)& Target,
						    const Handle(MDF_RRelocationTable)& /*RelocTable*/) const
{
  Handle(PXCAFDoc_Area) S = Handle(PXCAFDoc_Area)::DownCast (Source);
  Handle(XCAFDoc_Area) T = Handle(XCAFDoc_Area)::DownCast (Target);
  
  T->Set(S->Get());
}
