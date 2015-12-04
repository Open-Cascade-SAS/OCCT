// Created on: 2007-03-30
// Created by: Michael SAZONOV
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

// The original implementation Copyright: (C) RINA S.p.A

#include <XmlTObjDrivers_IntSparseArrayDriver.hxx>
#include <CDM_MessageDriver.hxx>
#include <TDF_Attribute.hxx>
#include <XmlObjMgt_Persistent.hxx>
#include <TObj_TIntSparseArray.hxx>
#include <TObj_Assistant.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_Tool.hxx>


IMPLEMENT_STANDARD_RTTIEXT(XmlTObjDrivers_IntSparseArrayDriver,XmlMDF_ADriver)

#define ITEM_ID "itemId_"
#define ITEM_VALUE "itemValue_"

//=======================================================================
//function : XmlTObjDrivers_IntSparseArrayDriver
//purpose  : constructor
//=======================================================================

XmlTObjDrivers_IntSparseArrayDriver::XmlTObjDrivers_IntSparseArrayDriver
                         (const Handle(CDM_MessageDriver)& theMessageDriver)
: XmlMDF_ADriver( theMessageDriver, NULL)
{
}

//=======================================================================
//function : NewEmpty
//purpose  : Creates a new attribute
//=======================================================================

Handle(TDF_Attribute) XmlTObjDrivers_IntSparseArrayDriver::NewEmpty() const
{
  return new TObj_TIntSparseArray;
}

//=======================================================================
//function : Paste
//purpose  : Retrieve. Translate the contents of <theSource> and put it
//           into <theTarget>.
//=======================================================================

Standard_Boolean XmlTObjDrivers_IntSparseArrayDriver::Paste
                         (const XmlObjMgt_Persistent&  theSource,
                          const Handle(TDF_Attribute)& theTarget,
                          XmlObjMgt_RRelocationTable&) const
{
  const XmlObjMgt_Element& anElement = theSource;
  Handle(TObj_TIntSparseArray) aTarget =
    Handle(TObj_TIntSparseArray)::DownCast(theTarget);

  // get pairs (ID, value) while ID != 0
  Standard_Integer i = 1;
  TCollection_AsciiString anItemID;
  TCollection_AsciiString anIdStr = TCollection_AsciiString( ITEM_ID ) + 
    TCollection_AsciiString( i );
  anItemID = anElement.getAttribute( anIdStr.ToCString() );
  while( anItemID.IsIntegerValue() && anItemID.IntegerValue() != 0 )
  {
    TCollection_AsciiString aStrIndex = TCollection_AsciiString( ITEM_VALUE ) +
      TCollection_AsciiString( i );
    TCollection_AsciiString anItemValue = anElement.getAttribute( aStrIndex.ToCString() );
    if ( anItemValue.IsIntegerValue() )
    {
      // store the value in the target array
      aTarget->SetDoBackup (Standard_False);
      aTarget->SetValue (anItemID.IntegerValue(), anItemValue.IntegerValue());
      aTarget->SetDoBackup (Standard_True);
    }
    i++;
  }
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : Store. Translate the contents of <theSource> and put it
//           into <theTarget>
//=======================================================================

void XmlTObjDrivers_IntSparseArrayDriver::Paste
                         (const Handle(TDF_Attribute)& theSource,
                          XmlObjMgt_Persistent&        theTarget,
                          XmlObjMgt_SRelocationTable&) const
{
  Handle(TObj_TIntSparseArray) aSource =
    Handle(TObj_TIntSparseArray)::DownCast (theSource);

  // put only non-null values as pairs (ID, value)
  // terminate the list by ID=0
  TObj_TIntSparseArray::Iterator anIt = aSource->GetIterator();
  Standard_Integer i = 1;
  for ( ; anIt.More() ; anIt.Next() )
  {
    Standard_Integer aValue = anIt.Value();
    if (aValue == 0)
      continue;
    TCollection_AsciiString anIdStr = TCollection_AsciiString( ITEM_ID ) +
      TCollection_AsciiString( i );
    TCollection_AsciiString aStrIndex = TCollection_AsciiString( ITEM_VALUE ) +
      TCollection_AsciiString( i );
    theTarget.Element().setAttribute( anIdStr.ToCString(), (Standard_Integer)anIt.Index() );
    theTarget.Element().setAttribute( aStrIndex.ToCString(), anIt.Value() );
    i++;
  }
  // write last item
  TCollection_AsciiString anIdStr = TCollection_AsciiString( ITEM_ID ) +
    TCollection_AsciiString( i );
  TCollection_AsciiString aStrIndex = TCollection_AsciiString( ITEM_VALUE ) +
    TCollection_AsciiString( i );
  theTarget.Element().setAttribute( anIdStr.ToCString(), 0 );
  theTarget.Element().setAttribute( aStrIndex.ToCString(), 0 );
}
