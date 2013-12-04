// Created on: 1997-04-10
// Created by: VAUTHIER Jean-Claude
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

#include <MDataStd_IntegerRetrievalDriver.ixx>
#include <TDataStd_Integer.hxx>
#include <PDataStd_Integer.hxx>
#include <TDF_Label.hxx>
#include <PCollection_HAsciiString.hxx>
#include <CDM_MessageDriver.hxx>



//=======================================================================
//function : MDataStd_IntegerRetrievalDriver
//purpose  : 
//=======================================================================

MDataStd_IntegerRetrievalDriver::MDataStd_IntegerRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{}


//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MDataStd_IntegerRetrievalDriver::VersionNumber() const
{ return 0; }


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MDataStd_IntegerRetrievalDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(PDataStd_Integer);
  return sourceType;
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) MDataStd_IntegerRetrievalDriver::NewEmpty () const {

  return new TDataStd_Integer ();
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MDataStd_IntegerRetrievalDriver::Paste (
  const Handle(PDF_Attribute)&        Source,
  const Handle(TDF_Attribute)&        Target,
//  const Handle(MDF_RRelocationTable)&       RelocTable) const
  const Handle(MDF_RRelocationTable)&       ) const
{
  Handle(PDataStd_Integer) S = Handle(PDataStd_Integer)::DownCast (Source);
  Handle(TDataStd_Integer) T = Handle(TDataStd_Integer)::DownCast (Target);
  T->Set (S->Get ());
}

