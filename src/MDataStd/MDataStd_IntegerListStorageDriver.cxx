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

#include <MDataStd_IntegerListStorageDriver.ixx>
#include <PDataStd_IntegerList.hxx>
#include <TDataStd_IntegerList.hxx>
#include <MDataStd.hxx>
#include <CDM_MessageDriver.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>

//=======================================================================
//function : MDataStd_IntegerListStorageDriver
//purpose  : 
//=======================================================================
MDataStd_IntegerListStorageDriver::MDataStd_IntegerListStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{

}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================
Standard_Integer MDataStd_IntegerListStorageDriver::VersionNumber() const
{ 
  return 0;
}


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================
Handle(Standard_Type) MDataStd_IntegerListStorageDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(TDataStd_IntegerList);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(PDF_Attribute) MDataStd_IntegerListStorageDriver::NewEmpty() const 
{
  return new PDataStd_IntegerList();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void MDataStd_IntegerListStorageDriver::Paste(const Handle(TDF_Attribute)&  Source,
					      const Handle(PDF_Attribute)&        Target,
					      const Handle(MDF_SRelocationTable)& /*RelocTable*/) const
{
  const Handle(TDataStd_IntegerList) S = Handle(TDataStd_IntegerList)::DownCast (Source);
  const Handle(PDataStd_IntegerList) T = Handle(PDataStd_IntegerList)::DownCast (Target);
  
  Standard_Integer lower(1), upper = S->Extent(), i = lower;
  if(upper == 0) {
    lower = 0;
    T->Init(lower, upper);
  }
  else if (upper >= lower)
  {
    T->Init(lower, upper);
    TColStd_ListIteratorOfListOfInteger itr(S->List());
    for (; itr.More(); itr.Next(), i++) {
      T->SetValue(i, itr.Value());
    }
  }
}
