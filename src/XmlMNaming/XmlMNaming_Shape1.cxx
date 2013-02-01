// Created on: 2001-08-01
// Created by: Alexander GRIGORIEV
// Copyright (c) 2001-2012 OPEN CASCADE SAS
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


#include <XmlMNaming_Shape1.ixx>

#include <XmlObjMgt.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS.hxx>
#include <gp_Pnt.hxx>
#include <BRep_Tool.hxx>

#include <stdio.h>

IMPLEMENT_DOMSTRING (TShapeString,   "tshape")
IMPLEMENT_DOMSTRING (LocationString, "location")

IMPLEMENT_DOMSTRING (XCoordString, "x")
IMPLEMENT_DOMSTRING (YCoordString, "y")
IMPLEMENT_DOMSTRING (ZCoordString, "z")

//=======================================================================
//function : XmlMNaming_Shape1
//purpose  : 
//=======================================================================
XmlMNaming_Shape1::XmlMNaming_Shape1 (XmlObjMgt_Document& theDoc)
     : myTShapeID (0),
       myLocID    (0),
       myOrientation (TopAbs_FORWARD)
{
  myElement = theDoc.createElement(XmlObjMgt_DOMString("shape"));
}

//=======================================================================
//function : XmlMNaming_Shape1
//purpose  : 
//=======================================================================
XmlMNaming_Shape1::XmlMNaming_Shape1 (const XmlObjMgt_Element& theEl)
     : myElement(theEl),
       myTShapeID (0),
       myLocID    (0),
       myOrientation (TopAbs_FORWARD)
{
  if (myElement != NULL)
  {
    myElement.getAttribute(::LocationString()).GetInteger (myLocID);
    XmlObjMgt_DOMString aString = myElement.getAttribute(::TShapeString());
    const char * aPtr = aString.GetString();
    switch (* aPtr) {
    case '+' : myOrientation = TopAbs_FORWARD;  break;
    case '-' : myOrientation = TopAbs_REVERSED; break;
    case 'i' : myOrientation = TopAbs_INTERNAL; break;
    case 'e' : myOrientation = TopAbs_EXTERNAL; break;
    default:       Standard_DomainError::Raise
      ("XmlMNaming_Shape1; orientation value without enum term equivalence");
    }
    Standard_CString anIntPtr = (Standard_CString) &aPtr[1];
    if (XmlObjMgt::GetInteger (anIntPtr, myTShapeID) == Standard_False)
      Standard_DomainError::Raise
        ("XmlMNaming_Shape1; tshape value cannot be initialised by integer");
  }
}

//=======================================================================
//function : Element
//purpose  : 
//=======================================================================
const XmlObjMgt_Element& XmlMNaming_Shape1::Element() const
{
  return myElement;
}

//=======================================================================
//function : Element
//purpose  : 
//=======================================================================
XmlObjMgt_Element& XmlMNaming_Shape1::Element()
{
  return myElement;
}

//=======================================================================
//function : TShapeId
//purpose  : 
//=======================================================================
Standard_Integer XmlMNaming_Shape1::TShapeId() const 
{
  return myTShapeID;
}

//=======================================================================
//function : LocId
//purpose  : 
//=======================================================================
Standard_Integer XmlMNaming_Shape1::LocId() const 
{
  return myLocID;
}

//=======================================================================
//function : Orientation
//purpose  : 
//=======================================================================
TopAbs_Orientation  XmlMNaming_Shape1::Orientation() const 
{
  return myOrientation;
}

//=======================================================================
//function : SetShape
//purpose  : 
//=======================================================================
void  XmlMNaming_Shape1::SetShape (const Standard_Integer       theID,
                                   const Standard_Integer       theLocID,
                                   const TopAbs_Orientation     theOrient)
{
  myTShapeID    = theID;
  myLocID       = theLocID;
  myOrientation = theOrient;

  char aBuffer[16], anOr;
  
  switch (theOrient) {
  case TopAbs_FORWARD   : anOr = '+'; break;
  case TopAbs_REVERSED  : anOr = '-'; break;
  case TopAbs_INTERNAL  : anOr = 'i'; break;
  case TopAbs_EXTERNAL  : anOr = 'e'; break;
  default               : anOr = '\0';
  }
  Sprintf (aBuffer, "%c%i", anOr, theID);
  Element().setAttribute (::TShapeString(), aBuffer);
  if (theLocID > 0)
    Element().setAttribute (::LocationString(), theLocID);
}

//=======================================================================
//function : SetVertex
//purpose  : 
//=======================================================================
void XmlMNaming_Shape1::SetVertex (const TopoDS_Shape& theVertex)
{
  TopoDS_Vertex aV = TopoDS::Vertex(theVertex);
  gp_Pnt aPos = BRep_Tool::Pnt(aV);

  char buf [16];
  Sprintf (buf, "%.8g", aPos.X());
  Element().setAttribute (::XCoordString(), buf);

  Sprintf (buf, "%.8g", aPos.Y());
  Element().setAttribute (::YCoordString(), buf);

  Sprintf (buf, "%.8g", aPos.Z());
  Element().setAttribute (::ZCoordString(), buf);
}
