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

#include <MDataStd_IntegerListRetrievalDriver.ixx>
#include <PDataStd_IntegerList.hxx>
#include <TDataStd_IntegerList.hxx>
#include <MDataStd.hxx>
#include <CDM_MessageDriver.hxx>

//=======================================================================
//function : MDataStd_IntegerListRetrievalDriver
//purpose  : 
//=======================================================================
MDataStd_IntegerListRetrievalDriver::MDataStd_IntegerListRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{

}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================
Standard_Integer MDataStd_IntegerListRetrievalDriver::VersionNumber() const
{ 
  return 0; 
}

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================
Handle(Standard_Type) MDataStd_IntegerListRetrievalDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(PDataStd_IntegerList);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) MDataStd_IntegerListRetrievalDriver::NewEmpty () const 
{
  return new TDataStd_IntegerList();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void MDataStd_IntegerListRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,
						const Handle(TDF_Attribute)& Target,
						const Handle(MDF_RRelocationTable)& ) const
{
  Handle(PDataStd_IntegerList) S = Handle(PDataStd_IntegerList)::DownCast (Source);
  Handle(TDataStd_IntegerList) T = Handle(TDataStd_IntegerList)::DownCast (Target);

  Standard_Integer i, lower = S->Lower(), upper = S->Upper();
  for (i = lower; i <= upper; i++)
  {
    T->Append(S->Value(i));
  }
}
