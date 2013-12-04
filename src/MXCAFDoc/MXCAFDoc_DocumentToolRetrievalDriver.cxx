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
#include <MXCAFDoc_DocumentToolRetrievalDriver.ixx>
#include <MgtTopLoc.hxx>
#include <PTColStd_PersistentTransientMap.hxx>
#include <PXCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <TDF_Label.hxx>


//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

MXCAFDoc_DocumentToolRetrievalDriver::MXCAFDoc_DocumentToolRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver) : MDF_ARDriver (theMsgDriver)
{
}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

 Standard_Integer MXCAFDoc_DocumentToolRetrievalDriver::VersionNumber() const
{
  return 0;
}

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

 Handle(Standard_Type) MXCAFDoc_DocumentToolRetrievalDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(PXCAFDoc_DocumentTool);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

 Handle(TDF_Attribute) MXCAFDoc_DocumentToolRetrievalDriver::NewEmpty() const
{
  return new XCAFDoc_DocumentTool();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

 void MXCAFDoc_DocumentToolRetrievalDriver::Paste(const Handle(PDF_Attribute)& ,//Source,
						    const Handle(TDF_Attribute)& Target,
						    const Handle(MDF_RRelocationTable)&// RelocTable
						    ) const
{
  Handle(XCAFDoc_DocumentTool) T = Handle(XCAFDoc_DocumentTool)::DownCast (Target);
  T->Init();
//  TDF_Label L = T->Label();
//  if (L.IsNull()) {
//    cout<<"XCAFDoc_DocumentTool::Paste  FUCK!!!"<<endl;
//    return;
//  }
//  L.ForgetAttribute(XCAFDoc_DocumentTool::GetID());
//  XCAFDoc_DocumentTool::Set(L, Standard_False);
}

