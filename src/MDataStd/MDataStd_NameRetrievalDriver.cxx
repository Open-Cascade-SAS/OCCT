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

#include <MDataStd_NameRetrievalDriver.ixx>
#include <PDataStd_Name.hxx>
#include <TDataStd_Name.hxx>
#include <TCollection_ExtendedString.hxx>
#include <PCollection_HExtendedString.hxx>
#include <CDM_MessageDriver.hxx>
#include <CDM_MessageDriver.hxx>

MDataStd_NameRetrievalDriver::MDataStd_NameRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{
}

Standard_Integer MDataStd_NameRetrievalDriver::VersionNumber() const
{ return 0; }

Handle(Standard_Type) MDataStd_NameRetrievalDriver::SourceType() const
{ return STANDARD_TYPE(PDataStd_Name); }

Handle(TDF_Attribute) MDataStd_NameRetrievalDriver::NewEmpty() const
{ return new TDataStd_Name (); }


#include <TCollection_AsciiString.hxx>
#include <TDF_Tool.hxx>
#include <TDF_Label.hxx>

#define BUGOCC425

void MDataStd_NameRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,const Handle(TDF_Attribute)& Target,const Handle(MDF_RRelocationTable)& ) const
{
  Handle(PDataStd_Name) S = Handle(PDataStd_Name)::DownCast (Source);
  Handle(TDataStd_Name) T = Handle(TDataStd_Name)::DownCast (Target);
#ifdef BUGOCC425
  if(!S->Get().IsNull()) {
#endif 
    TCollection_ExtendedString Name = (S->Get())->Convert ();
    T->Set (Name);
#ifdef BUGOCC425
  }
#endif 
}

