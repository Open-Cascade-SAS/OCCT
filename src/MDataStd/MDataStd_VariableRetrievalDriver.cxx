// Created on: 1997-04-10
// Created by: PASCAL Denis
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



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

