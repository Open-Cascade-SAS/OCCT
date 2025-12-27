// Created on: 2000-09-27
// Created by: Pavel TELKOV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#include <XCAFDoc_GraphNode.hxx>

#include <Standard_Dump.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_DataSet.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFDoc_GraphNode, TDF_Attribute)

//     class  methods working on the node:
//     ===================================
//=================================================================================================

bool XCAFDoc_GraphNode::Find(const TDF_Label& L, occ::handle<XCAFDoc_GraphNode>& G)
{
  return L.FindAttribute(XCAFDoc_GraphNode::GetDefaultGraphID(), G);
}

//=======================================================================
// GraphNode : GetDefaultGraphID
// purpose  : Static method to get the default ID of a GraphNode
//=======================================================================

const Standard_GUID& XCAFDoc_GraphNode::GetDefaultGraphID()
{
  static Standard_GUID XCAFDoc_GraphNodeID("efd212f5-6dfd-11d4-b9c8-0060b0ee281b");
  return XCAFDoc_GraphNodeID;
}

//=======================================================================
// GraphNode : Set
// purpose  : Finds or creates a GraphNode attribute with default ID
//=======================================================================

occ::handle<XCAFDoc_GraphNode> XCAFDoc_GraphNode::Set(const TDF_Label& L)
{
  occ::handle<XCAFDoc_GraphNode> GN;
  if (!L.FindAttribute(XCAFDoc_GraphNode::GetDefaultGraphID(), GN))
  {
    GN = new XCAFDoc_GraphNode();
    GN->SetGraphID(XCAFDoc_GraphNode::GetDefaultGraphID());
    L.AddAttribute(GN);
  }
  return GN;
}

//=======================================================================
// function : Set
// purpose  : Finds or creates a GraphNode  attribute with explicit ID
//         : a driver for it
//=======================================================================

occ::handle<XCAFDoc_GraphNode> XCAFDoc_GraphNode::Set(const TDF_Label&     L,
                                                 const Standard_GUID& explicitID)
{
  occ::handle<XCAFDoc_GraphNode> GN;
  if (!L.FindAttribute(explicitID, GN))
  {
    GN = new XCAFDoc_GraphNode();
    GN->SetGraphID(explicitID);
    L.AddAttribute(GN);
  }
  return GN;
}

//     Instance methods:
//     ================

//=================================================================================================

XCAFDoc_GraphNode::XCAFDoc_GraphNode() {}

//=================================================================================================

void XCAFDoc_GraphNode::SetGraphID(const Standard_GUID& explicitID)
{
  Backup();
  myGraphID = explicitID;
}

//=================================================================================================

int XCAFDoc_GraphNode::SetFather(const occ::handle<XCAFDoc_GraphNode>& F)
{
  Backup();
  int Findex = myFathers.Length();
  myFathers.Append(F);
  return ++Findex;
}

//=================================================================================================

int XCAFDoc_GraphNode::SetChild(const occ::handle<XCAFDoc_GraphNode>& Ch)
{
  Backup();
  int Chindex = myChildren.Length();
  myChildren.Append(Ch);
  return ++Chindex;
}

//=================================================================================================

void XCAFDoc_GraphNode::UnSetFather(const occ::handle<XCAFDoc_GraphNode>& F)
{
  int Findex = FatherIndex(F);
  if (Findex != 0)
  {
    F->UnSetChildlink(this);
    UnSetFatherlink(F);
  }
}

//=================================================================================================

void XCAFDoc_GraphNode::UnSetFather(const int Findex)
{
  if (Findex != 0)
  {
    UnSetFather(GetFather(Findex));
  }
}

//=================================================================================================

void XCAFDoc_GraphNode::UnSetFatherlink(const occ::handle<XCAFDoc_GraphNode>& F)
{
  Backup();
  int Findex = FatherIndex(F);
  if (Findex != 0)
  {
    myFathers.Remove(Findex);
  }
}

//=================================================================================================

void XCAFDoc_GraphNode::UnSetChild(const occ::handle<XCAFDoc_GraphNode>& Ch)
{
  int Chindex = ChildIndex(Ch);
  if (Chindex != 0)
  {
    Ch->UnSetFatherlink(this);
    UnSetChildlink(Ch);
  }
}

//=================================================================================================

void XCAFDoc_GraphNode::UnSetChild(const int Chindex)
{
  if (Chindex != 0)
  {
    UnSetChild(GetChild(Chindex));
  }
}

//=================================================================================================

void XCAFDoc_GraphNode::UnSetChildlink(const occ::handle<XCAFDoc_GraphNode>& Ch)
{
  Backup();
  int Chindex = ChildIndex(Ch);
  if (Chindex != 0)
  {
    myChildren.Remove(Chindex);
  }
}

//=================================================================================================

occ::handle<XCAFDoc_GraphNode> XCAFDoc_GraphNode::GetFather(const int Findex) const
{
  occ::handle<XCAFDoc_GraphNode> F = myFathers.Value(Findex);
  return F;
}

//=================================================================================================

occ::handle<XCAFDoc_GraphNode> XCAFDoc_GraphNode::GetChild(const int Chindex) const
{
  occ::handle<XCAFDoc_GraphNode> Ch = myChildren.Value(Chindex);
  return Ch;
}

//=================================================================================================

int XCAFDoc_GraphNode::FatherIndex(const occ::handle<XCAFDoc_GraphNode>& F) const
{
  int Findex = 0;
  if (NbFathers() != 0)
  {
    for (Findex = 1; Findex <= NbFathers(); Findex++)
    {
      if (F == myFathers.Value(Findex))
      {
        return Findex;
      }
    }
  }
  return 0;
}

