// Created on: 2007-07-03
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


#include <CDM_MessageDriver.hxx>
#include <LDOM_MemManager.hxx>
#include <Standard_Type.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TColStd_DataMapIteratorOfDataMapOfStringInteger.hxx>
#include <TColStd_DataMapOfStringInteger.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TDataStd_DataMapIteratorOfDataMapOfStringByte.hxx>
#include <TDataStd_DataMapIteratorOfDataMapOfStringHArray1OfInteger.hxx>
#include <TDataStd_DataMapIteratorOfDataMapOfStringHArray1OfReal.hxx>
#include <TDataStd_DataMapIteratorOfDataMapOfStringReal.hxx>
#include <TDataStd_DataMapIteratorOfDataMapOfStringString.hxx>
#include <TDataStd_DataMapOfStringByte.hxx>
#include <TDataStd_DataMapOfStringHArray1OfInteger.hxx>
#include <TDataStd_DataMapOfStringHArray1OfReal.hxx>
#include <TDataStd_DataMapOfStringReal.hxx>
#include <TDataStd_DataMapOfStringString.hxx>
#include <TDataStd_NamedData.hxx>
#include <TDF_Attribute.hxx>
#include <XmlMDataStd_NamedDataDriver.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Document.hxx>
#include <XmlObjMgt_Persistent.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMDataStd_NamedDataDriver,XmlMDF_ADriver)
IMPLEMENT_DOMSTRING (FirstIntegerIndex, "firstI")
IMPLEMENT_DOMSTRING (LastIntegerIndex,  "lastI")
IMPLEMENT_DOMSTRING (FirstRealIndex,    "firstR")
IMPLEMENT_DOMSTRING (LastRealIndex,     "lastR")
IMPLEMENT_DOMSTRING (FirstStringIndex,  "firstS")
IMPLEMENT_DOMSTRING (LastStringIndex,   "lastS")
IMPLEMENT_DOMSTRING (FirstByteIndex,    "firstB")
IMPLEMENT_DOMSTRING (LastByteIndex,     "lastB")
IMPLEMENT_DOMSTRING (FirstIntArrIndex,  "firstIA")
IMPLEMENT_DOMSTRING (LastIntArrIndex,   "lastIA")
IMPLEMENT_DOMSTRING (FirstRealArrIndex, "firstRA")
IMPLEMENT_DOMSTRING (LastRealArrIndex,   "lastRA")

IMPLEMENT_DOMSTRING (ExtString, "string")
IMPLEMENT_DOMSTRING (Value,     "value")

#ifdef _WIN32
#define EXCEPTION ...
#else
#define EXCEPTION Standard_Failure
#endif
//=======================================================================
//function : XmlMDataStd_NamedDataDriver
//purpose  : Constructor
//=======================================================================
XmlMDataStd_NamedDataDriver::XmlMDataStd_NamedDataDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
     : XmlMDF_ADriver (theMsgDriver, NULL)
{

}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMDataStd_NamedDataDriver::NewEmpty() const
{
  return new TDataStd_NamedData();
}

//=======================================================================
static TCollection_ExtendedString SplitItemFromEnd(TCollection_ExtendedString& Key) 
{
  TCollection_ExtendedString aValue;
  const Standard_Integer aPos = Key. SearchFromEnd (" ");
  if(aPos == -1) return aValue;
  aValue = Key.Split(aPos-1);
  aValue.Remove(1,1);
  return aValue;
}
//=======================================================================
static TCollection_ExtendedString SplitItemFromStart(TCollection_ExtendedString& Key) 
{
  TCollection_ExtendedString aValue;
  const Standard_Integer aPos = Key. Search (" ");
  if(aPos == -1) return aValue;
  aValue = Key.Split(aPos);
  Key.Remove(Key.Length(),1);
  return aValue;
}
//=======================================================================
Handle(TColStd_HArray1OfInteger) BuildIntArray(const TCollection_AsciiString& ValString, 
					       const Standard_Integer theLen) 
{
  Handle(TColStd_HArray1OfInteger) anArr;
  if(ValString.Length() == 0 || !theLen) return anArr;
  anArr = new TColStd_HArray1OfInteger (1, theLen, 0);
  for(Standard_Integer i = 1;i <= theLen;i++) {
    const TCollection_AsciiString& aSVal = ValString.Token(" ",i);
    if(aSVal.Length())
      anArr->SetValue(i, aSVal.IntegerValue());      
  }
  return anArr;
}

