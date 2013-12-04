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

#include <MXCAFDoc_VolumeRetrievalDriver.ixx>
#include <PXCAFDoc_Volume.hxx>
#include <XCAFDoc_Volume.hxx>

//=======================================================================
//function : MCAFDoc_VolumeRetrievalDriver
//purpose  : 
//=======================================================================

MXCAFDoc_VolumeRetrievalDriver::MXCAFDoc_VolumeRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver) : MDF_ARDriver (theMsgDriver)
{}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MXCAFDoc_VolumeRetrievalDriver::VersionNumber() const
{ return 0; }

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

 Handle(Standard_Type) MXCAFDoc_VolumeRetrievalDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(PXCAFDoc_Volume);
  return sourceType;
}
//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

 Handle(TDF_Attribute) MXCAFDoc_VolumeRetrievalDriver::NewEmpty() const
{
  return new XCAFDoc_Volume();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

 void MXCAFDoc_VolumeRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,
						    const Handle(TDF_Attribute)& Target,
						    const Handle(MDF_RRelocationTable)& /*RelocTable*/) const
{
  Handle(PXCAFDoc_Volume) S = Handle(PXCAFDoc_Volume)::DownCast (Source);
  Handle(XCAFDoc_Volume) T = Handle(XCAFDoc_Volume)::DownCast (Target);
  
  T->Set(S->Get());
}
