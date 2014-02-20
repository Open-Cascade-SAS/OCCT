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

#include <MDataStd_BooleanListStorageDriver.ixx>
#include <PDataStd_BooleanList.hxx>
#include <TDataStd_BooleanList.hxx>
#include <MDataStd.hxx>
#include <CDM_MessageDriver.hxx>
#include <TDataStd_ListIteratorOfListOfByte.hxx>

//=======================================================================
//function : MDataStd_BooleanListStorageDriver
//purpose  : 
//=======================================================================
MDataStd_BooleanListStorageDriver::MDataStd_BooleanListStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{

}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================
Standard_Integer MDataStd_BooleanListStorageDriver::VersionNumber() const
{ 
  return 0;
}


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================
Handle(Standard_Type) MDataStd_BooleanListStorageDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(TDataStd_BooleanList);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(PDF_Attribute) MDataStd_BooleanListStorageDriver::NewEmpty() const 
{
  return new PDataStd_BooleanList();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void MDataStd_BooleanListStorageDriver::Paste(const Handle(TDF_Attribute)&  Source,
					      const Handle(PDF_Attribute)&        Target,
					      const Handle(MDF_SRelocationTable)& /*RelocTable*/) const
{
  Handle(TDataStd_BooleanList) S = Handle(TDataStd_BooleanList)::DownCast (Source);
  Handle(PDataStd_BooleanList) T = Handle(PDataStd_BooleanList)::DownCast (Target);
  
  Standard_Integer lower = 1, upper = S->Extent(), i = lower;
  if (upper >= lower)
  {
    T->Init(lower, upper);
    TDataStd_ListIteratorOfListOfByte itr(S->List());
    for (; itr.More(); itr.Next(), i++) 
    {
      T->SetValue(i, itr.Value());
    }
  }
}