//=======================================================================
Handle(TColStd_HArray1OfReal) BuildRealArray(const TCollection_AsciiString& ValString, 
					     const Standard_Integer theLen) 
{
  Handle(TColStd_HArray1OfReal) anArr;
  if(ValString.Length() == 0 || !theLen) return anArr;
  anArr = new TColStd_HArray1OfReal (1, theLen, .0);
  for(Standard_Integer i = 1;i <= theLen;i++) {
    const TCollection_AsciiString& aSVal = ValString.Token(" ",i);
    if(aSVal.Length())
      anArr->SetValue(i, aSVal.RealValue());      
  }
  return anArr;
}
//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMDataStd_NamedDataDriver::Paste(const XmlObjMgt_Persistent&  theSource,
						      const Handle(TDF_Attribute)& theTarget,
						      XmlObjMgt_RRelocationTable&  ) const
{
  Standard_Integer aFirstInd, aLastInd, ind;
  const XmlObjMgt_Element& anElement = theSource;

  //DataMapOfStringInteger: Read the FirstIndex; if the attribute is absent initialize to 1
  XmlObjMgt_DOMString aFirstIndex = anElement.getAttribute(::FirstIntegerIndex());
  if (aFirstIndex == NULL)
    aFirstInd = 1;
  else if (!aFirstIndex.GetInteger(aFirstInd)) 
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the first index for NamedData"
                                 " attribute (DataMapOfStringInteger) as \"")
        + aFirstIndex + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  // Read the LastIndex;
  XmlObjMgt_DOMString aLastIndex = anElement.getAttribute(::LastIntegerIndex());
  if(aLastIndex == NULL) {
    aFirstInd = 0;
    aLastInd  = 0;
  } else if (!aLastIndex.GetInteger(aLastInd)) {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the last index for NamedData"
                                 " attribute (DataMapOfStringInteger) as \"")
        + aLastIndex + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }
  try {
    Handle(TDataStd_NamedData) T = Handle(TDataStd_NamedData)::DownCast(theTarget);
    LDOM_Node aCurNode;

    if((aFirstInd | aLastInd) && aLastInd >= aFirstInd) {
      if ( !anElement.hasChildNodes() )
	{
	  TCollection_ExtendedString aMessageString =
	    TCollection_ExtendedString("Cannot retrieve DataMapOfStringInteger");
	  WriteMessage (aMessageString);
	  return Standard_False;
	}
      aCurNode = anElement.getFirstChild();
      LDOM_Element* aCurElement = (LDOM_Element*)&aCurNode;
      TCollection_ExtendedString aValueStr, aKey;
      TColStd_DataMapOfStringInteger aMap;
      for (ind = aFirstInd; ind < aLastInd; ind++) {
	XmlObjMgt::GetExtendedString( *aCurElement, aKey );
	aValueStr = SplitItemFromEnd(aKey);
	if(aValueStr.Length() == 0) {
	  TCollection_ExtendedString aMessageString =
	    TCollection_ExtendedString("Cannot retrieve a value from item = ") + aKey;
	  WriteMessage (aMessageString);
	  return Standard_False;
	}
	TCollection_AsciiString aVal(aValueStr,'?');
	Standard_Integer aValue = aVal.IntegerValue();
	aMap.Bind(aKey, aValue);
	aCurNode = aCurElement->getNextSibling();
	aCurElement = (LDOM_Element*)&aCurNode;
      }
      XmlObjMgt::GetExtendedString( *aCurElement, aKey );
      aValueStr = SplitItemFromEnd(aKey);
      if(aValueStr.Length() == 0) {
	TCollection_ExtendedString aMessageString =
	  TCollection_ExtendedString("Cannot retrieve a value from item = ") + aKey;
	WriteMessage (aMessageString);
	return Standard_False;
      }
      TCollection_AsciiString aVal(aValueStr,'?');
      Standard_Integer aValue = aVal.IntegerValue();
      aMap.Bind(aKey, aValue);
      T->ChangeIntegers(aMap);
    }

//DataMapOfStringReal
    aFirstIndex = anElement.getAttribute(::FirstRealIndex());
    if (aFirstIndex == NULL)
      aFirstInd = 1;
    else if (!aFirstIndex.GetInteger(aFirstInd)) 
      {
	TCollection_ExtendedString aMessageString =
	  TCollection_ExtendedString("Cannot retrieve the first index for NamedData "
				     "attribute (DataMapOfStringReal) as \"")
	    + aFirstIndex + "\"";
	WriteMessage (aMessageString);
	return Standard_False;
      }

  // Read the LastIndex;
    aLastIndex = anElement.getAttribute(::LastRealIndex());
    if(aLastIndex == NULL) {
      aFirstInd = 0;
      aLastInd  = 0;
    } else if (!aLastIndex.GetInteger(aLastInd)) {
      TCollection_ExtendedString aMessageString =
	TCollection_ExtendedString("Cannot retrieve the last index for NamedData"
				   " attribute (DataMapOfStringReal) as \"")
	  + aLastIndex + "\"";
      WriteMessage (aMessageString);
      return Standard_False;
    }

    if((aFirstInd | aLastInd) && aLastInd >= aFirstInd) { 
      if ( !anElement.hasChildNodes())
	{
	  TCollection_ExtendedString aMessageString =
	    TCollection_ExtendedString("Cannot retrieve DataMapOfStringReal");
	  WriteMessage (aMessageString);
	  return Standard_False;
	}
      
      LDOM_Element* aCurElement; 
      if (aCurNode.isNull())
	    aCurNode = anElement.getFirstChild();
      else    
        aCurNode    = ((LDOM_Element*)&aCurNode)->getNextSibling();
          
      aCurElement = (LDOM_Element*)&aCurNode;
      TCollection_ExtendedString aValueStr, aKey;
      TDataStd_DataMapOfStringReal aMap;
      for (ind = aFirstInd; ind < aLastInd; ind++) {
	XmlObjMgt::GetExtendedString( *aCurElement, aKey );
	aValueStr = SplitItemFromEnd(aKey);
	if(aValueStr.Length() == 0) {
	  TCollection_ExtendedString aMessageString =
	    TCollection_ExtendedString("Cannot retrieve a value from item = ") + aKey;
	  WriteMessage (aMessageString);
	  return Standard_False;
	}
	TCollection_AsciiString aVal(aValueStr,'?');
	Standard_Real aValue = aVal.RealValue();
	aMap.Bind(aKey, aValue);
	aCurNode = aCurElement->getNextSibling();
	aCurElement = (LDOM_Element*)&aCurNode;
      }
      XmlObjMgt::GetExtendedString( *aCurElement, aKey );
      aValueStr = SplitItemFromEnd(aKey);
      if(aValueStr.Length() == 0) {
	TCollection_ExtendedString aMessageString =
	  TCollection_ExtendedString("Cannot retrieve a value from item = ") + aKey;
	WriteMessage (aMessageString);
	return Standard_False;
      }
      TCollection_AsciiString aVal(aValueStr,'?');
      Standard_Real aValue = aVal.RealValue();
      aMap.Bind(aKey, aValue);
      T->ChangeReals(aMap);
    }

//DataMapOfStringString
    aFirstIndex = anElement.getAttribute(::FirstStringIndex());
    if (aFirstIndex == NULL)
      aFirstInd = 1;
    else if (!aFirstIndex.GetInteger(aFirstInd)) 
      {
	TCollection_ExtendedString aMessageString =
	  TCollection_ExtendedString("Cannot retrieve the first index for NamedData"
				     " attribute (DataMapOfStringString) as \"")
	    + aFirstIndex + "\"";
	WriteMessage (aMessageString);
	return Standard_False;
      }
    aLastIndex = anElement.getAttribute(::LastStringIndex());
    if(aLastIndex == NULL) {
      aFirstInd = 0;
      aLastInd  = 0;
    } else if (!aLastIndex.GetInteger(aLastInd)) {
      TCollection_ExtendedString aMessageString =
	TCollection_ExtendedString("Cannot retrieve the last index for NamedData"
				   " attribute (DataMapOfStringString) as \"")
	  + aLastIndex + "\"";
      WriteMessage (aMessageString);
      return Standard_False;
    }
    
    if((aFirstInd | aLastInd) && aLastInd >= aFirstInd) {
      if ( !anElement.hasChildNodes())
	{
	  TCollection_ExtendedString aMessageString =
	    TCollection_ExtendedString("Cannot retrieve DataMapOfStringString");
	  WriteMessage (aMessageString);
	  return Standard_False;
	}
      LDOM_Element* aCurElement;
      if (aCurNode.isNull())
	    aCurNode = anElement.getFirstChild();
      else 
	    aCurNode = ((LDOM_Element*)&aCurNode)->getNextSibling();
      
      aCurElement = (LDOM_Element*)&aCurNode;
      TCollection_ExtendedString aValue, aKey;
      TDataStd_DataMapOfStringString aMap;
      for (ind = aFirstInd; ind < aLastInd; ind++) {
	XmlObjMgt::GetExtendedString( *aCurElement, aKey );
	aValue = SplitItemFromStart(aKey); // ==>from start
	if(aValue.Length() == 0) {
	  TCollection_ExtendedString aMessageString =
	    TCollection_ExtendedString("Cannot retrieve a value from item = ") + aKey;
	  WriteMessage (aMessageString);
	  return Standard_False;
	}
	aMap.Bind(aKey, aValue);
	aCurNode = aCurElement->getNextSibling();
	aCurElement = (LDOM_Element*)&aCurNode;
      }
      XmlObjMgt::GetExtendedString( *aCurElement, aKey );
      aValue = SplitItemFromStart(aKey);
      if(aValue.Length() == 0) {
	TCollection_ExtendedString aMessageString =
	  TCollection_ExtendedString("Cannot retrieve a value from item = ") + aKey;
	WriteMessage (aMessageString);
	return Standard_False;
      }
      
      aMap.Bind(aKey, aValue);
      T->ChangeStrings(aMap);
    }

//DataMapOfStringByte
    aFirstIndex = anElement.getAttribute(::FirstByteIndex());
    if (aFirstIndex == NULL)
      aFirstInd = 1;
    else if (!aFirstIndex.GetInteger(aFirstInd)) 
      {
	TCollection_ExtendedString aMessageString =
	  TCollection_ExtendedString("Cannot retrieve the first index for NamedData "
				     "attribute (DataMapOfStringByte) as \"")
	    + aFirstIndex + "\"";
	WriteMessage (aMessageString);
	return Standard_False;
      }

  // Read the LastIndex;
    aLastIndex = anElement.getAttribute(::LastByteIndex());
    if(aLastIndex == NULL) {
      aFirstInd = 0;
      aLastInd  = 0;
    } else if (!aLastIndex.GetInteger(aLastInd)) {
      TCollection_ExtendedString aMessageString =
	TCollection_ExtendedString("Cannot retrieve the last index for NamedData"
				   " attribute (DataMapOfStringByte) as \"")
	  + aLastIndex + "\"";
      WriteMessage (aMessageString);
      return Standard_False;
    }
    
    if((aFirstInd | aLastInd) && aLastInd >= aFirstInd) { 
      if ( !anElement.hasChildNodes())
	{
	  TCollection_ExtendedString aMessageString =
	    TCollection_ExtendedString("Cannot retrieve DataMapOfStringByte");
	  WriteMessage (aMessageString);
	  return Standard_False;
	}
      
      LDOM_Element* aCurElement;
      if (aCurNode.isNull())
	    aCurNode = anElement.getFirstChild();
      else 
	    aCurNode = ((LDOM_Element*)&aCurNode)->getNextSibling();
      
      aCurElement = (LDOM_Element*)&aCurNode;
      TCollection_ExtendedString aValueStr, aKey;
      TDataStd_DataMapOfStringByte aMap;
      for (ind = aFirstInd; ind < aLastInd; ind++) {
	XmlObjMgt::GetExtendedString( *aCurElement, aKey );
	aValueStr = SplitItemFromEnd(aKey);
	if(aValueStr.Length() == 0) {
	  TCollection_ExtendedString aMessageString =
	    TCollection_ExtendedString("Cannot retrieve a value from item = ") + aKey;
	  WriteMessage (aMessageString);
	  return Standard_False;
	}
	
	TCollection_AsciiString aVal(aValueStr,'?');
	Standard_Byte aValue = (Standard_Byte)aVal.IntegerValue();
	
	aMap.Bind(aKey, aValue);
	aCurNode = aCurElement->getNextSibling();
	aCurElement = (LDOM_Element*)&aCurNode;
      }
      XmlObjMgt::GetExtendedString( *aCurElement, aKey );
      aValueStr = SplitItemFromEnd(aKey);
      if(aValueStr.Length() == 0) {
	TCollection_ExtendedString aMessageString =
	  TCollection_ExtendedString("Cannot retrieve a value from item = ") + aKey;
	WriteMessage (aMessageString);
	return Standard_False;
      }
      
      TCollection_AsciiString aVal(aValueStr,'?');
      Standard_Byte aValue = (Standard_Byte)aVal.IntegerValue();
      aMap.Bind(aKey, aValue);
      T->ChangeBytes(aMap);
    }
    
//DataMapOfStringHArray1OfInteger
    aFirstIndex = anElement.getAttribute(::FirstIntArrIndex());
    if (aFirstIndex == NULL)
      aFirstInd = 1;
    else if (!aFirstIndex.GetInteger(aFirstInd)) 
      {
	TCollection_ExtendedString aMessageString =
	  TCollection_ExtendedString("Cannot retrieve the first index for NamedData "
				     "attribute (DataMapOfStringHArray1OfInteger) as \"")
	    + aFirstIndex + "\"";
	WriteMessage (aMessageString);
	return Standard_False;
      }
    
  // Read the LastIndex;
    aLastIndex = anElement.getAttribute(::LastIntArrIndex());
    if(aLastIndex == NULL) {
      aFirstInd = 0;
      aLastInd  = 0;
    } else if (!aLastIndex.GetInteger(aLastInd)) {
      TCollection_ExtendedString aMessageString =
	TCollection_ExtendedString("Cannot retrieve the last index for NamedData"
				   " attribute (DataMapOfStringHArray1OfInteger) as \"")
	  + aLastIndex + "\"";
      WriteMessage (aMessageString);
      return Standard_False;
    }
    
    if((aFirstInd | aLastInd) && aLastInd >= aFirstInd) { 
      if ( !anElement.hasChildNodes())
	{
	  TCollection_ExtendedString aMessageString =
	    TCollection_ExtendedString("Cannot retrieve DataMapOfStringHArray1OfInteger");
	  WriteMessage (aMessageString);
	  return Standard_False;
	}
      LDOM_Element* aCurElement;
      if (aCurNode.isNull())
	    aCurNode = anElement.getFirstChild();
      else 
	    aCurNode =((LDOM_Element*)&aCurNode)->getNextSibling();
      
      aCurElement = (LDOM_Element*)&aCurNode;
      TCollection_ExtendedString aKey, aValueStr;
      TDataStd_DataMapOfStringHArray1OfInteger aMap;
      
      for (ind = aFirstInd; ind < aLastInd; ind++) {
	XmlObjMgt::GetExtendedString( *aCurElement, aKey );// Len - at the end
	aValueStr = SplitItemFromEnd(aKey);
	if(aValueStr.Length() == 0) {
	  TCollection_ExtendedString aMessageString =
	    TCollection_ExtendedString("Cannot retrieve a value from item = ") + aKey;
	  WriteMessage (aMessageString);
	  return Standard_False;
	}
	TCollection_AsciiString aVal(aValueStr,'?');
	Standard_Integer aLen = aVal.IntegerValue();
	
	TCollection_AsciiString aValueString = aCurElement->getAttribute(::Value()); 
	Handle(TColStd_HArray1OfInteger) aValue = BuildIntArray(aValueString, aLen);
	if(aValue.IsNull()) {
	  TCollection_ExtendedString aMessageString =
	    TCollection_ExtendedString("Cannot retrieve a value from item = ") + aKey;
	  WriteMessage (aMessageString);
	  return Standard_False;
	}
	
	aMap.Bind(aKey, aValue);
	aCurNode = aCurElement->getNextSibling();
	aCurElement = (LDOM_Element*)&aCurNode;
      }
      
      XmlObjMgt::GetExtendedString( *aCurElement, aKey );
      aValueStr = SplitItemFromEnd(aKey);
      if(aValueStr.Length() == 0) {
	TCollection_ExtendedString aMessageString =
	  TCollection_ExtendedString("Cannot retrieve a value from item = ") + aKey;
	WriteMessage (aMessageString);
	return Standard_False;
      }
      TCollection_AsciiString aVal(aValueStr,'?');
      Standard_Integer aLen = aVal.IntegerValue();
      TCollection_AsciiString aValueString = aCurElement->getAttribute(::Value());
      Handle(TColStd_HArray1OfInteger) aValue = BuildIntArray(aValueString, aLen);
      if(aValue.IsNull()) {
	TCollection_ExtendedString aMessageString =
	  TCollection_ExtendedString("Cannot retrieve a value from item = ") + aKey;
	WriteMessage (aMessageString);
	return Standard_False;
      }
      aMap.Bind(aKey, aValue);
      T->ChangeArraysOfIntegers(aMap);
    }

//DataMapOfStringHArray1OfReal
    aFirstIndex = anElement.getAttribute(::FirstRealArrIndex());
    if (aFirstIndex == NULL)
      aFirstInd = 1;
    else if (!aFirstIndex.GetInteger(aFirstInd)) 
      {
	TCollection_ExtendedString aMessageString =
	  TCollection_ExtendedString("Cannot retrieve the first index for NamedData "
				     "attribute (DataMapOfStringHArray1OfReal) as \"")
	    + aFirstIndex + "\"";
	WriteMessage (aMessageString);
	return Standard_False;
      }
    
  // Read the LastIndex;
    aLastIndex = anElement.getAttribute(::LastRealArrIndex());
    if(aLastIndex == NULL) {
      aFirstInd = 0;
      aLastInd  = 0;
    } else if (!aLastIndex.GetInteger(aLastInd)) {
      TCollection_ExtendedString aMessageString =
	TCollection_ExtendedString("Cannot retrieve the last index for NamedData"
                                 " attribute (DataMapOfStringHArray1OfReal) as \"")
	  + aLastIndex + "\"";
      WriteMessage (aMessageString);
      return Standard_False;
    }
    
    if((aFirstInd | aLastInd) && aLastInd >= aFirstInd) { 
      if ( !anElement.hasChildNodes())
	{
	  TCollection_ExtendedString aMessageString =
	    TCollection_ExtendedString("Cannot retrieve DataMapOfStringHArray1OfReal");
	  WriteMessage (aMessageString);
	  return Standard_False;
	}
      
      LDOM_Element* aCurElement;
      if (aCurNode.isNull())
	    aCurNode = anElement.getFirstChild();
      else 
	    aCurNode =((LDOM_Element*)&aCurNode)->getNextSibling();
      
      aCurElement = (LDOM_Element*)&aCurNode;
      TCollection_ExtendedString aKey, aValueStr;
      TDataStd_DataMapOfStringHArray1OfReal aMap;
      
      for (ind = aFirstInd; ind < aLastInd; ind++) {
	XmlObjMgt::GetExtendedString( *aCurElement, aKey );// Len - at the end
	aValueStr = SplitItemFromEnd(aKey);
	if(aValueStr.Length() == 0) {
	  TCollection_ExtendedString aMessageString =
 	  TCollection_ExtendedString("Cannot retrieve a value from item = ") + aKey;
	  WriteMessage (aMessageString);
	  return Standard_False;
	}
	TCollection_AsciiString aVal(aValueStr,'?');
	Standard_Integer aLen = aVal.IntegerValue();      
	
	TCollection_AsciiString aValueString = aCurElement->getAttribute(::Value());
	Handle(TColStd_HArray1OfReal) aValue = BuildRealArray(aValueString, aLen);
	if(aValue.IsNull()) {
	  TCollection_ExtendedString aMessageString =
	    TCollection_ExtendedString("Cannot retrieve a value from item = ") + aKey;
	  WriteMessage (aMessageString);
	  return Standard_False;
	}
	
	aMap.Bind(aKey, aValue);
	aCurNode = aCurElement->getNextSibling();
	aCurElement = (LDOM_Element*)&aCurNode;
      }
      
      XmlObjMgt::GetExtendedString( *aCurElement, aKey );
      aValueStr = SplitItemFromEnd(aKey);
      if(aValueStr.Length() == 0) {
	TCollection_ExtendedString aMessageString =
	  TCollection_ExtendedString("Cannot retrieve a value from item = ") + aKey;
	WriteMessage (aMessageString);
	return Standard_False;
      }
      TCollection_AsciiString aVal(aValueStr,'?');
      Standard_Integer aLen = aVal.IntegerValue();
      
      TCollection_AsciiString aValueString = aCurElement->getAttribute(::Value());
      Handle(TColStd_HArray1OfReal) aValue = BuildRealArray(aValueString, aLen);
      if(aValue.IsNull()) {
	TCollection_ExtendedString aMessageString =
	  TCollection_ExtendedString("Cannot retrieve a value from item = ") + aKey;
	WriteMessage (aMessageString);
	return Standard_False;
      }
      
      aMap.Bind(aKey, aValue);
    T->ChangeArraysOfReals(aMap);
    } 
  } catch (EXCEPTION) {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Unknown exception during data retrieve in NamedDatDriver ");
    WriteMessage (aMessageString);
      return Standard_False;} 
  
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void XmlMDataStd_NamedDataDriver::Paste(const Handle(TDF_Attribute)& theSource,
					XmlObjMgt_Persistent&        theTarget,
					XmlObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_NamedData) S = Handle(TDataStd_NamedData)::DownCast(theSource);
  if(S.IsNull()) {
    WriteMessage ("NamedDataDriver:: The source attribute is Null.");
    return;}

  Standard_Integer i=0, up;
  XmlObjMgt_Element& anElement = theTarget;
  XmlObjMgt_Document aDoc (anElement.getOwnerDocument());
  if(S->HasIntegers() && !S->GetIntegersContainer().IsEmpty()) {
    // store a set of elements with string in each of them
    up = S->GetIntegersContainer().Extent();
    theTarget.Element().setAttribute(::LastIntegerIndex(), up);

    TColStd_DataMapIteratorOfDataMapOfStringInteger itr(S->GetIntegersContainer());
    for (i=1; itr.More(); itr.Next(),i++) {
      const TCollection_ExtendedString aValueStr = 
	itr.Key() + ' ' + TCollection_ExtendedString(itr.Value());// key - value;
      XmlObjMgt_Element aCurTarget = aDoc.createElement( ::ExtString() );
      XmlObjMgt::SetExtendedString( aCurTarget, aValueStr );
      anElement.appendChild( aCurTarget );
    }
  }

  if(S->HasReals() && !S->GetRealsContainer().IsEmpty()) {
    up = S->GetRealsContainer().Extent();
    theTarget.Element().setAttribute(::LastRealIndex(), up);
    TDataStd_DataMapIteratorOfDataMapOfStringReal itr(S->GetRealsContainer());
    for (i=1; itr.More(); itr.Next(),i++) {
     const TCollection_ExtendedString aValueStr = 
	itr.Key() + ' ' + TCollection_ExtendedString(itr.Value());// key - value;
     XmlObjMgt_Element aCurTarget = aDoc.createElement( ::ExtString() );
     XmlObjMgt::SetExtendedString( aCurTarget, aValueStr );
     anElement.appendChild( aCurTarget );
    }
  } 

  if(S->HasStrings() && !S->GetStringsContainer().IsEmpty()) {
    up = S->GetStringsContainer().Extent();
    theTarget.Element().setAttribute(::LastStringIndex(), up);
    TDataStd_DataMapIteratorOfDataMapOfStringString itr(S->GetStringsContainer());
    for (i=1; itr.More(); itr.Next(),i++) {
      const TCollection_ExtendedString aValueStr = 
	itr.Key() + ' ' + TCollection_ExtendedString(itr.Value());// key(without blanks) - value;
      XmlObjMgt_Element aCurTarget = aDoc.createElement( ::ExtString() );
      XmlObjMgt::SetExtendedString( aCurTarget, aValueStr );
      anElement.appendChild( aCurTarget );
    }
  } 

  if(S->HasBytes() && !S->GetBytesContainer().IsEmpty()) {
    up = S->GetBytesContainer().Extent();
    theTarget.Element().setAttribute(::LastByteIndex(), up);
    TDataStd_DataMapIteratorOfDataMapOfStringByte itr(S->GetBytesContainer());
    for (i=1; itr.More(); itr.Next(),i++) {
      const TCollection_ExtendedString aValueStr = 
	itr.Key() + ' ' + TCollection_ExtendedString(itr.Value());// key - value;
      XmlObjMgt_Element aCurTarget = aDoc.createElement( ::ExtString() );
      XmlObjMgt::SetExtendedString( aCurTarget, aValueStr );
      anElement.appendChild( aCurTarget );
    }
  } 

  if(S->HasArraysOfIntegers() && !S->GetArraysOfIntegersContainer().IsEmpty()) {
    up = S->GetArraysOfIntegersContainer().Extent();
    theTarget.Element().setAttribute(::LastIntArrIndex(), up);
    TDataStd_DataMapIteratorOfDataMapOfStringHArray1OfInteger itr(S->GetArraysOfIntegersContainer());
    for (i=1; itr.More(); itr.Next(),i++) {
      const TColStd_Array1OfInteger& anArr1 = itr.Value()->Array1();
      const Standard_Integer aLen = anArr1.Upper() - anArr1.Lower() +1;

      const TCollection_ExtendedString aValueStr = 
	itr.Key() + ' ' + TCollection_ExtendedString(aLen);// key - Num_of_Arr_elements;
      XmlObjMgt_Element aCurTarget = aDoc.createElement( ::ExtString() );
      XmlObjMgt::SetExtendedString( aCurTarget,  aValueStr); //key
      anElement.appendChild( aCurTarget );

      //Value = Array
      TCollection_AsciiString aValueStr2;
      Standard_Integer j = anArr1.Lower();
       for(;;) {
	 aValueStr2 += TCollection_AsciiString(anArr1.Value(j));
	 if (j >= anArr1.Upper()) break;
	 aValueStr2 += ' ';
	 j++;
       }

      aCurTarget.setAttribute(::Value(), aValueStr2.ToCString());
    }
  }

  if(S->HasArraysOfReals() && !S->GetArraysOfRealsContainer().IsEmpty()) {
    up = S->GetArraysOfRealsContainer().Extent();
    theTarget.Element().setAttribute(::LastRealArrIndex(), up);
    TDataStd_DataMapIteratorOfDataMapOfStringHArray1OfReal itr(S->GetArraysOfRealsContainer());
    for (i=1; itr.More(); itr.Next(),i++) {
      const TColStd_Array1OfReal& anArr1 = itr.Value()->Array1();
      const Standard_Integer aLen = anArr1.Upper() - anArr1.Lower() +1;

      //key
      const TCollection_ExtendedString aValueStr = 
	itr.Key() + ' ' + TCollection_ExtendedString(aLen);// key - Num_of_Arr_elements;
      XmlObjMgt_Element aCurTarget = aDoc.createElement( ::ExtString() );
      XmlObjMgt::SetExtendedString( aCurTarget,  aValueStr); //key
      anElement.appendChild( aCurTarget );

      //Value = Array
      TCollection_AsciiString aValueStr2;
      Standard_Integer j = anArr1.Lower();
      for(;;) {
	char aValueChar[32];
	Sprintf(aValueChar, "%.15g", anArr1.Value(j));
	TCollection_AsciiString aValueStr3(aValueChar);
	aValueStr2 += aValueStr3;
	if (j >= anArr1.Upper()) break;
	aValueStr2 += ' ';
	j++;
      }
      aCurTarget.setAttribute(::Value(), aValueStr2.ToCString());

    }
  }
}
