// Created on: 2005-05-17
// Created by: Eugeny NAPALKOV
// Copyright (c) 2005-2012 OPEN CASCADE SAS
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



#include <BinMXCAFDoc_GraphNodeDriver.ixx>
#include <XCAFDoc_GraphNode.hxx>

//=======================================================================
//function :
//purpose  : 
//=======================================================================
BinMXCAFDoc_GraphNodeDriver::BinMXCAFDoc_GraphNodeDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
     : BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(XCAFDoc_GraphNode)->Name()) {
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMXCAFDoc_GraphNodeDriver::NewEmpty() const {
  return new XCAFDoc_GraphNode();
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Standard_Boolean BinMXCAFDoc_GraphNodeDriver::Paste(const BinObjMgt_Persistent& theSource,
						    const Handle(TDF_Attribute)& theTarget,
						    BinObjMgt_RRelocationTable& theRelocTable) const
{
  Handle(XCAFDoc_GraphNode) aT = Handle(XCAFDoc_GraphNode)::DownCast(theTarget);
  Standard_Integer anID;

  // Read Fathers
  if (! (theSource >> anID))
    return Standard_False;
  while(anID != -1) {
    Handle(XCAFDoc_GraphNode) aNode;
    if(theRelocTable.IsBound(anID)) {
      aNode = Handle(XCAFDoc_GraphNode)::DownCast(theRelocTable.Find(anID));
    } else {
      aNode = Handle(XCAFDoc_GraphNode)::DownCast(aT->NewEmpty());
      theRelocTable.Bind(anID, aNode);
    }
    aT->SetFather(aNode);

    if (! (theSource >> anID))
      return Standard_False;    
  }

  // Read Children
  if (! (theSource >> anID))
    return Standard_False;
  while(anID != -1) {
    Handle(XCAFDoc_GraphNode) aNode;
    if(theRelocTable.IsBound(anID)) {
      aNode = Handle(XCAFDoc_GraphNode)::DownCast(theRelocTable.Find(anID));
    } else {
      aNode = Handle(XCAFDoc_GraphNode)::DownCast(aT->NewEmpty());
      theRelocTable.Bind(anID, aNode);
    }
    aT->SetChild(aNode);

    if (! (theSource >> anID))
      return Standard_False;    
  }

  // Graph id
  Standard_GUID aGUID;
  if (! (theSource >> aGUID))
    return Standard_False;
  aT->SetGraphID(aGUID);


  return Standard_True;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
void BinMXCAFDoc_GraphNodeDriver::Paste(const Handle(TDF_Attribute)& theSource,
                                        BinObjMgt_Persistent& theTarget,
                                        BinObjMgt_SRelocationTable& theRelocTable) const
{
  Handle(XCAFDoc_GraphNode) aS = Handle(XCAFDoc_GraphNode)::DownCast(theSource);
  Standard_Integer i, aNb, anID;

  // Write fathers
  aNb = aS->NbFathers();
  for(i = 1; i <= aNb; i++) {
    Handle(XCAFDoc_GraphNode) aNode = aS->GetFather(i);
    anID = theRelocTable.Add(aNode);
    theTarget << anID;
  }
  theTarget.PutInteger(-1);

  // Write children
  aNb = aS->NbChildren();
  for(i = 1; i <= aNb; i++) {
    Handle(XCAFDoc_GraphNode) aNode = aS->GetChild(i);
    anID = theRelocTable.Add(aNode);
    theTarget << anID;
  }
  theTarget.PutInteger(-1);

  // Graph id
  theTarget << aS->ID();
}

