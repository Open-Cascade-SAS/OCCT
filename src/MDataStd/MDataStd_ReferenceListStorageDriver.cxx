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

#include <MDataStd_ReferenceListStorageDriver.ixx>
#include <PDataStd_ReferenceList.hxx>
#include <TDataStd_ReferenceList.hxx>
#include <MDataStd.hxx>
#include <CDM_MessageDriver.hxx>
#include <TDF_ListIteratorOfLabelList.hxx>
#include <TDF_Tool.hxx>
#include <TDF_Label.hxx>
#include <PCollection_HExtendedString.hxx>

//=======================================================================
//function : MDataStd_ReferenceListStorageDriver
//purpose  : 
//=======================================================================
MDataStd_ReferenceListStorageDriver::MDataStd_ReferenceListStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{

}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================
Standard_Integer MDataStd_ReferenceListStorageDriver::VersionNumber() const
{ 
  return 0;
}


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================
Handle(Standard_Type) MDataStd_ReferenceListStorageDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(TDataStd_ReferenceList);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(PDF_Attribute) MDataStd_ReferenceListStorageDriver::NewEmpty() const 
{
  return new PDataStd_ReferenceList();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void MDataStd_ReferenceListStorageDriver::Paste(const Handle(TDF_Attribute)&  Source,
						const Handle(PDF_Attribute)&        Target,
						const Handle(MDF_SRelocationTable)& /*RelocTable*/) const
{
  Handle(TDataStd_ReferenceList) S = Handle(TDataStd_ReferenceList)::DownCast (Source);
  Handle(PDataStd_ReferenceList) T = Handle(PDataStd_ReferenceList)::DownCast (Target);
  
  Standard_Integer lower = 1, upper = S->Extent(), i = lower;
  if (upper >= lower)
  {
    T->Init(lower, upper);
    TDF_ListIteratorOfLabelList itr(S->List());
    for (; itr.More(); itr.Next(), i++) 
    {
      TDF_Label L = itr.Value();
      if (!L.IsNull())
      {
	TCollection_AsciiString tvalue;
	TDF_Tool::Entry(L, tvalue);
	Handle(PCollection_HExtendedString) pvalue = new PCollection_HExtendedString(tvalue);
	T->SetValue(i, pvalue);
      }
    }
  }
}
