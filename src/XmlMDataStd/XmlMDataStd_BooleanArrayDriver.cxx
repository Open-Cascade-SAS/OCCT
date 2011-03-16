// File:        XmlMDataStd_BooleanArrayDriver.cxx
// Created:     May 29 11:40:00 2007
// Author:      Vlad Romashko
//  	    	<vladislav.romashko@opencascade.com>
// Copyright:   Open CASCADE

#include <XmlMDataStd_BooleanArrayDriver.ixx>
#include <TDataStd_BooleanArray.hxx>
#include <TColStd_HArray1OfByte.hxx>
#include <XmlObjMgt.hxx>

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
  Standard_Integer aFirstInd, aLastInd, aValue, ind;
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
  Handle(TColStd_HArray1OfByte) array = new TColStd_HArray1OfByte(0, length >> 3);

  Standard_Integer i = 0, upper = array->Upper();
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
    array->SetValue(i, (Standard_Byte) aValue);
  }
  aBooleanArray->SetInternalArray(array);
  
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
  TCollection_AsciiString aValueStr;

  theTarget.Element().setAttribute(::FirstIndexString(), aL);
  theTarget.Element().setAttribute(::LastIndexString(), anU);

  const Handle(TColStd_HArray1OfByte)& array = aBooleanArray->InternalArray();
  Standard_Integer lower = array->Lower(), i = lower, upper = array->Upper();
  for (; i <= upper; i++)
  {
    aValueStr += TCollection_AsciiString((Standard_Integer) array->Value(i));
    aValueStr += ' ';
  }
  XmlObjMgt::SetStringValue (theTarget, aValueStr.ToCString(), Standard_True);
}
