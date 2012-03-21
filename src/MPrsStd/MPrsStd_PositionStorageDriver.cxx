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



#include <MPrsStd_PositionStorageDriver.ixx>

#include <PDataXtd_Position.hxx>
#include <TDataXtd_Position.hxx>
#include <CDM_MessageDriver.hxx>

//=======================================================================
//function : MPrsStd_PositionStorageDriver
//purpose  : 
//=======================================================================
MPrsStd_PositionStorageDriver::MPrsStd_PositionStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{
}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================
Standard_Integer MPrsStd_PositionStorageDriver::VersionNumber() const
{ return 0; }

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================
Handle(Standard_Type) MPrsStd_PositionStorageDriver::SourceType() const
{ return STANDARD_TYPE(TDataXtd_Position); }

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(PDF_Attribute) MPrsStd_PositionStorageDriver::NewEmpty() const
{ return new PDataXtd_Position; }

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void MPrsStd_PositionStorageDriver::Paste(const Handle(TDF_Attribute)& Source,
							  const Handle(PDF_Attribute)& Target,
							  const Handle(MDF_SRelocationTable)& RelocTable) const
{
  Handle(TDataXtd_Position) S = Handle(TDataXtd_Position)::DownCast (Source);
  Handle(PDataXtd_Position) T = Handle(PDataXtd_Position)::DownCast (Target);  
  T->SetPosition(S->GetPosition());
}


