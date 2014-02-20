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

#include <MDataStd_NameStorageDriver.ixx>
#include <PDataStd_Name.hxx>
#include <TDataStd_Name.hxx>
#include <TCollection_ExtendedString.hxx>
#include <PCollection_HExtendedString.hxx>
#include <CDM_MessageDriver.hxx>

#define BUGOCC425

MDataStd_NameStorageDriver::MDataStd_NameStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{
}

Standard_Integer MDataStd_NameStorageDriver::VersionNumber() const
{ return 0; }

Handle(Standard_Type) MDataStd_NameStorageDriver::SourceType() const
{ return STANDARD_TYPE(TDataStd_Name);}

Handle(PDF_Attribute) MDataStd_NameStorageDriver::NewEmpty() const
{ return new PDataStd_Name; }

void MDataStd_NameStorageDriver::Paste(const Handle(TDF_Attribute)& Source,const Handle(PDF_Attribute)& Target,const Handle(MDF_SRelocationTable)& ) const
{
  Handle(TDataStd_Name) S = Handle(TDataStd_Name)::DownCast (Source);
  Handle(PDataStd_Name) T = Handle(PDataStd_Name)::DownCast (Target);
#ifdef BUGOCC425
  if(S->Get().Length() != 0) {
#endif 
    Handle(PCollection_HExtendedString) Name = new PCollection_HExtendedString (S->Get());
    T->Set (Name);
#ifdef BUGOCC425
  }
#endif 
}

