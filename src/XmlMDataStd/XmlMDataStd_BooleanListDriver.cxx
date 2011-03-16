// File:        XmlMDataStd_BooleanListDriver.cxx
// Created:     May 29 11:40:00 2007
// Author:      Vlad Romashko
//  	    	<vladislav.romashko@opencascade.com>
// Copyright:   Open CASCADE

#include <XmlMDataStd_BooleanListDriver.ixx>
#include <TDataStd_BooleanList.hxx>
#include <TDataStd_ListIteratorOfListOfByte.hxx>
#include <XmlObjMgt.hxx>

IMPLEMENT_DOMSTRING (FirstIndexString, "first")
IMPLEMENT_DOMSTRING (LastIndexString,  "last")

//=======================================================================
//function : XmlMDataStd_BooleanListDriver
//purpose  : Constructor
//=======================================================================
XmlMDataStd_BooleanListDriver::XmlMDataStd_BooleanListDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
     : XmlMDF_ADriver (theMsgDriver, NULL)
{

}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMDataStd_BooleanListDriver::NewEmpty() const
{
  return new TDataStd_BooleanList();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMDataStd_BooleanListDriver::Paste(const XmlObjMgt_Persistent&  theSource,
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
                                 " for BooleanList attribute as \"")
        + aFirstIndex + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  // Read the LastIndex; the attribute should be present
  if (!anElement.getAttribute(::LastIndexString()).GetInteger(aLastInd)) 
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the last index"
                                 " for BooleanList attribute as \"")
        + aFirstIndex + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  Handle(TDataStd_BooleanList) aBooleanList = Handle(TDataStd_BooleanList)::DownCast(theTarget);
  if (aFirstInd == aLastInd) 
  {
    Standard_Integer anInteger;
    if (!XmlObjMgt::GetStringValue(anElement).GetInteger(anInteger)) 
    {
      TCollection_ExtendedString aMessageString =
        TCollection_ExtendedString("Cannot retrieve integer member"
                                   " for BooleanList attribute as \"");
      WriteMessage (aMessageString);
      return Standard_False;
    }
    aBooleanList->Append(anInteger ? Standard_True : Standard_False);
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
                                     " for BooleanList attribute as \"")
            + aValueStr + "\"";
        WriteMessage (aMessageString);
        return Standard_False;
      }
      aBooleanList->Append(aValue ? Standard_True : Standard_False);
    }
  }
  
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void XmlMDataStd_BooleanListDriver::Paste(const Handle(TDF_Attribute)& theSource,
					  XmlObjMgt_Persistent&        theTarget,
					  XmlObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_BooleanList) aBooleanList = Handle(TDataStd_BooleanList)::DownCast(theSource);

  Standard_Integer anU = aBooleanList->Extent();
  TCollection_AsciiString aValueStr;

  theTarget.Element().setAttribute(::LastIndexString(), anU);
  if (anU >= 1)
  {
    TDataStd_ListIteratorOfListOfByte itr(aBooleanList->List());
    for (; itr.More(); itr.Next())
    {
      aValueStr += TCollection_AsciiString(itr.Value());
      aValueStr += ' ';
    }
  }
  // No occurrence of '&', '<' and other irregular XML characters
  XmlObjMgt::SetStringValue (theTarget, aValueStr.ToCString(), Standard_True);
}
