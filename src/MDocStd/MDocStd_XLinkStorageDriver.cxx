// Created by: DAUTRY Philippe
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

