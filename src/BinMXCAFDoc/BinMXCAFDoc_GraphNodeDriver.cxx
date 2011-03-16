// File:        BinMXCAFDoc_GraphNodeDriver.cxx
// Created:     Tue May 17 14:54:18 2005
// Author:      Eugeny NAPALKOV <eugeny.napalkov@opencascade.com>
// Copyright:   Open CasCade S.A. 2005


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

