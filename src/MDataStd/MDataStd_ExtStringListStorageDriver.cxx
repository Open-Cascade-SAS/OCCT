// Created on: 2007-05-29
// Created by: Vlad Romashko
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#include <MDataStd_ExtStringListStorageDriver.ixx>
#include <PDataStd_ExtStringList.hxx>
#include <TDataStd_ExtStringList.hxx>
#include <MDataStd.hxx>
#include <CDM_MessageDriver.hxx>
#include <TDataStd_ListIteratorOfListOfExtendedString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <PCollection_HExtendedString.hxx>

//=======================================================================
//function : MDataStd_ExtStringListStorageDriver
//purpose  : 
//=======================================================================
MDataStd_ExtStringListStorageDriver::MDataStd_ExtStringListStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{
}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================
Standard_Integer MDataStd_ExtStringListStorageDriver::VersionNumber() const
{ 
  return 0;
}


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================
Handle(Standard_Type) MDataStd_ExtStringListStorageDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(TDataStd_ExtStringList);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(PDF_Attribute) MDataStd_ExtStringListStorageDriver::NewEmpty() const 
{
  return new PDataStd_ExtStringList();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void MDataStd_ExtStringListStorageDriver::Paste(const Handle(TDF_Attribute)& Source,
						const Handle(PDF_Attribute)& Target,
						const Handle(MDF_SRelocationTable)& /*RelocTable*/) const
{
  const Handle(TDataStd_ExtStringList) S = Handle(TDataStd_ExtStringList)::DownCast (Source);
  const Handle(PDataStd_ExtStringList) T = Handle(PDataStd_ExtStringList)::DownCast (Target);
  if(S.IsNull()) return;
  Standard_Integer lower(1), upper = S->Extent();
  if(upper == 0) {
    lower = 0;
    T->Init(lower, upper);
  }
  else if (upper >= lower)
  {
    T->Init(lower, upper);
    TDataStd_ListIteratorOfListOfExtendedString itr(S->List());
    for (Standard_Integer i = lower; itr.More(); itr.Next(), i++) 
    {
      const TCollection_ExtendedString& tvalue = itr.Value();
      const Handle(PCollection_HExtendedString)& pvalue = new PCollection_HExtendedString(tvalue);
      T->SetValue(i, pvalue);
    }
  }
}
