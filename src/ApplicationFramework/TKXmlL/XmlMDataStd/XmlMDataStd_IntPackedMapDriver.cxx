// Created on: 2007-08-22
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

#include <Message_Messenger.hxx>
#include <NCollection_LocalArray.hxx>
#include <Standard_Type.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <TDataStd_IntPackedMap.hxx>
#include <TDF_Attribute.hxx>
#include <TDocStd_FormatVersion.hxx>
#include <XmlMDataStd.hxx>
#include <XmlMDataStd_IntPackedMapDriver.hxx>
#include <XmlMDF_ADriver.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Persistent.hxx>
#include <XmlObjMgt_RRelocationTable.hxx>
#include <XmlObjMgt_SRelocationTable.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMDataStd_IntPackedMapDriver, XmlMDF_ADriver)
IMPLEMENT_DOMSTRING(IntPackedMapSize, "mapsize")
IMPLEMENT_DOMSTRING(IsDeltaOn, "delta")

//=================================================================================================

XmlMDataStd_IntPackedMapDriver::XmlMDataStd_IntPackedMapDriver(
  const Handle(Message_Messenger)& theMsgDriver)
    : XmlMDF_ADriver(theMsgDriver, STANDARD_TYPE(TDataStd_IntPackedMap)->Name())
{
}

//=================================================================================================

Handle(TDF_Attribute) XmlMDataStd_IntPackedMapDriver::NewEmpty() const
{
  return (new TDataStd_IntPackedMap());
}

//=======================================================================
// function : Paste()
// purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMDataStd_IntPackedMapDriver::Paste(
  const XmlObjMgt_Persistent&  theSource,
  const Handle(TDF_Attribute)& theTarget,
  XmlObjMgt_RRelocationTable&  theRelocTable) const
{
  Handle(TDataStd_IntPackedMap) aPackedMap = Handle(TDataStd_IntPackedMap)::DownCast(theTarget);
  if (!aPackedMap.IsNull())
  {
    Standard_Integer         aSize;
    const XmlObjMgt_Element& anElement = theSource;
    XmlObjMgt_DOMString      aSizeDStr = anElement.getAttribute(::IntPackedMapSize());
    if (aSizeDStr == NULL)
      aSize = 0;
    else if (!aSizeDStr.GetInteger(aSize))
    {
      TCollection_ExtendedString aMessageString =
        TCollection_ExtendedString("Cannot retrieve the Map size"
                                   " for IntPackedMap attribute as \"")
        + aSize + "\"";
      myMessageDriver->Send(aMessageString, Message_Fail);
      return Standard_False;
    }
    Handle(TColStd_HPackedMapOfInteger) aHMap = new TColStd_HPackedMapOfInteger();
    Standard_Boolean                    Ok    = Standard_True;
    if (aSize)
    {
      Standard_CString aValueString =
        Standard_CString(XmlObjMgt::GetStringValue(anElement).GetString());
      //      Handle(TColStd_HPackedMapOfInteger) aHMap = new TColStd_HPackedMapOfInteger ();
      for (Standard_Integer i = 1; i <= aSize; i++)
      {
        Standard_Integer aValue;
        if (!XmlObjMgt::GetInteger(aValueString, aValue))
        {
          Ok = Standard_False;
          break;
        }
        if (!aHMap->ChangeMap().Add(aValue))
        {
          Ok = Standard_False;
          break;
        }
      }
      if (!Ok)
      {
        TCollection_ExtendedString aMessageString =
          TCollection_ExtendedString("Cannot retrieve integer member"
                                     " for IntPackedMap attribute as \"")
          + aValueString + "\"";
        myMessageDriver->Send(aMessageString, Message_Fail);
        return Standard_False;
      }
      if (aPackedMap->ChangeMap(aHMap))
        Ok = Standard_True;
    }
    if (Ok)
    {
      Standard_Boolean aDelta(Standard_False);

      if (theRelocTable.GetHeaderData()->StorageVersion().IntegerValue()
          >= TDocStd_FormatVersion_VERSION_3)
      {
        Standard_Integer aDeltaValue;
        if (!anElement.getAttribute(::IsDeltaOn()).GetInteger(aDeltaValue))
        {
          TCollection_ExtendedString aMessageString =
            TCollection_ExtendedString("Cannot retrieve the isDelta value"
                                       " for IntPackedMap attribute as \"")
            + aDeltaValue + "\"";
          myMessageDriver->Send(aMessageString, Message_Fail);
          return Standard_False;
        }
        else
          aDelta = aDeltaValue != 0;
      }
      aPackedMap->SetDelta(aDelta);
      return Standard_True;
    }
  }
  myMessageDriver->Send("error retrieving Map for type TDataStd_IntPackedMap", Message_Fail);
  return Standard_False;
}

//=======================================================================
// function : Paste()
// purpose  : transient -> persistent (store)
//=======================================================================
void XmlMDataStd_IntPackedMapDriver::Paste(const Handle(TDF_Attribute)& theSource,
                                           XmlObjMgt_Persistent&        theTarget,
                                           XmlObjMgt_SRelocationTable&) const
{
  Handle(TDataStd_IntPackedMap) aS = Handle(TDataStd_IntPackedMap)::DownCast(theSource);
  if (aS.IsNull())
  {
    myMessageDriver->Send("IntPackedMapDriver:: The source attribute is Null.", Message_Warning);
    return;
  }

  Standard_Integer aSize = (aS->IsEmpty()) ? 0 : aS->Extent();
  theTarget.Element().setAttribute(::IntPackedMapSize(), aSize);
  theTarget.Element().setAttribute(::IsDeltaOn(), aS->GetDelta() ? 1 : 0);

  if (aSize)
  {
    // Allocation of 12 chars for each integer including the space.
    // An example: -2 147 483 648
    Standard_Integer                           iChar = 0;
    NCollection_LocalArray<Standard_Character> str(12 * aSize + 1);

    TColStd_MapIteratorOfPackedMapOfInteger anIt(aS->GetMap());
    for (; anIt.More(); anIt.Next())
    {
      const Standard_Integer intValue = anIt.Key();
      iChar += Sprintf(&(str[iChar]), "%d ", intValue);
    }

    // No occurrence of '&', '<' and other irregular XML characters
    XmlObjMgt::SetStringValue(theTarget, (Standard_Character*)str, Standard_True);
  }
}
