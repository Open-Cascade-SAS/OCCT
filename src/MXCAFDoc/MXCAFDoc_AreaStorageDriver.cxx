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

#include <MXCAFDoc_AreaStorageDriver.ixx>
#include <XCAFDoc_Area.hxx>
#include <PXCAFDoc_Area.hxx>

//=======================================================================
//function : MXCAFDoc_AreaStorageDriver
//purpose  : 
//=======================================================================

MXCAFDoc_AreaStorageDriver::MXCAFDoc_AreaStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver) : MDF_ASDriver (theMsgDriver)
{}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MXCAFDoc_AreaStorageDriver::VersionNumber() const
{ return 0; }

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

 Handle(Standard_Type) MXCAFDoc_AreaStorageDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(XCAFDoc_Area);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

 Handle(PDF_Attribute) MXCAFDoc_AreaStorageDriver::NewEmpty() const
{
  return new PXCAFDoc_Area();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

 void MXCAFDoc_AreaStorageDriver::Paste(const Handle(TDF_Attribute)& Source,
						  const Handle(PDF_Attribute)& Target,
						  const Handle(MDF_SRelocationTable)& /*RelocTable*/) const
{
  Handle(XCAFDoc_Area) S = Handle(XCAFDoc_Area)::DownCast (Source);
  Handle(PXCAFDoc_Area) T = Handle(PXCAFDoc_Area)::DownCast (Target);
  
  T->Set(S->Get());
}
