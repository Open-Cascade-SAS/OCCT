// Created on: 2007-08-27
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

#include <MDataStd_IntPackedMapRetrievalDriver.ixx>
#include <PDataStd_IntPackedMap.hxx>
#include <TDataStd_IntPackedMap.hxx>
#include <CDM_MessageDriver.hxx>
#include <TColStd_HPackedMapOfInteger.hxx>
#include <TCollection_ExtendedString.hxx>

//=======================================================================
//function : MDataStd_IntPackedMapRetrievalDriver
//purpose  : Constructor
//=======================================================================
MDataStd_IntPackedMapRetrievalDriver::MDataStd_IntPackedMapRetrievalDriver
  (const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{
}
//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================
Standard_Integer MDataStd_IntPackedMapRetrievalDriver::VersionNumber() const
{ return 0; }

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================
Handle(Standard_Type) MDataStd_IntPackedMapRetrievalDriver::SourceType() const
{ return STANDARD_TYPE(PDataStd_IntPackedMap); }

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) MDataStd_IntPackedMapRetrievalDriver::NewEmpty() const
{ return new TDataStd_IntPackedMap (); }

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void MDataStd_IntPackedMapRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,
const Handle(TDF_Attribute)& Target,const Handle(MDF_RRelocationTable)& ) const
{
  Handle(PDataStd_IntPackedMap) aS = Handle(PDataStd_IntPackedMap)::DownCast (Source);
  Handle(TDataStd_IntPackedMap) aT = Handle(TDataStd_IntPackedMap)::DownCast (Target);
  if(!aS.IsNull() && !aT.IsNull()) {
    if(!aS->IsEmpty()) {
      Handle(TColStd_HPackedMapOfInteger) aHMap = new TColStd_HPackedMapOfInteger ();
      Standard_Integer aKey;
      for(Standard_Integer i = aS->Lower(); i<= aS->Upper() ; i++) {
	aKey = aS->GetValue(i);
	if(!aHMap->ChangeMap().Add( aKey )) {
	  WriteMessage(
	  TCollection_ExtendedString("error retrieving integer mamaber of the attribute TDataStd_IntPackedMap"));
	  return;
	}
      }
      aT->ChangeMap(aHMap);
    }
  } else 
    WriteMessage(TCollection_ExtendedString("error retrieving attribute TDataStd_IntPackedMap"));
}
