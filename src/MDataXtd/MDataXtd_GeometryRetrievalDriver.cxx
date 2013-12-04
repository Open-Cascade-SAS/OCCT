// Created on: 1997-11-19
// Created by: Denis PASCAL
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

#include <MDataXtd_GeometryRetrievalDriver.ixx>
#include <TDataXtd_GeometryEnum.hxx>
#include <TDataXtd_Geometry.hxx>
#include <PDataXtd_Geometry.hxx>
#include <MDataXtd.hxx>
#include <CDM_MessageDriver.hxx>



//=======================================================================
//function : MDataXtd_GeometryRetrievalDriver
//purpose  : 
//=======================================================================

MDataXtd_GeometryRetrievalDriver::MDataXtd_GeometryRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{
}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MDataXtd_GeometryRetrievalDriver::VersionNumber() const
{ return 0; }


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MDataXtd_GeometryRetrievalDriver::SourceType() const
{ return STANDARD_TYPE(PDataXtd_Geometry);  }




//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) MDataXtd_GeometryRetrievalDriver::NewEmpty() const
{ return new TDataXtd_Geometry (); }



//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MDataXtd_GeometryRetrievalDriver::Paste (
  const Handle(PDF_Attribute)&        Source,
  const Handle(TDF_Attribute)&        Target,
//  const Handle(MDF_RRelocationTable)& RelocTable) const
  const Handle(MDF_RRelocationTable)& ) const
{
  Handle(PDataXtd_Geometry) S = 
     Handle(PDataXtd_Geometry)::DownCast (Source);
  Handle(TDataXtd_Geometry) T = 
    Handle(TDataXtd_Geometry)::DownCast (Target);

  T->SetType (MDataXtd::IntegerToGeometryType (S->GetType ()));
}



