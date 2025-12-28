// Created on: 1998-12-09
// Created by: Xuan PHAM PHU
// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <TopOpeBRepTool_connexity.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopAbs_State.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_List.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_DataMap.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopOpeBRepTool_TOOL.hxx>

#define M_FORWARD(sta) (sta == TopAbs_FORWARD)
#define M_REVERSED(sta) (sta == TopAbs_REVERSED)
#define M_INTERNAL(sta) (sta == TopAbs_INTERNAL)
#define M_EXTERNAL(sta) (sta == TopAbs_EXTERNAL)

#define FORWARD (1)
#define REVERSED (2)
#define INTERNAL (3)
#define EXTERNAL (4)
#define CLOSING (5)

//=================================================================================================

TopOpeBRepTool_connexity::TopOpeBRepTool_connexity()
    : theItems(1, 5)
{
}

//=================================================================================================

TopOpeBRepTool_connexity::TopOpeBRepTool_connexity(const TopoDS_Shape& Key)
    : theKey(Key),
      theItems(1, 5)
{
}

//=================================================================================================

void TopOpeBRepTool_connexity::SetKey(const TopoDS_Shape& Key)
{
  theKey = Key;
}

//=================================================================================================

const TopoDS_Shape& TopOpeBRepTool_connexity::Key() const
{
  return theKey;
}

/*static int FUN_toI(const TopAbs_Orientation& O)
{
  int Index = 0;
  if      (O == TopAbs_FORWARD)  Index = 1;
  else if (O == TopAbs_REVERSED) Index = 2;
  else if (O == TopAbs_INTERNAL) Index = 3;
  else if (O == TopAbs_EXTERNAL) Index = 0;
  return Index;
}*/

//=================================================================================================

int TopOpeBRepTool_connexity::Item(const int OriKey, NCollection_List<TopoDS_Shape>& Item) const
{
  Item.Clear();
  Item = theItems(OriKey);
  return (Item.Extent());
}

//=================================================================================================

int TopOpeBRepTool_connexity::AllItems(NCollection_List<TopoDS_Shape>& Item) const
{
  Item.Clear();
  for (int i = 1; i <= 4; i++)
  {
    NCollection_List<TopoDS_Shape> copy;
    copy.Assign(theItems.Value(i));
    Item.Append(copy);
  }
  return Item.Extent();
}

//=================================================================================================

void TopOpeBRepTool_connexity::AddItem(const int OriKey, const NCollection_List<TopoDS_Shape>& Item)
{
  NCollection_List<TopoDS_Shape> copy;
  copy.Assign(Item);
  theItems(OriKey).Append(copy);
}

void TopOpeBRepTool_connexity::AddItem(const int OriKey, const TopoDS_Shape& Item)
{
  NCollection_List<TopoDS_Shape> copy;
  copy.Append(Item);
  theItems(OriKey).Append(copy);
}

//=================================================================================================

bool TopOpeBRepTool_connexity::RemoveItem(const int OriKey, const TopoDS_Shape& Item)
{
  NCollection_List<TopoDS_Shape>&          item = theItems.ChangeValue(OriKey);
  NCollection_List<TopoDS_Shape>::Iterator it(item);
  while (it.More())
  {
    if (it.Value().IsEqual(Item))
    {
      item.Remove(it);
      return true;
    }
    else
      it.Next();
  }
  return false;
}

//=================================================================================================

bool TopOpeBRepTool_connexity::RemoveItem(const TopoDS_Shape& Item)
{
  bool removed = false;
  for (int i = 1; i <= 5; i++)
  {
    bool found = RemoveItem(i, Item);
    if (found)
      removed = true;
  }
  return removed;
}

//=================================================================================================

NCollection_List<TopoDS_Shape>& TopOpeBRepTool_connexity::ChangeItem(const int OriKey)
{
  return theItems.ChangeValue(OriKey);
}

//=================================================================================================

bool TopOpeBRepTool_connexity::IsMultiple() const
{
  NCollection_List<TopoDS_Shape> lfound;
  int                            nkeyitem = Item(FORWARD, lfound);
  //  nkeyRitem += Item(INTERNAL,lfound); NOT VALID
  // if key is vertex : key appears F in closing E, only one time
  nkeyitem += Item(CLOSING, lfound);
  bool multiple = (nkeyitem > 1);
  return multiple;
}

//=================================================================================================

bool TopOpeBRepTool_connexity::IsFaulty() const
{
  NCollection_List<TopoDS_Shape> lfound;
  int                            nkeyRintem = Item(FORWARD, lfound);
  int                            nkeyFitem  = Item(REVERSED, lfound);
  bool                           faulty     = (nkeyRintem != nkeyFitem);
  return faulty;
}

//=================================================================================================

int TopOpeBRepTool_connexity::IsInternal(NCollection_List<TopoDS_Shape>& Item) const
{
  Item.Clear();

  // all subshapes of INTERNAL(EXTERNAL) are oriented INTERNAL(EXTERNAL)
  NCollection_List<TopoDS_Shape> lINT;
  lINT.Assign(theItems.Value(INTERNAL));
  NCollection_List<TopoDS_Shape>::Iterator it1(lINT);
  while (it1.More())
  {
    const TopoDS_Shape& item1 = it1.Value();
    TopAbs_Orientation  o1    = item1.Orientation();
    if (!M_INTERNAL(o1))
    {
      it1.Next();
      continue;
    }
    int oKey1 = TopOpeBRepTool_TOOL::OriinSor(theKey, item1.Oriented(TopAbs_FORWARD));
    if (oKey1 != INTERNAL)
      lINT.Remove(it1);
    else
      it1.Next();
  }

  NCollection_List<TopoDS_Shape> lEXT;
  lEXT.Assign(theItems.Value(EXTERNAL));
  NCollection_List<TopoDS_Shape>::Iterator it2(lEXT);
  while (it2.More())
  {
    const TopoDS_Shape& item2 = it2.Value();
    TopAbs_Orientation  o2    = item2.Orientation();
    if (!M_EXTERNAL(o2))
    {
      it2.Next();
      continue;
    }
    int oKey2 = TopOpeBRepTool_TOOL::OriinSor(theKey, item2.Oriented(TopAbs_FORWARD));
    if (oKey2 == INTERNAL)
      lINT.Append(item2);
    it2.Next();
  }

  Item.Append(lINT);
  return Item.Extent();
}