//=================================================================================================

int XCAFDoc_GraphNode::ChildIndex(const occ::handle<XCAFDoc_GraphNode>& Ch) const
{
  int Chindex;
  if (NbChildren() != 0)
  {
    for (Chindex = 1; Chindex <= NbChildren(); Chindex++)
    {
      if (Ch == myChildren.Value(Chindex))
      {
        return Chindex;
      }
    }
  }
  return 0;
}

//=================================================================================================

bool XCAFDoc_GraphNode::IsFather(const occ::handle<XCAFDoc_GraphNode>& Ch) const
{
  if (ChildIndex(Ch))
    return true;
  return false;
}

//=================================================================================================

bool XCAFDoc_GraphNode::IsChild(const occ::handle<XCAFDoc_GraphNode>& F) const
{
  if (FatherIndex(F))
    return true;
  return false;
}

//=================================================================================================

int XCAFDoc_GraphNode::NbFathers() const
{
  return myFathers.Length();
}

//=================================================================================================

int XCAFDoc_GraphNode::NbChildren() const
{
  return myChildren.Length();
}

//     Implementation of Attribute methods:
//     ===================================

//=================================================================================================

const Standard_GUID& XCAFDoc_GraphNode::ID() const
{
  return myGraphID;
}

//=================================================================================================

void XCAFDoc_GraphNode::Restore(const occ::handle<TDF_Attribute>& other)
{
  occ::handle<XCAFDoc_GraphNode> F = occ::down_cast<XCAFDoc_GraphNode>(other);
  myFathers                   = F->myFathers;
  myChildren                  = F->myChildren;
  myGraphID                   = F->myGraphID;
}

//=================================================================================================

void XCAFDoc_GraphNode::Paste(const occ::handle<TDF_Attribute>&       into,
                              const occ::handle<TDF_RelocationTable>& RT) const
{
  occ::handle<XCAFDoc_GraphNode> intof = occ::down_cast<XCAFDoc_GraphNode>(into);
  occ::handle<XCAFDoc_GraphNode> func;
  int          i = 1;
  for (; i <= NbFathers(); i++)
  {
    if (!RT->HasRelocation(myFathers(i), func) && RT->AfterRelocate())
    {
      func.Nullify();
    }
    if (!func.IsNull())
    {
      intof->SetFather(func);
    }
  }

  i = 1;
  for (; i <= NbChildren(); i++)
  {
    if (!RT->HasRelocation(myChildren(i), func) && RT->AfterRelocate())
    {
      func.Nullify();
    }
    if (!func.IsNull())
    {
      intof->SetChild(func);
    }
  }
  intof->SetGraphID(myGraphID);
}

//=================================================================================================

occ::handle<TDF_Attribute> XCAFDoc_GraphNode::NewEmpty() const
{
  occ::handle<XCAFDoc_GraphNode> G = new XCAFDoc_GraphNode();
  G->SetGraphID(myGraphID);
  return G;
}

//=================================================================================================

void XCAFDoc_GraphNode::References(const occ::handle<TDF_DataSet>& aDataSet) const
{
  int          i;
  occ::handle<XCAFDoc_GraphNode> fct;
  for (i = 1; i <= NbChildren(); i++)
  {
    fct = myChildren(i);
    if (!fct.IsNull())
    {
      aDataSet->AddAttribute(fct);
    }
  }
  for (i = 1; i <= NbFathers(); i++)
  {
    fct = myFathers(i);
    if (!fct.IsNull())
    {
      aDataSet->AddAttribute(fct);
    }
  }
}

//=================================================================================================

Standard_OStream& XCAFDoc_GraphNode::Dump(Standard_OStream& anOS) const
{
  TDF_Attribute::Dump(anOS);
  int i = 1;
  if (myFathers.Length() != 0)
  {
    anOS << "  Fathers=";
    for (; i <= NbFathers(); i++)
    {
      if (!myFathers(i)->Label().IsNull())
        myFathers(i)->Label().EntryDump(anOS);
      anOS << std::endl;
    }
  }
  i = 1;
  if (myChildren.Length() != 0)
  {
    anOS << "  Children=";
    for (; i <= NbChildren(); i++)
    {
      if (!myChildren(i)->Label().IsNull())
        myChildren(i)->Label().EntryDump(anOS);
      anOS << std::endl;
    }
  }
  //  anOS<<std::endl;
  return anOS;
}

//=================================================================================================

void XCAFDoc_GraphNode::BeforeForget()
{
  while (myFathers.Length() > 0)
  {
    UnSetFather(1);
  }
  while (myChildren.Length() > 0)
  {
    UnSetChild(1);
  }
}

//=================================================================================================

void XCAFDoc_GraphNode::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDF_Attribute)

  for (NCollection_Sequence<occ::handle<XCAFDoc_GraphNode>>::Iterator anIteratorFather(myFathers); anIteratorFather.More();
       anIteratorFather.Next())
  {
    const occ::handle<XCAFDoc_GraphNode>& aFather = anIteratorFather.Value();
    OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, aFather)
  }

  for (NCollection_Sequence<occ::handle<XCAFDoc_GraphNode>>::Iterator anIteratorChild(myChildren); anIteratorChild.More();
       anIteratorChild.Next())
  {
    const occ::handle<XCAFDoc_GraphNode>& aChild = anIteratorChild.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, aChild.get())
  }
}
