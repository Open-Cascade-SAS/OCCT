// File:        XmlObjMgt_Persistent.cxx
// Created:     Tue Jul 17 12:30:46 2001
// Author:      Julia DOROVSKIKH <jfa@hotdox.nnov.matra-dtv.fr>
// Copyright:   Matra Datavision 2001
// History:   AGV 130202: Changed prototype LDOM_Node::getOwnerDocument()

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
