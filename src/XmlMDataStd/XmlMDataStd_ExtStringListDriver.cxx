// Created on: 2007-05-29
// Created by: Vlad Romashko
// Copyright (c) 2007-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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
