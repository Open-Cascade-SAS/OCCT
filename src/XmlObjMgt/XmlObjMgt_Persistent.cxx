// Created on: 2001-07-17
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
#include <XmlObjMgt_Document.hxx>
#include <XmlObjMgt_Persistent.hxx>

//=======================================================================
//function : XmlObjMgt_Persistent
//purpose  : empty constructor
//=======================================================================
XmlObjMgt_Persistent::XmlObjMgt_Persistent ()
     : myID (0)
{}

//=======================================================================
//function : XmlObjMgt_Persistent
//purpose  : 
//=======================================================================
XmlObjMgt_Persistent::XmlObjMgt_Persistent (const XmlObjMgt_Element& theElement)
     : myElement (theElement), myID (0)
{
  if (theElement != NULL)
    theElement.getAttribute(XmlObjMgt::IdString()).GetInteger(myID);
}

//=======================================================================
//function : XmlObjMgt_Persistent
//purpose  : 
//=======================================================================
XmlObjMgt_Persistent::XmlObjMgt_Persistent (const XmlObjMgt_Element& theElement,
                                            const XmlObjMgt_DOMString& theRef)
     : myID (0)
{
  if (theElement != NULL) {
    Standard_Integer aRefID;
    if (theElement.getAttribute (theRef).GetInteger (aRefID)) {
      myElement = XmlObjMgt::FindChildElement (theElement, aRefID);
      if (myElement != NULL)
        myElement.getAttribute(XmlObjMgt::IdString()).GetInteger(myID);
    }
  }
}

//=======================================================================
//function : CreateElement
//purpose  : <theType id="theID"/>
//=======================================================================
void XmlObjMgt_Persistent::CreateElement (XmlObjMgt_Element&         theParent,
                                          const XmlObjMgt_DOMString& theType,
                                          const Standard_Integer     theID)
{
//AGV  XmlObjMgt_Document& anOwnerDoc =
//AGV    (XmlObjMgt_Document&)theParent.getOwnerDocument();
  XmlObjMgt_Document anOwnerDoc =
    XmlObjMgt_Document (theParent.getOwnerDocument());
  myElement = anOwnerDoc.createElement (theType);
  theParent.appendChild (myElement);
  SetId (theID);
}

//=======================================================================
//function : SetId
//purpose  : 
//=======================================================================
void XmlObjMgt_Persistent::SetId(const Standard_Integer theId)
{
  myID = theId;
  myElement.setAttribute (XmlObjMgt::IdString(), theId);
}
