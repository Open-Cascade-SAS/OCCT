// Created on: 1999-06-11
// Created by: Sergey RUIN
// Copyright (c) 1999-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <MDataStd_IntegerArrayStorageDriver.ixx>
#include <PDataStd_IntegerArray.hxx>
#include <PDataStd_IntegerArray_1.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <MDataStd.hxx>
#include <CDM_MessageDriver.hxx>

//=======================================================================
//function : MDataStd_IntegerArrayStorageDriver
//purpose  : 
//=======================================================================

MDataStd_IntegerArrayStorageDriver::MDataStd_IntegerArrayStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{}


//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MDataStd_IntegerArrayStorageDriver::VersionNumber() const
{ return 0; }


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MDataStd_IntegerArrayStorageDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(TDataStd_IntegerArray);
  return sourceType;
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(PDF_Attribute) MDataStd_IntegerArrayStorageDriver::NewEmpty () const {

  return new PDataStd_IntegerArray_1 ();
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MDataStd_IntegerArrayStorageDriver::Paste (
  const Handle(TDF_Attribute)&  Source,
  const Handle(PDF_Attribute)&        Target,
  const Handle(MDF_SRelocationTable)& /*RelocTable*/) const
{
  Handle(TDataStd_IntegerArray) S = Handle(TDataStd_IntegerArray)::DownCast (Source);
  Handle(PDataStd_IntegerArray_1) T = Handle(PDataStd_IntegerArray_1)::DownCast (Target);
  
  Standard_Integer i, lower =S->Lower() ,upper = S->Upper() ;
  T->Init(lower, upper);
  
  for(i = lower; i<=upper; i++) T->SetValue( i, S->Value(i) );  
  T->SetDelta(S->GetDelta());
}

