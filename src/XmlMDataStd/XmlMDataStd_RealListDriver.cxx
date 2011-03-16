// File:        XmlMDataStd_RealListDriver.cxx
// Created:     May 29 11:40:00 2007
// Author:      Vlad Romashko
//  	    	<vladislav.romashko@opencascade.com>
// Copyright:   Open CASCADE

#include <XmlMDataStd_RealListDriver.ixx>
#include <TDataStd_RealList.hxx>
#include <TColStd_ListIteratorOfListOfReal.hxx>
#include <XmlObjMgt.hxx>

IMPLEMENT_DOMSTRING (FirstIndexString, "first")
IMPLEMENT_DOMSTRING (LastIndexString,  "last")

//=======================================================================
//function : XmlMDataStd_RealListDriver
//purpose  : Constructor
//=======================================================================
XmlMDataStd_RealListDriver::XmlMDataStd_RealListDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
     : XmlMDF_ADriver (theMsgDriver, NULL)
{

}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMDataStd_RealListDriver::NewEmpty() const
{
  return new TDataStd_RealList();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMDataStd_RealListDriver::Paste(const XmlObjMgt_Persistent&  theSource,
						   const Handle(TDF_Attribute)& theTarget,
						   XmlObjMgt_RRelocationTable&  ) const
{
  Standard_Real aValue;
  Standard_Integer aFirstInd, aLastInd, ind;
  const XmlObjMgt_Element& anElement = theSource;

  // Read the FirstIndex; if the attribute is absent initialize to 1
  XmlObjMgt_DOMString aFirstIndex= anElement.getAttribute(::FirstIndexString());
  if (aFirstIndex == NULL)
    aFirstInd = 1;
  else if (!aFirstIndex.GetInteger(aFirstInd)) 
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the first index"
                                 " for RealList attribute as \"")
        + aFirstIndex + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  // Read the LastIndex; the attribute should be present
  if (!anElement.getAttribute(::LastIndexString()).GetInteger(aLastInd)) 
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the last index"
                                 " for RealList attribute as \"")
        + aFirstIndex + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  Handle(TDataStd_RealList) aRealList = Handle(TDataStd_RealList)::DownCast(theTarget);

  // Check the type of LDOMString
  const XmlObjMgt_DOMString& aString = XmlObjMgt::GetStringValue(anElement);
  if (aString.Type() == LDOMBasicString::LDOM_Integer) 
  {
    if (aFirstInd == aLastInd) 
    {
      Standard_Integer anIntValue;
      if (aString.GetInteger(anIntValue))
        aRealList->Append(Standard_Real(anIntValue));
    } 
    else 
    {
      TCollection_ExtendedString aMessageString =
        TCollection_ExtendedString("Cannot retrieve array of real members"
                                   " for RealList attribute from Integer \"")
        + aString + "\"";
      WriteMessage (aMessageString);
      return Standard_False;
    }
  } 
  else 
  {
    Standard_CString aValueStr = Standard_CString(aString.GetString());
    for (ind = aFirstInd; ind <= aLastInd; ind++)
    {
      if (!XmlObjMgt::GetReal(aValueStr, aValue)) {
        TCollection_ExtendedString aMessageString =
          TCollection_ExtendedString("Cannot retrieve real member"
                                     " for RealList attribute as \"")
            + aValueStr + "\"";
        WriteMessage (aMessageString);
        return Standard_False;
      }
      aRealList->Append(aValue);
    }
  }

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void XmlMDataStd_RealListDriver::Paste(const Handle(TDF_Attribute)& theSource,
				       XmlObjMgt_Persistent&        theTarget,
				       XmlObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_RealList) aRealList = Handle(TDataStd_RealList)::DownCast(theSource);

  Standard_Integer anU = aRealList->Extent();
  TCollection_AsciiString aValueStr;

  theTarget.Element().setAttribute(::LastIndexString(), anU);
  if (anU >= 1)
  {
    TColStd_ListIteratorOfListOfReal itr(aRealList->List());
    for (; itr.More(); itr.Next())
    {
      aValueStr += TCollection_AsciiString(itr.Value());
      aValueStr += ' ';
    }
  }
  // No occurrence of '&', '<' and other irregular XML characters
  XmlObjMgt::SetStringValue (theTarget, aValueStr.ToCString(), Standard_True);
}
