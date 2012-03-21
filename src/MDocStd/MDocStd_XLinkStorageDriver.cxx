// Created by: DAUTRY Philippe
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

//      	---------------------------

// Version:	0.0
//Version	Date		Purpose
//		0.0	Sep 17 1997	Creation



#include <MDocStd_XLinkStorageDriver.ixx>

#include <TDF_Label.hxx>
#include <TDocStd_XLink.hxx>
#include <PDocStd_XLink.hxx>
#include <TCollection_AsciiString.hxx>
#include <PCollection_HAsciiString.hxx>
#include <CDM_MessageDriver.hxx>

#define DeclareAndSpeedCast(V,T,Vdown) Handle(T) Vdown = *((Handle(T)*)& V)
#define DeclareConstAndSpeedCast(V,T,Vdown) const Handle(T)& Vdown = (Handle(T)&) V
#define SpeedCast(V,T,Vdown) Vdown = *((Handle(T)*)& V)

//=======================================================================
//function : MDocStd_XLinkStorageDriver
//purpose  : 
//=======================================================================

MDocStd_XLinkStorageDriver::MDocStd_XLinkStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{}


//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MDocStd_XLinkStorageDriver::VersionNumber() const
{ return 0; }


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MDocStd_XLinkStorageDriver::SourceType() const
{ return STANDARD_TYPE(TDocStd_XLink); }


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(PDF_Attribute) MDocStd_XLinkStorageDriver::NewEmpty() const
{ return new PDocStd_XLink(); }


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MDocStd_XLinkStorageDriver::Paste
(const Handle(TDF_Attribute)& aSource,
 const Handle(PDF_Attribute)& aTarget,
// const Handle(MDF_SRelocationTable)& aRelocTable) const
 const Handle(MDF_SRelocationTable)& ) const
{
  DeclareConstAndSpeedCast(aSource,TDocStd_XLink,txref);
  DeclareConstAndSpeedCast(aTarget,PDocStd_XLink,pxref);
  pxref->DocumentEntry(new PCollection_HAsciiString(txref->DocumentEntry()));
  pxref->LabelEntry(new PCollection_HAsciiString(txref->LabelEntry()));
}

