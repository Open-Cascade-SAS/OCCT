// Created on: 2007-08-01
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


#include <BinMDataStd.hxx>
#include <BinMDataStd_IntPackedMapDriver.hxx>
#include <BinMDF_ADriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <BinObjMgt_RRelocationTable.hxx>
#include <BinObjMgt_SRelocationTable.hxx>
#include <CDM_MessageDriver.hxx>
#include <Standard_Type.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TColStd_HPackedMapOfInteger.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <TDataStd_IntPackedMap.hxx>
#include <TDF_Attribute.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMDataStd_IntPackedMapDriver,BinMDF_ADriver)

//=======================================================================
//function : BinMDataStd_IntPackedMapDriver
//purpose  :
//=======================================================================
BinMDataStd_IntPackedMapDriver::BinMDataStd_IntPackedMapDriver
                         (const Handle(CDM_MessageDriver)& theMessageDriver)
     : BinMDF_ADriver (theMessageDriver, STANDARD_TYPE(TDataStd_IntPackedMap)->Name())
{
}

//=======================================================================
//function : NewEmpty
//purpose  :
//=======================================================================

Handle(TDF_Attribute) BinMDataStd_IntPackedMapDriver::NewEmpty() const
{
  return new TDataStd_IntPackedMap;
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================

Standard_Boolean BinMDataStd_IntPackedMapDriver::Paste
                         (const BinObjMgt_Persistent&  Source,
                          const Handle(TDF_Attribute)& Target,
                          BinObjMgt_RRelocationTable&  /*RelocTable*/) const
{
  Handle(TDataStd_IntPackedMap) aTagAtt = Handle(TDataStd_IntPackedMap)::DownCast(Target);
  if(aTagAtt.IsNull()) {
    WriteMessage (TCollection_ExtendedString("IntPackedMapDriver:: The target attribute is Null."));
    return Standard_False;
  }

  Standard_Integer aSize = 0;
  if (! (Source >> aSize)) {
    WriteMessage (TCollection_ExtendedString("Cannot retrieve size for IntPackedMap attribute."));
    return Standard_False;
  }
  if(aSize) {
    Handle(TColStd_HPackedMapOfInteger) aHMap = new TColStd_HPackedMapOfInteger ();
    Standard_Integer aKey;
    for(Standard_Integer i = 0; i< aSize; i++) {
      Standard_Boolean ok = Source >> aKey;
      if (!ok) {
	WriteMessage ("Cannot retrieve integer member for IntPackedMap attribute.");
	return Standard_False;
      }
      if(!aHMap->ChangeMap().Add( aKey )) return Standard_False;
    }
    aTagAtt->ChangeMap(aHMap);
  }

  Standard_Boolean aDelta(Standard_False);
  if(BinMDataStd::DocumentVersion() > 2) {
    Standard_Byte aDeltaValue;
    if (! (Source >> aDeltaValue))
      return Standard_False;
    else
      aDelta = (aDeltaValue != 0);
  }
  aTagAtt->SetDelta(aDelta);
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================

void BinMDataStd_IntPackedMapDriver::Paste
                         (const Handle(TDF_Attribute)& Source,
                          BinObjMgt_Persistent&        Target,
                          BinObjMgt_SRelocationTable&  /*RelocTable*/) const
{
  Handle(TDataStd_IntPackedMap) anAtt = Handle(TDataStd_IntPackedMap)::DownCast(Source);
  if (anAtt.IsNull()) {
    WriteMessage ("IntPackedMapDriver:: The source attribute is Null.");
    return;
  }
  Standard_Integer aSize = (anAtt->IsEmpty()) ? 0 : anAtt->Extent();
  Target << aSize;
  if(aSize) {
    TColStd_MapIteratorOfPackedMapOfInteger anIt(anAtt->GetMap());
    for(;anIt.More();anIt.Next())
      Target << anIt.Key();
  }
  Target << (Standard_Byte)(anAtt->GetDelta() ? 1 : 0);
}
