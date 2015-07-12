// Created on: 2007-05-29
// Created by: Vlad Romashko
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


#include <CDM_MessageDriver.hxx>
#include <NCollection_LocalArray.hxx>
#include <Standard_Type.hxx>
#include <TColStd_HArray1OfByte.hxx>
#include <TDataStd_BooleanArray.hxx>
#include <TDF_Attribute.hxx>
#include <XmlMDataStd_BooleanArrayDriver.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Persistent.hxx>

IMPLEMENT_DOMSTRING (FirstIndexString, "first")
IMPLEMENT_DOMSTRING (LastIndexString,  "last")

//=======================================================================
//function : XmlMDataStd_BooleanArrayDriver
//purpose  : Constructor
//=======================================================================
XmlMDataStd_BooleanArrayDriver::XmlMDataStd_BooleanArrayDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
     : XmlMDF_ADriver (theMsgDriver, NULL)
{

}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMDataStd_BooleanArrayDriver::NewEmpty() const
{
  return new TDataStd_BooleanArray();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMDataStd_BooleanArrayDriver::Paste(const XmlObjMgt_Persistent&  theSource,
						       const Handle(TDF_Attribute)& theTarget,
						       XmlObjMgt_RRelocationTable&  ) const
{
  Standard_Integer aFirstInd, aLastInd, aValue;
  const XmlObjMgt_Element& anElement = theSource;

  // Read the FirstIndex; if the attribute is absent initialize to 1
  XmlObjMgt_DOMString aFirstIndex= anElement.getAttribute(::FirstIndexString());
  if (aFirstIndex == NULL)
    aFirstInd = 1;
  else if (!aFirstIndex.GetInteger(aFirstInd)) 
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the first index"
                                 " for BooleanArray attribute as \"")
        + aFirstIndex + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  // Read the LastIndex; the attribute should be present
  if (!anElement.getAttribute(::LastIndexString()).GetInteger(aLastInd)) 
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the last index"
                                 " for BooleanArray attribute as \"")
        + aFirstIndex + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  if (aFirstInd > aLastInd)
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("The last index is greater than the first index"
                                 " for BooleanArray attribute \"");
    WriteMessage (aMessageString);
    return Standard_False;
  }

  Handle(TDataStd_BooleanArray) aBooleanArray = Handle(TDataStd_BooleanArray)::DownCast(theTarget);
  aBooleanArray->Init(aFirstInd, aLastInd);
  Standard_Integer length = aLastInd - aFirstInd + 1;
  Handle(TColStd_HArray1OfByte) hArr = new TColStd_HArray1OfByte(0, length >> 3);
  TColStd_Array1OfByte& arr = hArr->ChangeArray1();

  Standard_Integer i = 0, upper = arr.Upper();
  Standard_CString aValueStr = Standard_CString(XmlObjMgt::GetStringValue(anElement).GetString());
  for (; i <= upper; i++)
  {
    if (!XmlObjMgt::GetInteger(aValueStr, aValue)) 
    {
      TCollection_ExtendedString aMessageString =
	TCollection_ExtendedString("Cannot retrieve integer member"
				   " for BooleanArray attribute as \"")
	  + aValueStr + "\"";
      WriteMessage (aMessageString);
      return Standard_False;
    }
    arr.SetValue(i, (Standard_Byte) aValue);
  }
  aBooleanArray->SetInternalArray(hArr);
  
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void XmlMDataStd_BooleanArrayDriver::Paste(const Handle(TDF_Attribute)& theSource,
					   XmlObjMgt_Persistent&        theTarget,
					   XmlObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_BooleanArray) aBooleanArray = Handle(TDataStd_BooleanArray)::DownCast(theSource);

  Standard_Integer aL = aBooleanArray->Lower();
  Standard_Integer anU = aBooleanArray->Upper();

  theTarget.Element().setAttribute(::FirstIndexString(), aL);
  theTarget.Element().setAttribute(::LastIndexString(), anU);

  const Handle(TColStd_HArray1OfByte)& hArr = aBooleanArray->InternalArray();
  const TColStd_Array1OfByte& arr = hArr->Array1();

  // Allocation of 4 chars for each byte.
  Standard_Integer iChar = 0;
  NCollection_LocalArray<Standard_Character> str;
  if (arr.Length())
    str.Allocate(4 * arr.Length() + 1);

  // Convert integers - compressed boolean values, to a string.
  Standard_Integer lower = arr.Lower(), i = lower, upper = arr.Upper();
  for (; i <= upper; i++)
  {
    const Standard_Byte& byte = arr.Value(i);
    iChar += Sprintf(&(str[iChar]), "%d ", byte);
  }

  if (arr.Length())
    XmlObjMgt::SetStringValue (theTarget, (Standard_Character*)str, Standard_True);
}
