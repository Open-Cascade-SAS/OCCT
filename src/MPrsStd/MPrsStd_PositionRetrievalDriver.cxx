// Created on: 1998-04-07
// Created by: Jean-Pierre COMBE
// Copyright (c) 1998-1999 Matra Datavision
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



#include <MPrsStd_PositionRetrievalDriver.ixx>

#include <PDataXtd_Position.hxx>
#include <TDataXtd_Position.hxx>
#include <CDM_MessageDriver.hxx>

//=======================================================================
//function : MPrsStd_PositionRetrievalDriver
//purpose  : 
//=======================================================================
MPrsStd_PositionRetrievalDriver::MPrsStd_PositionRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================
Standard_Integer MPrsStd_PositionRetrievalDriver::VersionNumber() const
{ return 0; }

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================
Handle(Standard_Type) MPrsStd_PositionRetrievalDriver::SourceType() const
{ return STANDARD_TYPE(PDataXtd_Position); }

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) MPrsStd_PositionRetrievalDriver::NewEmpty() const
{ return new TDataXtd_Position; }

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void MPrsStd_PositionRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,
							    const Handle(TDF_Attribute)& Target,
							    const Handle(MDF_RRelocationTable)& RelocTable) const
{
  Handle(PDataXtd_Position) S = Handle(PDataXtd_Position)::DownCast (Source);
  Handle(TDataXtd_Position) T = Handle(TDataXtd_Position)::DownCast (Target);  
  T->SetPosition(S->GetPosition());
}

