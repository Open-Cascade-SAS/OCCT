// Created on: 1997-04-10
// Created by: PASCAL Denis
// Copyright (c) 1997-1999 Matra Datavision
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

#include <MDataStd_VariableRetrievalDriver.ixx>
#include <TDataStd_Variable.hxx>
#include <PDataStd_Variable.hxx>
#include <MDataStd.hxx>
#include <PCollection_HAsciiString.hxx>
#include <CDM_MessageDriver.hxx>

//=======================================================================
//function : MDataStd_VariableRetrievalDriver
//purpose  : 
//=======================================================================

MDataStd_VariableRetrievalDriver::MDataStd_VariableRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{}


//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MDataStd_VariableRetrievalDriver::VersionNumber() const
{ return 0; }


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MDataStd_VariableRetrievalDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(PDataStd_Variable);
  return sourceType;
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) MDataStd_VariableRetrievalDriver::NewEmpty () const {

  return new TDataStd_Variable ();
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MDataStd_VariableRetrievalDriver::Paste 
(const Handle(PDF_Attribute)&  Source,
 const Handle(TDF_Attribute)&  Target,
// const Handle(MDF_RRelocationTable)& RelocTable) const
 const Handle(MDF_RRelocationTable)& ) const
{
  Handle(PDataStd_Variable) S = Handle(PDataStd_Variable)::DownCast (Source);
  Handle(TDataStd_Variable) T = Handle(TDataStd_Variable)::DownCast (Target);
  T->Constant (S->Constant());
  T->Unit(S->Unit()->Convert());
}

