// File:        XmlMDataStd_IntegerListDriver.cxx
// Created:     May 29 11:40:00 2007
// Author:      Vlad Romashko
//  	    	<vladislav.romashko@opencascade.com>
// Copyright:   Open CASCADE

#include <XmlMDataStd_IntegerListDriver.ixx>
#include <TDataStd_IntegerList.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <XmlObjMgt.hxx>

IMPLEMENT_DOMSTRING (FirstIndexString, "first")
IMPLEMENT_DOMSTRING (LastIndexString,  "last")

//=======================================================================
//function : XmlMDataStd_IntegerListDriver
//purpose  : Constructor
//=======================================================================
XmlMDataStd_IntegerListDriver::XmlMDataStd_IntegerListDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
     : XmlMDF_ADriver (theMsgDriver, NULL)
{

}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMDataStd_IntegerListDriver::NewEmpty() const
{
  return new TDataStd_IntegerList();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMDataStd_IntegerListDriver::Paste(const XmlObjMgt_Persistent&  theSource,
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
                                 " for IntegerList attribute as \"")
        + aFirstIndex + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  // Read the LastIndex; the attribute should be present
  if (!anElement.getAttribute(::LastIndexString()).GetInteger(aLastInd)) 
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the last index"
                                 " for IntegerList attribute as \"")
        + aFirstIndex + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  Handle(TDataStd_IntegerList) anIntList = Handle(TDataStd_IntegerList)::DownCast(theTarget);
  if (aFirstInd == aLastInd) 
  {
    Standard_Integer anInteger;
    if (!XmlObjMgt::GetStringValue(anElement).GetInteger(anInteger)) 
    {
      TCollection_ExtendedString aMessageString =
        TCollection_ExtendedString("Cannot retrieve integer member"
                                   " for IntegerList attribute as \"");
      WriteMessage (aMessageString);
      return Standard_False;
    }
    anIntList->Append(anInteger);
  }
  else 
  {
    Standard_CString aValueStr = Standard_CString(XmlObjMgt::GetStringValue(anElement).GetString());
    for (ind = aFirstInd; ind <= aLastInd; ind++)
    {
      if (!XmlObjMgt::GetInteger(aValueStr, aValue)) 
      {
        TCollection_ExtendedString aMessageString =
          TCollection_ExtendedString("Cannot retrieve integer member"
                                     " for IntegerList attribute as \"")
            + aValueStr + "\"";
        WriteMessage (aMessageString);
        return Standard_False;
      }
      anIntList->Append(aValue);
    }
  }
  
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void XmlMDataStd_IntegerListDriver::Paste(const Handle(TDF_Attribute)& theSource,
					  XmlObjMgt_Persistent&        theTarget,
					  XmlObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_IntegerList) anIntList = Handle(TDataStd_IntegerList)::DownCast(theSource);

  Standard_Integer anU = anIntList->Extent();
  TCollection_AsciiString aValueStr;

  theTarget.Element().setAttribute(::LastIndexString(), anU);
  if (anU >= 1)
  {
    TColStd_ListIteratorOfListOfInteger itr(anIntList->List());
    for (; itr.More(); itr.Next())
    {
      aValueStr += TCollection_AsciiString(itr.Value());
      aValueStr += ' ';
    }
  }
  // No occurrence of '&', '<' and other irregular XML characters
  XmlObjMgt::SetStringValue (theTarget, aValueStr.ToCString(), Standard_True);
}
