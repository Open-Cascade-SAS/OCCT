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

#include <MDF_RRelocationTable.hxx>
#include <MXCAFDoc_ColorRetrievalDriver.ixx>
#include <MgtTopLoc.hxx>
#include <PTColStd_PersistentTransientMap.hxx>
#include <PXCAFDoc_Color.hxx>
#include <XCAFDoc_Color.hxx>


//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

MXCAFDoc_ColorRetrievalDriver::MXCAFDoc_ColorRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver) : MDF_ARDriver (theMsgDriver)
{
}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

 Standard_Integer MXCAFDoc_ColorRetrievalDriver::VersionNumber() const
{
  return 0;
}

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

 Handle(Standard_Type) MXCAFDoc_ColorRetrievalDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(PXCAFDoc_Color);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

 Handle(TDF_Attribute) MXCAFDoc_ColorRetrievalDriver::NewEmpty() const
{
  return new XCAFDoc_Color();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

 void MXCAFDoc_ColorRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,
						    const Handle(TDF_Attribute)& Target,
						    const Handle(MDF_RRelocationTable)&// RelocTable
						    ) const
{
  Handle(PXCAFDoc_Color) S = Handle(PXCAFDoc_Color)::DownCast (Source);
  Handle(XCAFDoc_Color) T = Handle(XCAFDoc_Color)::DownCast (Target);
  
  T->Set(S->Get());

}

