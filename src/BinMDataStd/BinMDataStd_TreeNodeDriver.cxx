// Created on: 2001-08-24
// Created by: Alexnder GRIGORIEV
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


#include <BinMDataStd_TreeNodeDriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <CDM_MessageDriver.hxx>
#include <Standard_Type.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDF_Attribute.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMDataStd_TreeNodeDriver,BinMDF_ADriver)

//=======================================================================
//function : BinMDataStd_TreeNodeDriver
//purpose  : Constructor
//=======================================================================
BinMDataStd_TreeNodeDriver::BinMDataStd_TreeNodeDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : BinMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMDataStd_TreeNodeDriver::NewEmpty() const
{
  return (new TDataStd_TreeNode());
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
Standard_Boolean BinMDataStd_TreeNodeDriver::Paste
                               (const BinObjMgt_Persistent&  theSource,
                                const Handle(TDF_Attribute)& theTarget,
                                BinObjMgt_RRelocationTable& theRelocTable) const
{
  Handle(TDataStd_TreeNode) aT = Handle(TDataStd_TreeNode)::DownCast(theTarget);

  // read int fields
  Standard_Integer i, aNb;
  for (i = 0 ; i < 4; ++i)
  {
    if (! (theSource >> aNb))
      return Standard_False;
    if (aNb < 0) continue;

    Handle(TDataStd_TreeNode) aNode;
    if (theRelocTable.IsBound(aNb))
      aNode = Handle(TDataStd_TreeNode)::DownCast(theRelocTable.Find(aNb));
    else
    {
      aNode = Handle(TDataStd_TreeNode)::DownCast( aT->NewEmpty() ); // already with tree ID
      theRelocTable.Bind(aNb, aNode);
    }
    switch (i) {
    case 0: aT->SetFather  (aNode); break;
    case 1: aT->SetNext    (aNode); break;  
    case 2: aT->SetPrevious(aNode); break;
    case 3: aT->SetFirst   (aNode); break;
    default: continue;
    }
  }

  // tree id
  Standard_GUID aGUID;
  if (! (theSource >> aGUID))
    return Standard_False;
  aT->SetTreeID(aGUID);

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void BinMDataStd_TreeNodeDriver::Paste
                               (const Handle(TDF_Attribute)& theSource,
                                BinObjMgt_Persistent&       theTarget,
                                BinObjMgt_SRelocationTable& theRelocTable) const
{
  Handle(TDataStd_TreeNode) aS = Handle(TDataStd_TreeNode)::DownCast(theSource);

  // first write int fields
  Standard_Integer i, aNb;
  for (i = 0 ; i < 4; ++i)
  {
    Handle(TDataStd_TreeNode) aNode;
    switch (i) {
    case 0: aNode = aS->Father();   break;
    case 1: aNode = aS->Next();     break;  
    case 2: aNode = aS->Previous(); break;
    case 3: aNode = aS->First();    break;
    default: continue;
    }
    if (aNode.IsNull())
      aNb = -1;
    else
      aNb = theRelocTable.Add(aNode);    // create and/or get index
    theTarget.PutInteger(aNb);
  }

  // tree id
  theTarget << aS->ID();
}
