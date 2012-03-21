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


#include <XmlMDataStd_ReferenceListDriver.ixx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Document.hxx>
#include <LDOM_MemManager.hxx>

#include <TDF_Tool.hxx>
#include <TDF_Label.hxx>
#include <TDataStd_ReferenceList.hxx>
#include <TDF_ListIteratorOfLabelList.hxx>

IMPLEMENT_DOMSTRING (FirstIndexString, "first")
IMPLEMENT_DOMSTRING (LastIndexString,  "last")
IMPLEMENT_DOMSTRING (ExtString,        "string")

//=======================================================================
//function : XmlMDataStd_ReferenceListDriver
//purpose  : Constructor
//=======================================================================
XmlMDataStd_ReferenceListDriver::XmlMDataStd_ReferenceListDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
     : XmlMDF_ADriver (theMsgDriver, NULL)
{

}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMDataStd_ReferenceListDriver::NewEmpty() const
{
  return new TDataStd_ReferenceList();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMDataStd_ReferenceListDriver::Paste(const XmlObjMgt_Persistent&  theSource,
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
                                 " for ReferenceList attribute as \"")
        + aFirstIndex + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  // Read the LastIndex; the attribute should present
  if (!anElement.getAttribute(::LastIndexString()).GetInteger(aLastInd)) 
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the last index"
                                 " for ReferenceList attribute as \"")
        + aFirstIndex + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  Handle(TDataStd_ReferenceList) aReferenceList = Handle(TDataStd_ReferenceList)::DownCast(theTarget);
  
  if (!anElement.hasChildNodes())
  {
    TCollection_ExtendedString aMessageString = 
      TCollection_ExtendedString("Cannot retrieve a list of reference");
    WriteMessage (aMessageString);
    return Standard_False;
  }

  LDOM_Node aCurNode = anElement.getFirstChild();
  LDOM_Element* aCurElement = (LDOM_Element*)&aCurNode;
  XmlObjMgt_DOMString aValueStr;
  while (*aCurElement != anElement.getLastChild())
  {
    aValueStr = XmlObjMgt::GetStringValue( *aCurElement );
    if (aValueStr == NULL)
    {
      WriteMessage ("Cannot retrieve reference string from element");
      return Standard_False;
    }
    TCollection_AsciiString anEntry;
    if (XmlObjMgt::GetTagEntryString (aValueStr, anEntry) == Standard_False)
    {
      TCollection_ExtendedString aMessage =
	TCollection_ExtendedString ("Cannot retrieve reference from \"")
	  + aValueStr + '\"';
      WriteMessage (aMessage);
      return Standard_False;
    }
    // Find label by entry
    TDF_Label tLab; // Null label.
    if (anEntry.Length() > 0)
    {
      TDF_Tool::Label(aReferenceList->Label().Data(), anEntry, tLab, Standard_True);
    }
    aReferenceList->Append(tLab);
    aCurNode = aCurElement->getNextSibling();
    aCurElement = (LDOM_Element*)&aCurNode;
  }

  // Last reference
  aValueStr = XmlObjMgt::GetStringValue( *aCurElement );
  if (aValueStr == NULL)
  {
    WriteMessage ("Cannot retrieve reference string from element");
    return Standard_False;
  }
  TCollection_AsciiString anEntry;
  if (XmlObjMgt::GetTagEntryString (aValueStr, anEntry) == Standard_False)
  {
    TCollection_ExtendedString aMessage =
      TCollection_ExtendedString ("Cannot retrieve reference from \"")
	+ aValueStr + '\"';
    WriteMessage (aMessage);
    return Standard_False;
  }
  // Find label by entry
  TDF_Label tLab; // Null label.
  if (anEntry.Length() > 0)
  {
    TDF_Tool::Label(aReferenceList->Label().Data(), anEntry, tLab, Standard_True);
  }
  aReferenceList->Append(tLab);

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void XmlMDataStd_ReferenceListDriver::Paste(const Handle(TDF_Attribute)& theSource,
					    XmlObjMgt_Persistent&        theTarget,
					    XmlObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_ReferenceList) aReferenceList = Handle(TDataStd_ReferenceList)::DownCast(theSource);
  TDF_Label L = aReferenceList->Label();
  if (L.IsNull())
  {
    WriteMessage ("Label of a ReferenceList is Null.");
    return;
  }

  Standard_Integer anU = aReferenceList->Extent();
  XmlObjMgt_Element& anElement = theTarget;
  anElement.setAttribute(::LastIndexString(), anU);
  
  XmlObjMgt_Document aDoc = anElement.getOwnerDocument().Doc();
  
  TDF_ListIteratorOfLabelList itr(aReferenceList->List());
  for (; itr.More(); itr.Next())
  {
    if (L.IsDescendant(itr.Value().Root()))
    {
      // Internal reference
      TCollection_AsciiString anEntry;
      TDF_Tool::Entry(itr.Value(), anEntry);

      XmlObjMgt_DOMString aDOMString;
      XmlObjMgt::SetTagEntryString (aDOMString, anEntry);
      XmlObjMgt_Element aCurTarget = aDoc.createElement( ::ExtString() );
      XmlObjMgt::SetStringValue (aCurTarget, aDOMString, Standard_True);
      anElement.appendChild( aCurTarget );
    }
  }
}
