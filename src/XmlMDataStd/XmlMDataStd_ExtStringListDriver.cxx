// File:        XmlMDataStd_ExtStringListDriver.cxx
// Created:     May 29 11:40:00 2007
// Author:      Vlad Romashko
//  	    	<vladislav.romashko@opencascade.com>
// Copyright:   Open CASCADE

#include <XmlMDataStd_ExtStringListDriver.ixx>
#include <TDataStd_ExtStringList.hxx>
#include <TDataStd_ListIteratorOfListOfExtendedString.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Document.hxx>
#include <LDOM_MemManager.hxx>

IMPLEMENT_DOMSTRING (FirstIndexString, "first")
IMPLEMENT_DOMSTRING (LastIndexString,  "last")
IMPLEMENT_DOMSTRING (ExtString,        "string")

//=======================================================================
//function : XmlMDataStd_ExtStringListDriver
//purpose  : Constructor
//=======================================================================
XmlMDataStd_ExtStringListDriver::XmlMDataStd_ExtStringListDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
     : XmlMDF_ADriver (theMsgDriver, NULL)
{

}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMDataStd_ExtStringListDriver::NewEmpty() const
{
  return new TDataStd_ExtStringList();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMDataStd_ExtStringListDriver::Paste(const XmlObjMgt_Persistent&  theSource,
							const Handle(TDF_Attribute)& theTarget,
							XmlObjMgt_RRelocationTable&  ) const
{
  Standard_Integer aFirstInd, aLastInd;
  const XmlObjMgt_Element& anElement = theSource;

  // Read the FirstIndex; if the attribute is absent initialize to 1
  XmlObjMgt_DOMString aFirstIndex= anElement.getAttribute(::FirstIndexString());
  if (aFirstIndex == NULL)
    aFirstInd = 1;
  else if (!aFirstIndex.GetInteger(aFirstInd)) 
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the first index"
                                 " for ExtStringList attribute as \"")
        + aFirstIndex + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  // Read the LastIndex; the attribute should present
  if (!anElement.getAttribute(::LastIndexString()).GetInteger(aLastInd)) 
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the last index"
                                 " for ExtStringList attribute as \"")
        + aFirstIndex + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  Handle(TDataStd_ExtStringList) anExtStringList = Handle(TDataStd_ExtStringList)::DownCast(theTarget);
  
  if (!anElement.hasChildNodes())
  {
    TCollection_ExtendedString aMessageString = 
      TCollection_ExtendedString("Cannot retrieve a list of extended strings");
    WriteMessage (aMessageString);
    return Standard_False;
  }

  LDOM_Node aCurNode = anElement.getFirstChild();
  LDOM_Element* aCurElement = (LDOM_Element*)&aCurNode;
  TCollection_ExtendedString aValueStr;
  while (*aCurElement != anElement.getLastChild())
  {
    XmlObjMgt::GetExtendedString( *aCurElement, aValueStr );
    anExtStringList->Append(aValueStr);
    aCurNode = aCurElement->getNextSibling();
    aCurElement = (LDOM_Element*)&aCurNode;
  }

  XmlObjMgt::GetExtendedString( *aCurElement, aValueStr );
  anExtStringList->Append(aValueStr);

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void XmlMDataStd_ExtStringListDriver::Paste(const Handle(TDF_Attribute)& theSource,
				       XmlObjMgt_Persistent&        theTarget,
				       XmlObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_ExtStringList) anExtStringList = Handle(TDataStd_ExtStringList)::DownCast(theSource);

  Standard_Integer anU = anExtStringList->Extent();
  XmlObjMgt_Element& anElement = theTarget;
  anElement.setAttribute(::LastIndexString(), anU);
  
  XmlObjMgt_Document aDoc = anElement.getOwnerDocument().Doc();
  
  TDataStd_ListIteratorOfListOfExtendedString itr(anExtStringList->List());
  for (; itr.More(); itr.Next())
  {
    const TCollection_ExtendedString& aValueStr = itr.Value();
    XmlObjMgt_Element aCurTarget = aDoc.createElement( ::ExtString() );
    XmlObjMgt::SetExtendedString( aCurTarget, aValueStr );
    anElement.appendChild( aCurTarget );
  }
}
