// Created on: 2008-03-05
// Created by: Vlad ROMASHKO
// Copyright (c) 2008-2012 OPEN CASCADE SAS
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


#include <XmlMFunction_ScopeDriver.ixx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Document.hxx>
#include <LDOM_MemManager.hxx>

#include <TColStd_ListOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>

#include <TDF_Tool.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelList.hxx>
#include <TDF_ListIteratorOfLabelList.hxx>

#include <TFunction_Scope.hxx>
#include <TFunction_DoubleMapIteratorOfDoubleMapOfIntegerLabel.hxx>

IMPLEMENT_DOMSTRING (LastIDIndex,    "lastid")
IMPLEMENT_DOMSTRING (LastLabelIndex, "lastlabel")

IMPLEMENT_DOMSTRING (ExtString,      "string")

//=======================================================================
//function : XmlMFunction_ScopeDriver
//purpose  : Constructor
//=======================================================================
XmlMFunction_ScopeDriver::XmlMFunction_ScopeDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, NULL)
{

}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMFunction_ScopeDriver::NewEmpty() const
{
  return (new TFunction_Scope());
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMFunction_ScopeDriver::Paste(const XmlObjMgt_Persistent&  theSource,
						 const Handle(TDF_Attribute)& theTarget,
						 XmlObjMgt_RRelocationTable&  ) const
{
  Handle(TFunction_Scope) S = Handle(TFunction_Scope)::DownCast(theTarget);
  TColStd_ListOfInteger   IDs;
  TDF_LabelList           Labels;

  Standard_Integer aFirstInd, aLastInd, aValue, ind, nbIDs = 0, nbLabels = 0;
  const XmlObjMgt_Element& anElement = theSource;

  // IDs
  // ===

  // Read the FirstIndex; if the attribute is absent initialize to 1
  aFirstInd = 1;

  // Read the LastIndex; the attribute should present
  if (!anElement.getAttribute(::LastIDIndex()).GetInteger(aLastInd)) 
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the last index"
                                 " for Scope attribute");
    WriteMessage (aMessageString);
    return Standard_False;
  }
  nbIDs = aLastInd - aFirstInd + 1;

  if (aFirstInd == aLastInd) 
  {
    Standard_Integer anInteger;
    if (!XmlObjMgt::GetStringValue(anElement).GetInteger(anInteger)) 
    {
      TCollection_ExtendedString aMessageString =
        TCollection_ExtendedString("Cannot retrieve integer member"
                                   " for Scope attribute as \"");
      WriteMessage (aMessageString);
      return Standard_False;
    }
    IDs.Append(anInteger);
  }
  else 
  {
    Standard_CString aValueStr =
      Standard_CString(XmlObjMgt::GetStringValue(anElement).GetString());
    
    for (ind = aFirstInd; ind <= aLastInd; ind++)
    {
      if (!XmlObjMgt::GetInteger(aValueStr, aValue)) 
      {
        TCollection_ExtendedString aMessageString =
          TCollection_ExtendedString("Cannot retrieve integer member"
                                     " for Scope attribute as \"")
            + aValueStr + "\"";
        WriteMessage (aMessageString);
        return Standard_False;
      }
      IDs.Append(aValue);
    }
  }


  // Labels
  // ======

  aFirstInd = 1;

  // Read the LastIndex; the attribute should present
  if (!anElement.getAttribute(::LastLabelIndex()).GetInteger(aLastInd)) 
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the last index"
                                 " for Scope attribute");
    WriteMessage (aMessageString);
    return Standard_False;
  }
  nbLabels = aLastInd - aFirstInd + 1;

  if (!anElement.hasChildNodes())
  {
    TCollection_ExtendedString aMessageString = 
      TCollection_ExtendedString("Cannot retrieve an array of labels");
    WriteMessage (aMessageString);
    return Standard_False;
  }

  LDOM_Node aCurNode = anElement.getFirstChild()/*.getNextSibling().getNextSibling()*/;
  LDOM_Element* aCurElement = (LDOM_Element*)&aCurNode;
  XmlObjMgt_DOMString aValueStr;
  while (*aCurElement != anElement.getLastChild())
  {
    aValueStr = XmlObjMgt::GetStringValue( *aCurElement );
    if (aValueStr == NULL)
    {
      aCurNode = aCurElement->getNextSibling();
      aCurElement = (LDOM_Element*)&aCurNode;
      continue;
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
      TDF_Tool::Label(S->Label().Data(), anEntry, tLab, Standard_True);
    }
    Labels.Append(tLab);
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
    TDF_Tool::Label(S->Label().Data(), anEntry, tLab, Standard_True);
  }
  Labels.Append(tLab);

  // Check equality of lengths of the list of IDs & Labels.
  if (nbIDs != nbLabels)
  {
    TCollection_ExtendedString aMessage =
      TCollection_ExtendedString ("Numbers of IDs & Labels are different");
    WriteMessage (aMessage);
    return Standard_False;
  }

  // Set IDs & Labels into the Scope attribute
  int freeID = 0;
  TColStd_ListIteratorOfListOfInteger itri(IDs);
  TDF_ListIteratorOfLabelList         itrl(Labels);
  for (; itri.More(); itri.Next(), itrl.Next())
  {
    int ID = itri.Value();
    if (ID > freeID)
      freeID = ID;
    S->ChangeFunctions().Bind(ID, itrl.Value());
  }
  freeID++;
  S->SetFreeID(freeID);

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void XmlMFunction_ScopeDriver::Paste (const Handle(TDF_Attribute)& theSource,
				      XmlObjMgt_Persistent&        theTarget,
				      XmlObjMgt_SRelocationTable&  ) const
{
  Handle(TFunction_Scope) S = Handle(TFunction_Scope)::DownCast(theSource);

  // IDs
  // ===

  theTarget.Element().setAttribute(::LastIDIndex(), S->GetFunctions().Extent());

  TCollection_AsciiString aValueStr;
  TFunction_DoubleMapIteratorOfDoubleMapOfIntegerLabel itrd(S->GetFunctions());
  for (; itrd.More(); itrd.Next())
  {
    const Standard_Integer ID = itrd.Key1();
    aValueStr += TCollection_AsciiString(ID);
    aValueStr += ' ';
  }
  aValueStr += "\n";

  XmlObjMgt::SetStringValue (theTarget, aValueStr.ToCString(), Standard_True);


  // Labels
  // ======

  XmlObjMgt_Element& anElement = theTarget;
  anElement.setAttribute(::LastLabelIndex(), S->GetFunctions().Extent());
  
  XmlObjMgt_Document aDoc = anElement.getOwnerDocument().Doc();
  
  for (itrd.Initialize(S->GetFunctions()); itrd.More(); itrd.Next())
  {
    TDF_Label L = itrd.Key2();

    TCollection_AsciiString anEntry;
    TDF_Tool::Entry(L, anEntry);

    XmlObjMgt_DOMString aDOMString;
    XmlObjMgt::SetTagEntryString (aDOMString, anEntry);
    XmlObjMgt_Element aCurTarget = aDoc.createElement( ::ExtString() );
    XmlObjMgt::SetStringValue (aCurTarget, aDOMString, Standard_True);
    anElement.appendChild( aCurTarget );
  }
}
