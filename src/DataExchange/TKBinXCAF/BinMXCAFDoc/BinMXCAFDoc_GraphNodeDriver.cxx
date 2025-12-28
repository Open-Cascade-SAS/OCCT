// Created on: 2005-05-17
// Created by: Eugeny NAPALKOV
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#include <BinMXCAFDoc_GraphNodeDriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <XCAFDoc_GraphNode.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMXCAFDoc_GraphNodeDriver, BinMDF_ADriver)

//=================================================================================================

BinMXCAFDoc_GraphNodeDriver::BinMXCAFDoc_GraphNodeDriver(
  const occ::handle<Message_Messenger>& theMsgDriver)
    : BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(XCAFDoc_GraphNode)->Name())
{
}

//=================================================================================================

occ::handle<TDF_Attribute> BinMXCAFDoc_GraphNodeDriver::NewEmpty() const
{
  return new XCAFDoc_GraphNode();
}

//=================================================================================================

bool BinMXCAFDoc_GraphNodeDriver::Paste(const BinObjMgt_Persistent&  theSource,
                                                    const occ::handle<TDF_Attribute>& theTarget,
                                                    BinObjMgt_RRelocationTable& theRelocTable) const
{
  occ::handle<XCAFDoc_GraphNode> aT = occ::down_cast<XCAFDoc_GraphNode>(theTarget);
  int          anID;

  // Read Fathers
  if (!(theSource >> anID))
    return false;
  while (anID != -1)
  {
    occ::handle<XCAFDoc_GraphNode> aNode;
    if (theRelocTable.IsBound(anID))
    {
      aNode = occ::down_cast<XCAFDoc_GraphNode>(theRelocTable.Find(anID));
    }
    else
    {
      aNode = occ::down_cast<XCAFDoc_GraphNode>(aT->NewEmpty());
      theRelocTable.Bind(anID, aNode);
    }
    aT->SetFather(aNode);

    if (!(theSource >> anID))
      return false;
  }

  // Read Children
  if (!(theSource >> anID))
    return false;
  while (anID != -1)
  {
    occ::handle<XCAFDoc_GraphNode> aNode;
    if (theRelocTable.IsBound(anID))
    {
      aNode = occ::down_cast<XCAFDoc_GraphNode>(theRelocTable.Find(anID));
    }
    else
    {
      aNode = occ::down_cast<XCAFDoc_GraphNode>(aT->NewEmpty());
      theRelocTable.Bind(anID, aNode);
    }
    aT->SetChild(aNode);

    if (!(theSource >> anID))
      return false;
  }

  // Graph id
  Standard_GUID aGUID;
  if (!(theSource >> aGUID))
    return false;
  aT->SetGraphID(aGUID);

  return true;
}

//=================================================================================================

void BinMXCAFDoc_GraphNodeDriver::Paste(const occ::handle<TDF_Attribute>& theSource,
                                        BinObjMgt_Persistent&        theTarget,
                                        NCollection_IndexedMap<occ::handle<Standard_Transient>>&  theRelocTable) const
{
  occ::handle<XCAFDoc_GraphNode> aS = occ::down_cast<XCAFDoc_GraphNode>(theSource);
  int          i, aNb, anID;

  // Write fathers
  aNb = aS->NbFathers();
  for (i = 1; i <= aNb; i++)
  {
    occ::handle<XCAFDoc_GraphNode> aNode = aS->GetFather(i);
    anID                            = theRelocTable.Add(aNode);
    theTarget << anID;
  }
  theTarget.PutInteger(-1);

  // Write children
  aNb = aS->NbChildren();
  for (i = 1; i <= aNb; i++)
  {
    occ::handle<XCAFDoc_GraphNode> aNode = aS->GetChild(i);
    anID                            = theRelocTable.Add(aNode);
    theTarget << anID;
  }
  theTarget.PutInteger(-1);

  // Graph id
  theTarget << aS->ID();
}
