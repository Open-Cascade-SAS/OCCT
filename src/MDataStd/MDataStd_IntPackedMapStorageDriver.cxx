// Created on: 2007-08-23
// Created by: Sergey ZARITCHNY
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#include <MDataStd_IntPackedMapStorageDriver.ixx>
#include <PDataStd_IntPackedMap.hxx>
#include <PDataStd_IntPackedMap_1.hxx>
#include <TDataStd_IntPackedMap.hxx>
#include <PColStd_HArray1OfInteger.hxx>
#include <CDM_MessageDriver.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <TCollection_ExtendedString.hxx>
//=======================================================================
//function : MDataStd_IntPackedMapStorageDriver
//purpose  : Constructor
//=======================================================================
MDataStd_IntPackedMapStorageDriver::MDataStd_IntPackedMapStorageDriver
  (const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{
}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================
Standard_Integer MDataStd_IntPackedMapStorageDriver::VersionNumber() const
{ return 0; }

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================
Handle(Standard_Type) MDataStd_IntPackedMapStorageDriver::SourceType() const
{ return STANDARD_TYPE(TDataStd_IntPackedMap);}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(PDF_Attribute) MDataStd_IntPackedMapStorageDriver::NewEmpty() const
{ return new PDataStd_IntPackedMap_1; }

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void MDataStd_IntPackedMapStorageDriver::Paste(const Handle(TDF_Attribute)& Source,
  const Handle(PDF_Attribute)& Target,const Handle(MDF_SRelocationTable)& ) const
{
  Handle(TDataStd_IntPackedMap)   aS = 
    Handle(TDataStd_IntPackedMap)::DownCast (Source);
  Handle(PDataStd_IntPackedMap_1) aT = 
    Handle(PDataStd_IntPackedMap_1)::DownCast (Target);
  if(!aS.IsNull() && !aT.IsNull()) {
    Standard_Integer aSize = (aS->IsEmpty()) ? 0 : aS->Extent();
    if(aSize) {
      aT->Init (1, aSize);
      TColStd_MapIteratorOfPackedMapOfInteger anIt(aS->GetMap());
      for(Standard_Integer i = 1;anIt.More();anIt.Next(),i++)
	aT->SetValue(i, anIt.Key());
    }
    aT->SetDelta(aS->GetDelta());
  } else 
    WriteMessage(TCollection_ExtendedString ("error storing attribute TDataStd_IntPackedMap"));
}
