// Created on: 2001-08-01
// Created by: Julia DOROVSKIKH
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

//AGV 130202: Changed prototype LDOM_Node::getOwnerDocument()

#include <TCollection_AsciiString.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Array1.hxx>
#include <XmlObjMgt_Document.hxx>
#include <XmlObjMgt_DOMString.hxx>

IMPLEMENT_DOMSTRING (LowerString, "lower")
IMPLEMENT_DOMSTRING (UpperString, "upper")
IMPLEMENT_DOMSTRING (IndString,   "index")

//=======================================================================
//function : XmlObjMgt_Array1
//purpose  : Constructor
//=======================================================================

XmlObjMgt_Array1::XmlObjMgt_Array1 (const XmlObjMgt_Element&   theParent,
                                    const XmlObjMgt_DOMString& theName)
     : myElement            (XmlObjMgt::FindChildByName (theParent, theName)),
       myFirst              (1),
       myLast               (0)
{
  if (myElement != NULL) {
    if (!myElement.getAttribute(::LowerString()).GetInteger(myFirst))
      myFirst = 1;
    if (!myElement.getAttribute(::UpperString()).GetInteger (myLast))
      myLast = 1;
  }
}

//=======================================================================
//function : XmlObjMgt_Array1
//purpose  : Constructor
//=======================================================================

XmlObjMgt_Array1::XmlObjMgt_Array1 (const Standard_Integer aFirst, 
                                    const Standard_Integer aLast)
     : myFirst (aFirst), myLast (aLast)
{}

//=======================================================================
//function : CreateArrayElement
//purpose  : Create DOM_Element representing the array, under 'theParent'
//=======================================================================

void XmlObjMgt_Array1::CreateArrayElement (XmlObjMgt_Element&         theParent,
                                           const XmlObjMgt_DOMString& theName)
{
  if (myLast > 0)
  {
//AGV    XmlObjMgt_Document& anOwnerDoc =
//AGV      (XmlObjMgt_Document&)theParent.getOwnerDocument();
    XmlObjMgt_Document anOwnerDoc =
      XmlObjMgt_Document (theParent.getOwnerDocument());
    myElement = anOwnerDoc.createElement (theName);
    theParent.appendChild (myElement);
    if (myLast > 1) {
      myElement.setAttribute (::UpperString(), myLast);
      if (myFirst != 1)
        myElement.setAttribute (::LowerString(), myFirst);
    }
  }
}

//=======================================================================
//function : SetValue
//purpose  : 
//=======================================================================

void XmlObjMgt_Array1::SetValue
            (const Standard_Integer theIndex, XmlObjMgt_Element& theValue)
{
  myElement.appendChild (theValue);
  theValue.setAttribute(::IndString(), theIndex);
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

XmlObjMgt_Element XmlObjMgt_Array1::Value(const Standard_Integer theIndex) const
{
  XmlObjMgt_Element anElem;

  if (theIndex >= myFirst && theIndex <= myLast)
  {
    Standard_Integer ind;
    LDOM_Node aNode = myElement.getFirstChild();
    while (!aNode.isNull())
    {
      if (aNode.getNodeType() == LDOM_Node::ELEMENT_NODE)
      {
        anElem = (XmlObjMgt_Element &) aNode;
        if (anElem.getAttribute(::IndString()).GetInteger(ind))
          if (ind == theIndex)
            break;
      }
      aNode = aNode.getNextSibling();
    }
  }
  return anElem;
}
