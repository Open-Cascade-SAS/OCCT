// Created on: 1997-04-10
// Created by: PASCAL Denis
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <MDataStd_VariableStorageDriver.ixx>
#include <PDataStd_Variable.hxx>
#include <TDataStd_Variable.hxx>
#include <MDataStd.hxx>
#include <PCollection_HAsciiString.hxx>
#include <CDM_MessageDriver.hxx>
#include <CDM_MessageDriver.hxx>


//=======================================================================
//function : MDataStd_VariableStorageDriver
//purpose  : 
//=======================================================================

MDataStd_VariableStorageDriver::MDataStd_VariableStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{}


//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MDataStd_VariableStorageDriver::VersionNumber() const
{ return 0; }


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MDataStd_VariableStorageDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(TDataStd_Variable);
  return sourceType;
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(PDF_Attribute) MDataStd_VariableStorageDriver::NewEmpty () const {

  return new PDataStd_Variable ();
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MDataStd_VariableStorageDriver::Paste (
  const Handle(TDF_Attribute)&  Source,
  const Handle(PDF_Attribute)&        Target,
//  const Handle(MDF_SRelocationTable)& RelocTable) const
  const Handle(MDF_SRelocationTable)& ) const
{
  Handle(TDataStd_Variable) S = Handle(TDataStd_Variable)::DownCast (Source);
  Handle(PDataStd_Variable) T = Handle(PDataStd_Variable)::DownCast (Target);
  T->Constant (S->IsConstant ());  
  T->Unit(new PCollection_HAsciiString(S->Unit()));
}

