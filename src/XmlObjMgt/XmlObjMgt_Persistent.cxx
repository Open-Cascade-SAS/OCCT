// Created on: 2001-07-17
// Created by: Julia DOROVSKIKH
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

//AGV 130202: Changed prototype LDOM_Node::getOwnerDocument()

#include <XmlObjMgt_Persistent.ixx>
#include <XmlObjMgt_Document.hxx>
#include <XmlObjMgt.hxx>
#include <TCollection_AsciiString.hxx>

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
