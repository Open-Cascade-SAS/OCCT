// Created on: 2000-09-11
// Created by: data exchange team
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#include <MXCAFDoc_CentroidStorageDriver.ixx>
#include <MDF_SRelocationTable.hxx>
#include <PTColStd_TransientPersistentMap.hxx>
#include <PXCAFDoc_Centroid.hxx>
#include <XCAFDoc_Centroid.hxx>

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

MXCAFDoc_CentroidStorageDriver::MXCAFDoc_CentroidStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver) : MDF_ASDriver (theMsgDriver)
{
}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

 Standard_Integer MXCAFDoc_CentroidStorageDriver::VersionNumber() const
{  return 0; } 

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

 Handle(Standard_Type) MXCAFDoc_CentroidStorageDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(XCAFDoc_Centroid);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

 Handle(PDF_Attribute) MXCAFDoc_CentroidStorageDriver::NewEmpty() const
{
  return new PXCAFDoc_Centroid();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

 void MXCAFDoc_CentroidStorageDriver::Paste(const Handle(TDF_Attribute)& Source,
                                            const Handle(PDF_Attribute)& Target,
                                            const Handle(MDF_SRelocationTable)& /*RelocTable*/) const
{
  Handle(XCAFDoc_Centroid) S = Handle(XCAFDoc_Centroid)::DownCast (Source);
  Handle(PXCAFDoc_Centroid) T = Handle(PXCAFDoc_Centroid)::DownCast (Target);
  
  T->Set(S->Get());
}
