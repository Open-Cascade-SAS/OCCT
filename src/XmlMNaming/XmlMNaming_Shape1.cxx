// Created on: 2001-08-01
// Created by: Alexander GRIGORIEV
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


#include <BRep_Tool.hxx>
#include <gp_Pnt.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <XmlMNaming_Shape1.hxx>
#include <XmlObjMgt.hxx>

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
    default:   throw Standard_DomainError("XmlMNaming_Shape1; orientation value without enum term equivalence");
    }
    Standard_CString anIntPtr = (Standard_CString) &aPtr[1];
    if (XmlObjMgt::GetInteger (anIntPtr, myTShapeID) == Standard_False)
      throw Standard_DomainError("XmlMNaming_Shape1; tshape value cannot be initialised by integer");
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
